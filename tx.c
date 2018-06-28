#include <stdio.h>
#include <stdlib.h>
// #include <itpp/itbase.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>
#include <hackrf.h>
#include "test.h"
#include "header.h"
// #include "cap.h"

// using namespace itpp;
// using namespace std;

#define PI 3.14157

#define DEBUG 1
// #define DEBUG_

#define FSK_DEFAULT_MSG "010100100101011011110011"
#define ISREPEAT 0

typedef enum
{
    TRANSCEIVER_MODE_OFF = 0,
    TRANSCEIVER_MODE_RX = 1,
    TRANSCEIVER_MODE_TX = 2,
    TRANSCEIVER_MODE_SS = 3,
} transceiver_mode_t;

typedef enum
{
    HW_SYNC_MODE_OFF = 0,
    HW_SYNC_MODE_ON = 1,
} hw_sync_mode_t;

/********global************/
volatile uint32_t byte_count = 0;

int limit_num_samples = -1;
uint64_t samples_to_xfer = 0;
size_t bytes_to_xfer = 0;

/**************************/

// Open the HACKRF device
int config_hackrf(const hackrf_device *dev, const int16 *gain)
{
    unsigned int lna_gain = 40; // default value
    unsigned int vga_gain = 40; // default value
    // if (gain != -99)
    //     vga_gain = (gain / 2) * 2;

    int result = hackrf_init();
    if (result != HACKRF_SUCCESS)
    {
        printf("config_hackrf hackrf_init() failed: %s (%d)\n", hackrf_error_name((result)), result);
        // ABORT(-1);
        return (result);
    }

    result = hackrf_open(&dev);
    if (result != HACKRF_SUCCESS)
    {
        printf("config_hackrf hackrf_open() failed: %s (%d)\n", hackrf_error_name((result)), result);
        // ABORT(-1);
        return (result);
    }

    double sampling_rate = SAMP_RATE;
    // Sampling frequency
    result = hackrf_set_sample_rate_manual(dev, sampling_rate, 1);
    if (result != HACKRF_SUCCESS)
    {
        printf("config_hackrf hackrf_sample_rate_set() failed: %s (%d)\n", hackrf_error_name((result)), result);
        // ABORT(-1);
        return (result);
    }

    // Need to make more study in the future. temperily set it 0.
    result = hackrf_set_baseband_filter_bandwidth(dev, 0);
    if (result != HACKRF_SUCCESS)
    {
        printf("config_hackrf hackrf_baseband_filter_bandwidth_set() failed: %s (%d)\n", hackrf_error_name((result)), result);
        // ABORT(-1);
        return (result);
    }

    result = hackrf_set_vga_gain(dev, vga_gain);
    result |= hackrf_set_lna_gain(dev, lna_gain);

    if (result != HACKRF_SUCCESS)
    {
        printf("config_hackrf hackrf_set_vga_gain hackrf_set_lna_gain failed: %s (%d)\n", hackrf_error_name((result)), result);
        // ABORT(-1);
        return (result);
    }

    // Center frequency
    result = hackrf_set_freq(dev, FREQ_CENTRE);
    if (result != HACKRF_SUCCESS)
    {
        printf("config_hackrf hackrf_set_freq() failed: %s (%d)\n", hackrf_error_name((result)), result);
        // ABORT(-1);
        return (result);
    }
    return (result);
}

int8_t *txbufferI;
int8_t *txbufferQ;
int bufferOffset;
char *bits;
int msg_size = 0;

//give data to hackrfrf
int tx_callback(hackrf_transfer *transfer)
{
    printf("a\n");
    // How much to do?
    size_t count = transfer->valid_length;
    printf("%d\n",count);
    int i = 0;
    while (i < count)
    {
        //
        (transfer->buffer)[i++] = txbufferI[bufferOffset]; // I
        (transfer->buffer)[i++] = txbufferQ[bufferOffset]; // Q
        bufferOffset++;
    }
    return 0;
}

