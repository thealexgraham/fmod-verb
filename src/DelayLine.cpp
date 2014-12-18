//
//  DelayLine.cpp
//  fmod_schroeder
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) 2014 Firelight Technologies. All rights reserved.
//

/*
 * Description: A very simple single tap delay line
 */

#include "DelayLine.h"

/*
 * Description: Initializes the delay line
 */
DelayLine::DelayLine(float delay_time, float max_delay_time, int samplerate) {
    // Convert the max delay time (s) to samples
    m_delay_max = (float) samplerate * max_delay_time;
    
    // allocate the circular buffer to the max delay length
    m_buffer = (float *)malloc(sizeof(float) * m_delay_max);
    // Initialize the buffer to 0s
    for (int i=0; i < m_delay_max; i++) {
        m_buffer[i] = 0;
    }
    // Writer starts at the beginning
    m_writer = 0;
    // Set the reader to the delay time samples away from the end of the buffer
    // since the buffer wraps and the writer starts at 0
    m_reader = m_delay_max - (samplerate * delay_time);
}

/*
 * Reads the sample at the current read point in the buffer
 */
float DelayLine::read() {
    return m_buffer[m_reader];
}

/*
 * Description: This function reads n samples back from the current reader position
 * Note: This was for an in class LPF that is not used anymore
 */
float DelayLine::readBack(int n) {
    int readAt = m_reader - n;
    if (readAt < 0) {
        // m_delay_max is the buffer size, so wrap from the back
        readAt = m_delay_max + readAt;
    }
    return m_buffer[readAt];
}

/*
 * Description: Write a sample at the current write point for the delay line
 */
void DelayLine::write(float sample) {
    m_buffer[m_writer] = sample;
}

/*
 * Description: Move the delay line forward a sample, this should be done in the
 * parent process function after reading and writing is done.
 */
void DelayLine::forward() {
    // Move readers/writers forward
    m_reader++;
    m_writer++;
    // Wrap around to the beginning if necessary
    m_writer %= m_delay_max;
    m_reader %= m_delay_max;
}

/* 
 * Description: Writes sample, returns current read sample, moves line forward.
 * This can be done if the delay is used as a simple delay with no feedback
 */
float DelayLine::process(float sample) {
    // Save the sample since we'll need to move forward
    float out = this->read();
    this->write(sample);
    this->forward();
    
   return out;
}

/*
 * Destructor
 */
DelayLine::~DelayLine() {
    free(m_buffer);
}