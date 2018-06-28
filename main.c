#include <stdio.h>
#include <stdlib.h>
// #include <itpp/itbase.h>
#include <sys/time.h>
#include <unistd.h>
// #include <hackrf.h>
#include "receiver/receiver.h"


#define DEBUG 1

typedef enum {
        TRANSCEIVER_MODE_OFF = 0,
        TRANSCEIVER_MODE_RX = 1,
        TRANSCEIVER_MODE_TX = 2,
        TRANSCEIVER_MODE_SS = 3,
} transceiver_mode_t;

typedef enum {
	HW_SYNC_MODE_OFF = 0,
	HW_SYNC_MODE_ON = 1,
} hw_sync_mode_t;

/********global************/


int main()
{
    hackrf_device *hackrf_dev = NULL;
    int result = 0;

    if (config_hackrf(hackrf_dev, 0) ==0)
    {
        cout << "HACKRF device FOUND!\n";
    }
    else
    {
        cout << "HACKRF device not FOUND!\n";
    }

    rx(hackrf_dev);

}
