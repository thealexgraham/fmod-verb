//
//  DelayLine.h
//  fmod_schroeder
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) Alex Graham. All rights reserved.
//

/*
 * Description: A very simple single tap delay line
 */

#ifndef __fmod_schroeder__DelayLine__
#define __fmod_schroeder__DelayLine__

#include <stdlib.h>
#include <stdio.h>

class DelayLine {
private:
    int m_reader;
    int m_writer;
    float *m_buffer;
    int m_delay_max;
public:
    DelayLine(float delay_time, float max_delay_time, int samplerate);
    ~DelayLine();
    float read();
    float readBack(int n);
    void write(float sample);
    void forward();
    float process(float sample); // Read, write, forward
};

#endif /* defined(__fmod_schroeder__DelayLine__) */
