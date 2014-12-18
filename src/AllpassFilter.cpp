//
//  AllpassFilter.cpp
//  PS7
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: A simple Allpass filter
 */

#include "AllpassFilter.h"

/* 
 * Description: Initializes the object. Times are in seconds, sampling rate is in seconds
 */
AllpassFilter::AllpassFilter(float loop_time, float reverb_time, int srate) {
    m_loop_time = loop_time;
    m_reverb_time = reverb_time;
    
    // Initialize delay line with max buffer to the loop time since this will not be changed
    m_delay = new DelayLine(loop_time, loop_time, srate);
    // Calculate the amplitude based on loop time and reverb time
    m_fb_amplitude = pow(0.001, loop_time / reverb_time); // a = 0.001^loopTime/reverbTime
}

/*
 * Description: process a sample and return the processed sample based on Allpass design
 */
float AllpassFilter::process(float in) {
    // Get the sample from the delay multiplied by feedback amp and added to the current sample
    float first = m_delay->read() * m_fb_amplitude + in;
    // Get the output of the delay added to the previous sample multiplied by negative feedback amp
    float out = m_delay->read() + first * -m_fb_amplitude;
    
    // Write the first sample to the delay line and move forward
    m_delay->write(first);
    m_delay->forward();
    
    // Return the out sample added to the input sample
    return out + in;
}

/*
 * Description: Set Reverb time and adjust feedback amplitude
 */
void AllpassFilter::setReverbTime(float reverbTime) {
    m_reverb_time = reverbTime;
    m_fb_amplitude = pow(0.001, m_loop_time / m_reverb_time);
}

/*
 * Description: Deconstructor
 */
AllpassFilter::~AllpassFilter() {
    delete m_delay;
}