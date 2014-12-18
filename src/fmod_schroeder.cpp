//
//  A plugin for FMOD that is a Schroeder Reverb
//  fmod_schroeder
//
//  Created by Alex Graham on 12/14/14.
//  Copyright (c) Alex Graham. All rights reserved.
//
#define SAMPLE_RATE         44100

#ifdef WIN32
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "AllpassFilter.h"
#include "CombFilter.h"
#include "MultiTapDelay.h"
#include "fmod.hpp"

#ifdef WIN32
    #define _CRT_SECURE_NO_WARNINGS
#endif


// FMOD Boilerplate
extern "C" {
    F_DECLSPEC F_DLLEXPORT FMOD_DSP_DESCRIPTION* F_STDCALL FMODGetDSPDescription();
}

#define NUM_COMBS 4
#define NUM_ALLPASS 2

enum
{
    AG_SCHROEDER_PARAM_WET = 0,
    AG_SCHROEDER_PARAM_REVERB_TIME,
    AG_SCHROEDER_PARAM_LPF_FREQ,
    AG_SCHROEDER_PARAM_EARLY_TIME,
    AG_SCHROEDER_PARAM_LATE_TIME,
    AG_SCHROEDER_PARAM_EARLY_AMP,
    AG_SCHROEDER_PARAM_LATE_AMP,
    AG_SCHROEDER_NUM_PARAMETERS
};

// Define Callbacks (FMOD Boilerplate)
FMOD_RESULT F_CALLBACK AG_Schroeder_dspcreate       (FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK AG_Schroeder_dsprelease      (FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspreset        (FMOD_DSP_STATE *dsp_state);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspread         (FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspsetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float value);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspsetparamint  (FMOD_DSP_STATE *dsp_state, int index, int value);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspsetparambool (FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL value);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspsetparamdata (FMOD_DSP_STATE *dsp_state, int index, void *data, unsigned int length);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspgetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspgetparamint  (FMOD_DSP_STATE *dsp_state, int index, int *value, char *valuestr);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspgetparambool (FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL *value, char *valuestr);
FMOD_RESULT F_CALLBACK AG_Schroeder_dspgetparamdata (FMOD_DSP_STATE *dsp_state, int index, void **value, unsigned int *length, char *valuestr);
FMOD_RESULT F_CALLBACK AG_Schroeder_shouldiprocess  (FMOD_DSP_STATE *dsp_state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode);

// FMOD Parameter descriptions (FMOD Boilerplate)
static FMOD_DSP_PARAMETER_DESC p_wet;
static FMOD_DSP_PARAMETER_DESC p_reverb_time;
static FMOD_DSP_PARAMETER_DESC p_lpf_freq;
static FMOD_DSP_PARAMETER_DESC p_early_time;
static FMOD_DSP_PARAMETER_DESC p_late_time;
static FMOD_DSP_PARAMETER_DESC p_early_amp;
static FMOD_DSP_PARAMETER_DESC p_late_amp;

// FMOD Parameter descriptions (FMOD Boilerplate)
FMOD_DSP_PARAMETER_DESC *AG_Schroeder_dspparam[AG_SCHROEDER_NUM_PARAMETERS] =
{
    &p_wet,
    &p_reverb_time,
    &p_lpf_freq,
    &p_early_time,
    &p_late_time,
    &p_early_amp,
    &p_late_amp,
};

// Plugin description (FMOD Boilerplate)
FMOD_DSP_DESCRIPTION AG_Schroeder_Desc =
{
    FMOD_PLUGIN_SDK_VERSION,
    "AG Super SchroederVerb",    // name
    0x00010000,     // plug-in version
    1,              // number of input buffers to process
    1,              // number of output buffers to process
    AG_Schroeder_dspcreate,
    AG_Schroeder_dsprelease,
    AG_Schroeder_dspreset,
    AG_Schroeder_dspread,
    0,
    0,
    AG_SCHROEDER_NUM_PARAMETERS,
    AG_Schroeder_dspparam,
    AG_Schroeder_dspsetparamfloat,
    0, // AG_Schroeder_dspsetparamint,
    0, //AG_Schroeder_dspsetparambool,
    0, // AG_Schroeder_dspsetparamdata,
    AG_Schroeder_dspgetparamfloat,
    0, //AG_Schroeder_dspgetparamint,
    0, //AG_Schroeder_dspgetparambool,
    0, // AG_Schroeder_dspgetparamdata,
    AG_Schroeder_shouldiprocess,
    0
};


extern "C"
{
// Descriptions of parameters (FMOD Boilerplate)
F_DECLSPEC F_DLLEXPORT FMOD_DSP_DESCRIPTION* F_STDCALL FMODGetDSPDescription()
{
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_wet, "Dry/Wet", "%", "Sets Dry/Wet Percentage", 0, 100, 50);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_reverb_time,"Reverb Time", "s", "Length of reverberation", 0.0, 10.0, 1.0);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_lpf_freq, "LPF Frequency", "hz", "Frequency of lowpass filter cutoff", 200.0, 22000.0, 22000.0);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_early_time, "Early Time", "ms", "Delay time for early echo", 0.0, 200.0, 20.0);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_late_time, "Late Time", "ms","Delay time for late echo", 0.0, 200.0, 80.0);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_early_amp, "Early Amp", "f", "Amplitude modifier for early echo", 0.0, 1.0, 0.8);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(p_late_amp, "Late Amp", "f","Amplitude modifier for late echo", 0.0, 1.0, 0.8);
    return &AG_Schroeder_Desc;
}

}

