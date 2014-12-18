//
//  LowpassFilter.cpp
//  PS7
//
//  Created by Alex Graham on 12/16/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: A simple IIR Lowpass filter
 */

#include "LowpassFilter.h"

/*
 * Description: Constructor
 */
LowpassFilter::LowpassFilter(double freq, double srate) {
    
    // Set the frequency and calculate the coefficients
    this->changeFrequency(freq, srate);
    
    // Initialize the feedback and feedforward buffers
    for (int i = 0; i < 2; i++) {
        m_last_out[i] = 0;
        m_last_in[i] = 0;
    }
}

/*
 * Description: Calculates the coefficients for a simple lowpass filter
 */
void LowpassFilter::changeFrequency(double freq, double srate) {
    
    double w0 = 2.0 * M_PI * freq / srate;
    double alpha = sin(w0) / (2.0 * 0.8);
    
    m_b_coeff[0] = (1.0 - cos(w0)) / 2.0;
    m_b_coeff[1] = 1.0 - cos(w0);
    m_b_coeff[2] = (1.0 - cos(w0)) / 2.0;
    
    m_a_coeff[0] = 1.0 + alpha;
    m_a_coeff[1] = -2.0 * cos(w0);
    m_a_coeff[2] = 1.0 - alpha;
    
    // Predivide the coefficients by a0
    m_b_coeff[0] = m_b_coeff[0] / m_a_coeff[0];
    m_b_coeff[1] = m_b_coeff[1] / m_a_coeff[0];
    m_b_coeff[2] = m_b_coeff[2] / m_a_coeff[0];
    m_a_coeff[1] = m_a_coeff[1] / m_a_coeff[0];
    m_a_coeff[2] = m_a_coeff[2] / m_a_coeff[0];
}

/* 
 * Description: Process and sample and return the filtered sample
 */
float LowpassFilter::process(float in) {
    
    // The difference equation
    float out = (m_b_coeff[0] * in + m_b_coeff[1] * m_last_in[0] + m_b_coeff[2] * m_last_in[1]);
    out = out - m_a_coeff[1] * m_last_out[0] - m_a_coeff[2] * m_last_out[1];
    
    // Shift in the next sample to the feedback and kick out the next sample
    m_last_out[1] = m_last_out[0];
    m_last_out[0] = out;
    
    // Shift in the feedforward sample
    m_last_in[1] = m_last_in[0];
    m_last_in[0] = in;
    
    return out;
}