#include "effects.h"
#include <math.h>

static const int SAMPLE_RATE = 48000;

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

void biquadBandpass(int resonance, int centerFreq){
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

int applyBiquad(int sample){
    float normalized = sample/ 2147483648.0f; //this will map it from -1 to 1

    // state[0] = x[n-1]
    // state[1] = x[n-2]
    // state[2] = y[n-1]
    // state[3] = y[n-2]

    float output = coefficients[0] * normalized
        + coefficients[1] * state[0] //feedforward 1 sample back
        + coefficients[2] * state[1] //feedforward 2 samples back
        - coefficients[3] * state[2] //feedback 1 sample back
        - coefficients[4] * state[3]; //feedback 2 samples back

    state[1] = state[0];
    state[0] = normalized;
    state[3] = state[2];
    state[2] = output;

    return (int)(output*2147483648.0f);
}