// Define Plugin Class
class AGSchroederState
{
    
private:
    float m_dry_wet;
    float m_reverb_time;
    float m_lpf_freq;
    float m_early_time;
    float m_late_time;
    float m_early_amp;
    float m_late_amp;
    CombFilter *m_combs[NUM_COMBS];
    AllpassFilter *m_allpass[NUM_ALLPASS];
    MultiTapDelay *m_echo;
    
public:
    AGSchroederState();

    void process(float *inbuffer, float *outbuffer, unsigned int length, int channels);
    void reset();
    void setDryWet(float);
    void setReverbTime(float);
    void setLPFFreq(float);
    void setEarlyTime(float);
    void setLateTime(float);
    void setEarlyAmp(float);
    void setLateAmp(float);
    void releaseMemory();
    
    float dryWet() { return m_dry_wet; };
    float reverbTime() { return m_reverb_time; };
    float lpfFreq() { return m_lpf_freq; };
    float earlyTime() { return m_early_time; };
    float lateTime() { return m_late_time; };
    float earlyAmp() { return m_early_amp; };
    float lateAmp() { return m_late_amp; };
};

/*
 * Description: Constructor, set up various parameters
 */
AGSchroederState::AGSchroederState()
{
    // These will likely get overwritten, but we might need them to initialize the
    m_dry_wet = 0.5;
    m_reverb_time = 1.0;
    m_lpf_freq = 20000.0;
    m_early_time = 20.0;
    m_late_time = 80.0;
    m_early_amp = 0.6;
    m_late_amp = 0.8;
    
    // As defined by Schroeder for a concert hall sound
    float comb_lengths[NUM_COMBS] = {0.0297, 0.0371, 0.0411, 0.0437};
    float allpass_lengths[NUM_ALLPASS] = {0.09683, 0.03292};
    float allpass_rvt[NUM_ALLPASS] = {0.005, 0.0017};
    
    // Initialize the Early and Late delays
    float initial_echoes[2] = {m_early_time / 1000, m_late_time / 1000};
    float initial_echo_amps[2] = {m_early_amp, m_late_amp};
    m_echo = new MultiTapDelay(2, initial_echoes, initial_echo_amps, 5, SAMPLE_RATE);
    
    // Initialize the comb filters and allpass filters
    for (int i=0; i < NUM_COMBS; i++) {
        m_combs[i] = new CombFilter(comb_lengths[i], m_reverb_time, SAMPLE_RATE, m_lpf_freq);
    }
    
    for (int i=0; i < NUM_ALLPASS; i++) {
        m_allpass[i] = new AllpassFilter(allpass_lengths[i], allpass_rvt[i], SAMPLE_RATE);
    }
    
    reset();
}

