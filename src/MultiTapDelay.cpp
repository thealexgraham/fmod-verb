//
//  MultiTapDelay.cpp
//  PS7
//
//  Created by Alex Graham on 12/16/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: This class is a multi tap delay that can create a variable amount of
 * taps with lengths and amplitudes. The default processing does not use feedback
 */

#include "MultiTapDelay.h"

/*
 * Description: The constructor. Delay times is an array of delay times in seconds and should match the number
 * of taps. Similarly, amps is 0-1 in an array length of num_taps. Max delay is in seconds
 */
MultiTapDelay::MultiTapDelay(int num_taps, float *delay_times, float *amps, float max_delay, int srate) {
    m_delay_max = (float) srate * max_delay; // Convert to samples
    m_num_taps = num_taps;
    m_srate = srate;
    
    // Allocate delay max space
    m_buffer = (float *)malloc(sizeof(float) * m_delay_max);
    for (int i=0; i < m_delay_max; i++) {
        m_buffer[i] = 0;
    }
    
    m_writer = 0;
    // Allocate space for readers and amplitude arrays
    m_readers = (int*) malloc(sizeof(int) * num_taps);
    m_amps = (float*)malloc(sizeof(float) * num_taps);
    
    // Actually initialize readers and amplitude delays
    for (int i=0; i < m_num_taps; i++) {
        // Convert to samples and subtract from delay max (Since reader starts at 0 and buffer is circular)
        m_readers[i] = m_delay_max - (srate * delay_times[i]);
        m_amps[i] = amps[i];
    }

}

/*
 * Description: Set a new delay time and recalculate the reader
 */
void MultiTapDelay::setDelayTime(int tap, float time) {
    
    int time_samp = time * m_srate; // Convert time to samples
    // If it for some reason is greater than the delay max, cap it
    if (time_samp > m_delay_max) {
        time_samp = m_delay_max - 1;
    }
    
    // New reader should be time_samp behind the writer
    m_readers[tap] = m_writer - time_samp;
    
    // If it is before 0, wrap it to before the end of the buffer instead (since it is circular)
    if (m_readers[tap] < 0) {
        m_readers[tap] = m_delay_max - m_readers[tap];
    }
    
    // This shouldn't ever happen, but you never know
    if (m_readers[tap] > m_delay_max) {
        m_readers[tap] = m_readers[tap] - m_delay_max;
    }
}

/*
 * Description: Reads a sample from the specified tap
 */
float MultiTapDelay::read(int tap) {
    return m_buffer[m_readers[tap]];
}

/*
 * Description: Writes the sample at the current delay
 */
void MultiTapDelay::write(float sample) {
    m_buffer[m_writer] = sample;
}

/* 
 * Description: Move all taps forward and circle back
 */
void MultiTapDelay::forward() {
    // Move readers/writers forward
    for (int i=0; i < m_num_taps; i++) {
        m_readers[i]++;
        m_readers[i] %= m_delay_max;

    }
    m_writer++;
    // Wrap around to the beginning if necessary
    m_writer %= m_delay_max;
}

/*
 * Description: Set the amplitude of the given tap
 */
void MultiTapDelay::setAmplitude(int tap, float amp) {
    m_amps[tap] = amp;
}

/*
 * Description: Writes sample, returns current read samples, moves line forward
 */
float MultiTapDelay::process(float sample) {
    float out = 0;

    for (int i=0; i < m_num_taps; i++) {
        
        // Because this is sometimes called in the middle of the delay being changed
        // we need to make sure the reader is not past the end of the buffer
        if (m_readers[i] < 0) {
            m_readers[i] = m_delay_max - m_readers[i];
        }
        // Accumulate the scaled sample from this delay with the rest
        out += this->read(i) * m_amps[i];
    }
    // Write the sample and move forward
    this->write(sample);
    this->forward();
    
    return out;
}
/* 
 * Description: Deconstructor
 */
MultiTapDelay::~MultiTapDelay() {
    free(m_readers);
    free(m_amps);
}