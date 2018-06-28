/*******
filename: receiver.c
author:qcc
data:20180627
*/
#include <stdio.h>
#include <stdlib.h>
#include <hackrf.h>
#include "receiver.h"

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