void AGSchroederState::process(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
    float sample, filtered, runningSample;
    int i, j;
    
    // Temporary buffer to hold the filter's processing
    float tempbuff[length];
    
    if (m_combs[0]) { // Make sure we instantiated the objects before every trying any of this
        
        /* Comb filters run in parallel, so apply each sample to the comb filters, and add the processed
         * sample from each comb filter to the corresponding place in a temporary buffer which we'll
         * add to the dry and echo signal later 
         */
        for ( int i = 0; i < length; i++ ) {
            sample = inbuffer[i]; // Get the current input sample
            
            runningSample = 0;
            // Process all of the comb filters, adding the filtered sample to the running sample
            for (j = 0; j < NUM_COMBS; j++) {
                filtered = m_combs[j]->process(sample);
                runningSample += filtered;
            }
            // Copy the filtered sample into the temp buffer
            tempbuff[i] = runningSample;
        }
        
        /* Allpass filters are subsequent, so apply each to the entire buffer
         * that the comb filters created
         */
        for (j = 0; j < NUM_ALLPASS; j++) {
            // Go through each sample in each allpass filter
            for (i = 0; i < length; i++) {
                sample = tempbuff[i]; // Get the input sample from the comb filtered buffer
                filtered = m_allpass[j]->process(sample); // Run the sample through the allpass
                tempbuff[i] = filtered; // Copy back into the temp buffer
            }
        }
        
        float echoSamp = 0;
        for (int i = 0; i < length; i++) {
            
            // Grab the current sample from the multitap delay object
            echoSamp = m_echo->process(inbuffer[i]);
            
            // Combine everything, scaling by the dry wet percentages, and compensating for extra gain
            outbuffer[i] = ((echoSamp + tempbuff[i]) * m_dry_wet + inbuffer[i] * (1 - m_dry_wet)) * 0.5;
        }
        

    } else {
        // If the objects aren't ready, just send the input straight through
        for (int i = 0; i < length; i++) {
            outbuffer[i] = inbuffer[i];
        }
    }


}

void AGSchroederState::reset()
{
    // Make sure all the objects have been deleted before recreating them
    for (int i = 0; i < NUM_COMBS; i++) {
        if (m_combs[i]) {
            delete m_combs[i];
        }
    }
    
    for (int i = 0; i < NUM_ALLPASS; i++) {
        if (m_allpass[i]) {
            delete m_allpass[i];
        }
    }
    
    delete m_echo;
    
    /* For some reason, FMOD is freeing some of the memory before calling reset,
     * so every time we reset we recreate the objects. This isn't ideal, but 
     * it seems to be necessary.
     */
    
    // Initial times for filters based on Schroeder's findings
    float comb_lengths[NUM_COMBS] = {0.0297, 0.0371, 0.0411, 0.0437};
    float allpass_lengths[NUM_ALLPASS] = {0.09683, 0.03292};
    float allpass_rvt[NUM_ALLPASS] = {0.005, 0.0017};
    
    // Initial echo arrays
    float initial_echoes[2] = {m_early_time / 1000, m_late_time / 1000};
    float initial_echo_amps[2] = {m_early_amp, m_late_amp};
    m_echo = new MultiTapDelay(2, initial_echoes, initial_echo_amps, 5, SAMPLE_RATE);
    
    // Instantiate comb filters
    for (int i=0; i < NUM_COMBS; i++) {
        m_combs[i] = new CombFilter(comb_lengths[i], m_reverb_time, SAMPLE_RATE, m_lpf_freq);
    }
    
    // Instantiate allpass filters
    for (int i=0; i < NUM_ALLPASS; i++) {
        m_allpass[i] = new AllpassFilter(allpass_lengths[i], allpass_rvt[i], SAMPLE_RATE);
    }
}

void AGSchroederState::releaseMemory() {
    // Go through and delete all combs
    for (int i = 0; i < NUM_COMBS; i++) {
        if (m_combs[i]) {
            delete m_combs[i];
        }
    }
    // Delete all allpasses
    for (int i = 0; i < NUM_ALLPASS; i++) {
        if (m_allpass[i]) {
            delete m_allpass[i];
        }
    }
    if (m_echo) {
        delete m_echo;
    }
}

/* Note: for following setters, since these get called from callbacks,
 * we need to check to make sure the filter/delay objects have been 
 * instantiated yet in the constructor before trying to set them
 */

void AGSchroederState::setDryWet(float dry_wet) {
    // Convert dry/wet to 0-1 float from percentage
    m_dry_wet = dry_wet / 100.0;
}

void AGSchroederState::setReverbTime(float reverb_time)
{
    m_reverb_time = reverb_time;
    
    // Set the reverb time for all the comb filters (allpass stay the same)
    for (int i=0; i < NUM_COMBS; i++) {
        if (m_combs[i]) {
            m_combs[i]->setReverbTime(m_reverb_time);
        }
    }
}

