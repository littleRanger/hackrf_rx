/*******
filename: receiver.c
author:qcc
data:20180627
*/
#include <stdio.h>
#include <stdlib.h>
#include <hackrf.h>
#include "receiver.h"


int8_t hackrf_rx_buf[CAPLENGTH*2];  // used for capture_data() and hackrf rx callback
size_t hackrf_rx_count;

int rx_callback(hackrf_transfer* transfer) {
	size_t bytes_to_write;
    size_t hackrf_rx_count_new = hackrf_rx_count + transfer->valid_length;
    // printf("btw:%d\n",bytes_to_write);
    int count_left = (CAPLENGTH*2) - hackrf_rx_count_new;
    if ( count_left <= 0 ) {
        bytes_to_write = transfer->valid_length + count_left;
    } else {
        bytes_to_write = transfer->valid_length;
    }

    //  cout << transfer->valid_length  << " " << hackrf_rx_count << " " << bytes_to_write << "\n";
    printf("%5d,%5d, %5d\n", transfer->valid_length, hackrf_rx_count, bytes_to_write);
    if (bytes_to_write!=0)
    {
        memcpy( hackrf_rx_buf+hackrf_rx_count, transfer->buffer, bytes_to_write );
        // for (size_t i=0; i<bytes_to_write; i++) {
        //     //  hackrf_rx_buf[hackrf_rx_count+i] = transfer->buffer[i];
        //     printf("%5d", transfer->buffer[i]);
        // }
        hackrf_rx_count = hackrf_rx_count + bytes_to_write;
    }

    return(0);

}



int rx(hackrf_device *dev)
{
    struct timeval time_start;
    int result = 0;
    fd = fopen("./recodefiledemo.file", "wb");

    gettimeofday(&time_start, NULL);

    #ifdef DEBUG
    printf("time_start:%ld\n", 1000000*time_start.tv_sec+time_start.tv_usec);
    #endif
    result = hackrf_start_rx(dev, rx_callback, NULL);
    // printf("2\n");
	// while( (hackrf_is_streaming(device) == HACKRF_TRUE) &&(do_exit == false) )
	while( (hackrf_is_streaming(dev) == HACKRF_TRUE) )
	{
        // printf("3-in\n");
        // if(byte_count > 1000000)
            // break;

        /********
         * original
         *
         * *******/

        // uint32_t byte_count_now;
		struct timeval time_now;
        gettimeofday(&time_now, NULL);


        if( (1000000*(time_now.tv_sec-time_start.tv_sec)+time_now.tv_usec - time_start.tv_usec) > 3000000)
        {
            #ifdef DEBUG
            printf("time_now : %ld\n", time_now.tv_usec);
            printf("time_gap : %ld\n", 1000000*(time_now.tv_sec-time_start.tv_sec)+time_now.tv_usec - time_start.tv_usec);
            #endif
            break;
        }
    }

	result = hackrf_stop_rx(dev);

    if( result != HACKRF_SUCCESS ) {
		// fprintf(stderr, "hackrf_stop_rx() failed: %s (%d)\n", hackrf_error_name(result), result);
		fprintf(stderr, "hackrf_stop_rx() failed: (%d)\n", result);
	}else {
		fprintf(stderr, "hackrf_stop_rx() done\n");
	}
  printf("\n");

	result = hackrf_close(dev);
	if(result != HACKRF_SUCCESS) {
		// fprintf(stderr, "hackrf_close() failed: %s (%d)\n", hackrf_error_name(result), result);
		fprintf(stderr, "hackrf_close() failed: (%d)\n", result);
	} else {
		fprintf(stderr, "hackrf_close() done\n");
	}

	// hackrf_exit();
	fprintf(stderr, "hackrf_exit() done\n");


    return result;
}


int config_hackrf( hackrf_device * & dev, const int16 & gain)
{
    unsigned int lna_gain=40; // default value
    unsigned int vga_gain=40; // default value
    if (gain!=-9999)
        vga_gain = (gain/2)*2;

    int result = hackrf_init();
	if( result != HACKRF_SUCCESS ) {
		printf("config_hackrf hackrf_init() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        return(result);
	}

	result = hackrf_open(&dev);
	if( result != HACKRF_SUCCESS ) {
		printf("config_hackrf hackrf_open() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        return(result);
	}

    double sampling_rate = SAMP_RATE;
    // Sampling frequency
    result = hackrf_set_sample_rate_manual(dev, sampling_rate, 1);
	if( result != HACKRF_SUCCESS ) {
		printf("config_hackrf hackrf_sample_rate_set() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        return(result);
	}

    // Need to make more study in the future. temperily set it 0.
    result = hackrf_set_baseband_filter_bandwidth(dev, 0);
	if( result != HACKRF_SUCCESS ) {
		printf("config_hackrf hackrf_baseband_filter_bandwidth_set() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
		return(result);
	}

    result = hackrf_set_vga_gain(dev, vga_gain);
	result |= hackrf_set_lna_gain(dev, lna_gain);

    if( result != HACKRF_SUCCESS ) {
		printf("config_hackrf hackrf_set_vga_gain hackrf_set_lna_gain failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
		return(result);
	}

    // Center frequency
    result = hackrf_set_freq(dev, FREQ_CENTRE);
    if( result != HACKRF_SUCCESS ) {
        printf("config_hackrf hackrf_set_freq() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        return(result);
    }
    return(result);
}
