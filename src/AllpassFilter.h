//
//  AllpassFilter.h
//  PS7
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/*
 * Description: A simple Allpass filter
 */

#ifndef __fmod_schroeder__AllpassFilter__
#define __fmod_schroeder__AllpassFilter__

#include <stdio.h>
#include <stdlib.h>
#include "DelayLine.h"
#include <math.h>


class AllpassFilter {
private:
    DelayLine *m_delay;
    float m_fb_amplitude;
    float m_loop_time;
    float m_reverb_time;
public:
    AllpassFilter(float loop_time, float reverb_time, int srate);
    ~AllpassFilter();
    float process(float sample);
    void setReverbTime(float reverbTime);
};

#endif /* defined(__PS7__AllpassFilter__) */
