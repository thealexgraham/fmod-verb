//
//  CombFilter.cpp
//  PS7
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: A simple comb filter
 */

#include "CombFilter.h"

/*
 * Description: Constructor, takes in loop time and reverb time in seconds, frequency in hz
 */
CombFilter::CombFilter(float loop_time, float reverb_time, float srate, double lpf_frequency) {
    
    m_loop_time = loop_time;
    m_reverb_time = reverb_time;
    
    m_srate = srate;
    
    // Max delay is the loop time since this stays static
    m_delay = new DelayLine(m_loop_time, m_loop_time, srate);
    
    // Calculate the feedback amplitude
    m_fb_amplitude = pow(0.001, m_loop_time / m_reverb_time); // a = 0.001^loopTime/reverbTime
    
    // Create the lowpass filter class with the first LPF freq
    m_lpf = new LowpassFilter(lpf_frequency, srate);
    
}

/* 
 * Description: Processes the current sample and returns the procssed sample
 */
float CombFilter::process(float in) {
    // Get the current output of the delay to return
    float out = m_delay->read();
    
    // Lowpass the output. Multiply by 0.95 since the lowpass is amplifying the sample slightly
    float lowpassed = m_lpf->process(out) * 0.95;

    //Write the current input + lowpassed/scaled delay feedback
    m_delay->write(in + lowpassed * m_fb_amplitude);
    
    // Move the delay forward
    m_delay->forward();

    return out;
}

/*
 * Description: Set Reverb time and adjust feedback amplitude
 */
void CombFilter::setReverbTime(float reverb_time) {
    m_reverb_time = reverb_time;
    m_fb_amplitude = pow(0.001, m_loop_time / m_reverb_time);
}

/*
 * Description: Change low pass filter frequency
 */
void CombFilter::setLPFFrequency(double lpf_frequency) {
    m_lpf->changeFrequency(lpf_frequency, m_srate);
}

/*
 * Description: Deconstructor, free memory
 */
CombFilter::~CombFilter() {
    if (m_delay) {
        delete m_delay;
    }
    
    if (m_lpf) {
        delete m_lpf;
    }
}