void AGSchroederState::setLPFFreq(float lpf_freq) {
    m_lpf_freq = lpf_freq;
    
    // Set the LPF for all the comb filters
    for (int i=0; i < NUM_COMBS; i++) {
        if (m_combs[i]) {
            m_combs[i]->setLPFFrequency(lpf_freq);
        }
    }
}

void AGSchroederState::setEarlyTime(float early_time) {
    m_early_time = early_time;
    if (m_echo) {
        // Set delay of reader in slot 0, divide by 1000 because function takes seconds
        m_echo->setDelayTime(0, early_time / 1000.0);
    }
}

void AGSchroederState::setLateTime(float late_time) {
    m_late_time = late_time;
    if (m_echo) {
        // Set delay of reader in slot 1, divide by 1000 because function takes seconds
        m_echo->setDelayTime(1, late_time / 1000.0);
    }
}

void AGSchroederState::setEarlyAmp(float early_amp) {
    m_early_amp = early_amp;
    if (m_echo) {
        // Set amplitude of reader in slot 0
        m_echo->setAmplitude(0, early_amp);
    }
}

void AGSchroederState::setLateAmp(float late_amp) {
    m_late_amp = late_amp;
    if (m_echo) {
        // Set amplitude of reader in slot 1
        m_echo->setAmplitude(1, late_amp);
    }
}

// Below is all boilerplate for FMOD plugin
// ---------------------------------------------

FMOD_RESULT F_CALLBACK AG_Schroeder_dspcreate(FMOD_DSP_STATE *dsp_state)
{
    dsp_state->plugindata = (AGSchroederState *)FMOD_DSP_STATE_MEMALLOC(dsp_state, sizeof(AGSchroederState), FMOD_MEMORY_NORMAL, "AGSchroederState");
    if (!dsp_state->plugindata)
    {
        return FMOD_ERR_MEMORY;
    }
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AG_Schroeder_dsprelease(FMOD_DSP_STATE *dsp_state)
{
    AGSchroederState *state = (AGSchroederState *)dsp_state->plugindata;
    //state->releaseMemory();
    FMOD_DSP_STATE_MEMFREE(dsp_state, state, FMOD_MEMORY_NORMAL, "AGSchroederState");
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AG_Schroeder_dspread(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int * /*outchannels*/)
{
    AGSchroederState *state = (AGSchroederState *)dsp_state->plugindata;
    state->process(inbuffer, outbuffer, length, inchannels); // input and output channels count match for this effect
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AG_Schroeder_dspreset(FMOD_DSP_STATE *dsp_state)
{
    AGSchroederState *state = (AGSchroederState *)dsp_state->plugindata;
    state->reset();
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AG_Schroeder_dspsetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float value)
{
    AGSchroederState *state = (AGSchroederState *)dsp_state->plugindata;

    switch (index)
    {
        case AG_SCHROEDER_PARAM_WET:
            state->setDryWet(value);
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_REVERB_TIME:
            state->setReverbTime(value);
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_LPF_FREQ:
            state->setLPFFreq(value);
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_EARLY_TIME:
            state->setEarlyTime(value);
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_LATE_TIME:
            state->setLateTime(value);
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_EARLY_AMP:
            state->setEarlyAmp(value);
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_LATE_AMP:
            state->setLateAmp(value);
            return FMOD_OK;
    }

    return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT F_CALLBACK AG_Schroeder_dspgetparamfloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr)
{
    AGSchroederState *state = (AGSchroederState *)dsp_state->plugindata;

    switch (index)
    {
        case AG_SCHROEDER_PARAM_WET:
            *value = state->dryWet();
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_REVERB_TIME:
            *value = state->reverbTime();
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_LPF_FREQ:
            *value = state->lpfFreq();
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_EARLY_TIME:
            *value = state->earlyTime();
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_LATE_TIME:
            *value = state->lateTime();
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_EARLY_AMP:
            *value = state->earlyAmp();
            return FMOD_OK;
        case AG_SCHROEDER_PARAM_LATE_AMP:
            *value = state->lateAmp();
            return FMOD_OK;
    }

    return FMOD_ERR_INVALID_PARAM;
}


FMOD_RESULT F_CALLBACK AG_Schroeder_shouldiprocess(FMOD_DSP_STATE * /*dsp_state*/, FMOD_BOOL inputsidle, unsigned int /*length*/, FMOD_CHANNELMASK /*inmask*/, int /*inchannels*/, FMOD_SPEAKERMODE /*speakermode*/)
{
    if (inputsidle)
    {
        return FMOD_OK;
    }

    return FMOD_OK;
}
