#include "effects.h"
#include <math.h>

static const int SAMPLE_RATE = 48000;
static const float MIN_FREQ = 440.0f;
static const float MAX_FREQ = 2500.0f;
static const int RECALC_PERIOD = 64; //samples

/*
Architecture: 
Design a bandpass filter with resonant peaks at sweeping center frequencies
* Use feedback and and feedforward for poles and zeroes
* center frequency should sweep from x to y
*/

static float coefficients[5] = {0}; //will contain b0, b1, b2, a1, a2
// coefficients[0] = b0
// coefficients[1] = b1 (always 0 for bandpass)
// coefficients[2] = b2
// coefficients[3] = a1
// coefficients[4] = a2

void biquadBandpass(float resonance, float centerFreq){
    //coefficients 
    //resonance = Q (how close to the radius of the unit circle)


    const float w0 = 2 * 3.14159f * centerFreq / SAMPLE_RATE; //place on the unit circle
    const float jw0 = sinf(w0);
    const float re_w0 = cosf(w0);
    const float alpha = jw0/(2*resonance); // RBJ, this is cool because it adjusts how large the bandwidth should be depending on the frequency due to its logarithmic nature
    const float a0 = 1 + alpha; //coefficient for y[n]

    coefficients[0] = alpha/a0;
    coefficients[1] = 0;
    coefficients[2] = -alpha/a0;
    coefficients[3] = (-2 * re_w0)/a0;
    coefficients[4] = (1-alpha)/a0;

}

//buffers and delay lines to apply stuff to
static float state[4] = {0};
static float envelope = 0.0f;

static const float decay[10] = {
    0.9990f, 0.9980f, 0.997f, 0.995f, 0.993f, 0.991f, 0.986f, 0.984f, 0.982f, 0.98f
};

static const float attack[10] = {
    0.9990f, 0.9980f, 0.997f, 0.995f, 0.990f, 0.980f, 0.96f, 0.94f, 0.90f, 0.88f
};

//limit this to 10 dont want peaks too close
static const float resonance[10] = {
    2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f
};

static int nSamples = 0;

void wah(int *L, int *R, int effectStrength){
    nSamples++;

    float normalized = *L/ 2147483648.0f; //this will map it from -1 to 1

    //negatives dont matter for peaks

    float rectified = fabsf(normalized);

    //extract the loudest
    envelope = (rectified > envelope) ? envelope + (1.0f - attack[effectStrength]) * (rectified - envelope)  : envelope * decay[effectStrength];
    // state[0] = x[n-1]
    // state[1] = x[n-2]
    // state[2] = y[n-1]
    // state[3] = y[n-2]

    float centerFreq = MIN_FREQ + (MAX_FREQ-MIN_FREQ) * envelope; 

    if(nSamples >= RECALC_PERIOD){
        nSamples = 0;
        biquadBandpass(resonance[effectStrength], centerFreq);
    }

    float output = coefficients[0] * normalized
        + coefficients[1] * state[0] //feedforward 1 sample back
        + coefficients[2] * state[1] //feedforward 2 samples back
        - coefficients[3] * state[2] //feedback 1 sample back
        - coefficients[4] * state[3]; //feedback 2 samples back

    state[1] = state[0];
    state[0] = normalized;
    state[3] = state[2];
    state[2] = output;

    *R = (int)(output*2147483648.0f);
    *L = (int)(output*2147483648.0f);
}




