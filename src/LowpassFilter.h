//
//  LowpassFilter.h
//  Schroeder Reverb
//
//  Created by Alex Graham on 12/16/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: A simple IIR Lowpass filter
 */

#ifndef __fmod_schroeder__LowpassFilter__
#define __fmod_schroeder__LowpassFilter__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_COEFF 5

class LowpassFilter {
private:
    float m_last_out[2];
    float m_last_in[2];
    float m_a_coeff[3];
    float m_b_coeff[3];
public:
    LowpassFilter(double freq, double srate);
    void changeFrequency(double freq, double srate);
    float process(float sample);
};

#endif /* defined(__PS7__LowpassFilter__) */