int tx(hackrf_device * dev)
{
    int result = 0;
    bits = NULL;
    // if (bits == NULL)
    //     bits = strdup(FSK_DEFAULT_MSG);
    
    FILE * fd = NULL;
    fd = fopen("../dataxx","rb");
    if(fd==NULL)
    {
        printf("cannot open file.\n");
        exit(1);
    }
    
    struct stat statbuf;
    stat("../dataxx", &statbuf);
    int size = statbuf.st_size;
    int8_t in[size];
    int8_t in_opp[size];
    fread(in, 1, size, fd);
    fclose(fd);

    for(int i=0;i<size;i++)
    {
        in_opp[i] = 1-in[i];
        // printf("%2d",in_opp[i]);

    }
    
    txbufferI = malloc(400*size*sizeof(int8_t));
    txbufferQ = malloc(400*size*sizeof(int8_t));

    double angle = 0;
    int tmp=0;
    for(int j=0;j<size;j++)
    {
        for(int p=0;p<400;p++)
        { 
           txbufferI[tmp] = (int8_t)(in[j]*127.0*cos(tmp*2.0*PI/200.0) + in_opp[j]*127.0*sin(tmp*2.0*PI/200.0) );
           txbufferQ[tmp] = (int8_t)(in[j]*127.0*sin(tmp*2.0*PI/200.0) - in_opp[j]*127.0*cos(tmp*2.0*PI/200.0) );
        //    printf("%lf\n",127*cos(tmp*2*PI/200));
        //    printf("%5d%5d,",txbufferI[tmp],txbufferQ[tmp]);
           tmp++;

        }
    }
    printf("%d\n",400*size*sizeof(int8_t)/1024);
    printf("end\n");
    




    struct timeval time_start;
    gettimeofday(&time_start, NULL);

    // #ifdef DEBUG
    printf("time_start:%ld\n", 1000000*time_start.tv_sec+time_start.tv_usec);

    // #endif

    result = hackrf_start_tx(dev, tx_callback, NULL);
    printf("2\n");
    // while( (hackrf_is_streaming(device) == HACKRF_TRUE) &&(do_exit == false) )
    while ((hackrf_is_streaming(dev) == HACKRF_TRUE))
    {
        struct timeval time_now;
        gettimeofday(&time_now, NULL);

        if ((1000000 * (time_now.tv_sec - time_start.tv_sec) + time_now.tv_usec - time_start.tv_usec) > 3000000)
        {
        // #ifdef DEBUG
        // printf("time_now : %ld\n", time_now.tv_usec);
        // printf("time_gap : %ld\n", 1000000 * (time_now.tv_sec - time_start.tv_sec) + time_now.tv_usec - time_start.tv_usec);
        // #endif
            break;
        }
    }

    result = hackrf_stop_tx(dev);
    // printf("b\n");

    if (result != HACKRF_SUCCESS)
    {
        // fprintf(stderr, "hackrf_stop_rx() failed: %s (%d)\n", hackrf_error_name(result), result);
        fprintf(stderr, "hackrf_stop_rx() failed: (%d)\n", result);
    }
    else
    {
        fprintf(stderr, "hackrf_stop_tx() done\n");
    }
    return result;
}

int main()
{
    hackrf_device *hackrf_dev = NULL;
    int result = 0;

    if (config_hackrf(hackrf_dev, 0) == 0)
    {
        printf("HACKRF device FOUND!\n");
    }
    else
    {
        printf("HACKRF device not FOUND!\n");
        //     ABORT(-1);
    }

    //testing
    tx(hackrf_dev);

    result = hackrf_close(hackrf_dev);
    if (result != HACKRF_SUCCESS)
    {
        // fprintf(stderr, "hackrf_close() failed: %s (%d)\n", hackrf_error_name(result), result);
        fprintf(stderr, "hackrf_close() failed: (%d)\n", result);
    }
    else
    {
        fprintf(stderr, "hackrf_close() done\n");
    }
    hackrf_exit();
    fprintf(stderr, "hackrf_exit() done\n");
}
