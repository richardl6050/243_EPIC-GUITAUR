// wobbles pitch. playback faster=higher pitch, slower = lower pitch. We
// playback audio delayed, and a sine wave determines the offset from that main
// delay. (DELAY + SIN) When the sine is positive (delay greater), we read audio
// further away, and when sine is negative, we read audio closer to reality.

// strength: 0 -> fast vibration, low amplitude
//           10 -> slow vibration (more noticible wobble), high amplitude,

#include "effects.h"
#include <math.h>

#define SAMPLE_RATE   48000
#define MAX_DEPTH     200       // max ±200 samples of delay swing (~±4ms at 48kHz)
#define CENTER_DELAY  400       // base delay so read head never goes past write head
#define BUF_LEN       (CENTER_DELAY + MAX_DEPTH + 1)

#define PI 3.14159265358979

static int buf_L[BUF_LEN];
static int buf_R[BUF_LEN];
static int write_head = 0;

/*
 * vibrato
 *
 * strength: 0  → dry signal
 *           1  → subtle wobble (~1 Hz, shallow depth)
 *           10 → heavy wobble  (~6 Hz, full depth)
 */
void vibrato(int* L, int* R, int strength) {
    if (strength <= 0) return;

    // Rate: strength 1 → 1 Hz, strength 10 → 6 Hz
    double rate_hz = 1.0 + (strength - 1) * (5.0 / 9.0);

    // Depth: strength 1 → 20 samples, strength 10 → 200 samples
    int depth = (strength * MAX_DEPTH) / 10;

    // Phase accumulator — tracks where we are in the sine cycle
    static double phase = 0.0;
    phase += (2.0 * PI * rate_hz) / SAMPLE_RATE;
    if (phase >= 2.0 * PI) phase -= 2.0 * PI;   // keep phase in 0 to 2π

    // sine output scaled to ±depth samples
    int offset = (int)(sin(phase) * depth);

    // Read head sits CENTER_DELAY behind write, shifted by sine offset
    int read_head = write_head - CENTER_DELAY - offset;
    if (read_head < 0) read_head += BUF_LEN;

    // Write current sample into buffer
    buf_L[write_head] = *L;
    buf_R[write_head] = *R;

    // Output the delayed sample
    *L = buf_L[read_head];
    *R = buf_R[read_head];

    write_head++;
    if (write_head >= BUF_LEN) write_head = 0;
}
