//
//  CombFilter.h
//  PS7
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: A simple comb filter
 */

#ifndef __fmod_schroeder__CombFilter__
#define __fmod_schroeder__CombFilter__

#include <stdio.h>
#include <stdlib.h>
#include "DelayLine.h"
#include <math.h>
#include "LowpassFilter.h"


#define NUM_COEFF 5

class CombFilter {
private:
    DelayLine *m_delay;
    float m_fb_amplitude;
    float m_loop_time;
    float m_reverb_time;
    float m_srate;
    LowpassFilter *m_lpf;
    
public:
    CombFilter(float loop_time, float reverb_time, float srate, double lpf_frequency);
    ~CombFilter();
    float process(float sample);
    void setReverbTime(float reverb_time);
    void setLPFFrequency(double lpf_frequency);
};


#endif /* defined(__PS7__CombFilter__) */

