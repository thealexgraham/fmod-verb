//
//  MultiTapDelay.h
//  Schroeder Reverb
//
//  Created by Alex Graham on 12/16/14.
//  Copyright (c) 2014 Alex Graham. All rights reserved.
//

/* 
 * Description: This class is a multi tap delay that can create a variable amount of
 * taps with lengths and amplitudes. The default processing does not use feedback
 */

#ifndef __fmod_schroeder__MultiTapDelay__
#define __fmod_schroeder__MultiTapDelay__

#include <stdio.h>
#include <stdlib.h>
class MultiTapDelay {
private:
    int *m_readers;
    int m_writer;
    float *m_amps;
    int m_num_taps;
    float *m_buffer;
    int m_delay_max;
    float m_srate;
    
public:
    MultiTapDelay(int num_taps, float *delay_times, float *amps, float max_delay, int srate);
    MultiTapDelay();
    ~MultiTapDelay();
    float read(int tap);
    void write(float sample);
    void forward();
    float process(float sample); // Read, write, forward
    void setDelayTime(int tap, float time);
    void setAmplitude(int tap, float amp);
};

#endif /* defined(__PS7__MultiTapDelay__) */
