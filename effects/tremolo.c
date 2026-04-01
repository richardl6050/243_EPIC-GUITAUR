// unlike vibrato, wobbles/pulses volume instead of pitch.

// strength 1 -> 1Hz pulse
//          10 -> 1-Hz pulse

// Volume = 1.0 - (DEPTH/2) + (DEPTH/2) * sin(phase) which keeps volume between
// (1 - DEPTH) and 1.0 at all times.

#include <math.h>

#include "effects.h"

#define SAMPLE_RATE 48000
#define DEPTH 0.5f  // swings volume between 50% and 100%
#define PI 3.1415926f

void tremolo(int* L, int* R, int strength) {
  if (strength <= 0) return;
  float rate_hz = strength;  // 1-10 Hz
  float phase = 0.0;
  phase += (2.0 * PI * rate_hz) /
           SAMPLE_RATE;  // 2pi * frequency goes around circle (2pi) 'frequency'
  // times per second. But we are per sample, so divide
  // by SAMPLE_RATE
  if (phase >= (2.0 * PI))
    phase -=
        2.0f *
        PI;  // wrap phase around so it doesn't increase to infinity and beyond

  int volume = 1.0 - (DEPTH / 2) + (DEPTH / 2) * sinf(phase);

  *L = (int)*L * volume;
  *R = (int)*R * volume;
}
