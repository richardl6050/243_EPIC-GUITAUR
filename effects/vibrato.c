// wobbles pitch. playback faster=higher pitch, slower = lower pitch. We
// playback audio delayed, and a sine wave determines the offset from that main
// delay. (DELAY + SIN) When the sine is positive (delay greater), we read audio
// further away, and when sine is negative, we read audio closer to reality.

// strength: 0 -> fast vibration, low amplitude
//           10 -> slow vibration (more noticible wobble), high amplitude,

#include "effects.h"

#define SAMPLE_RATE 48000
#define MAX_DEPTH 200     // max ±200 samples of delay swing (~±4ms at 48kHz)
#define CENTER_DELAY 400  // base delay so we never read before the buffer start
#define BUF_LEN (CENTER_DELAY + MAX_DEPTH + 1)

static int buf_L[BUF_LEN];
static int buf_R[BUF_LEN];
static int write_head = 0;

// Sine approximation using a Bhaskara-style polynomial.
// input: phase 0–65535 (full circle), output: -32767 to +32767
static int fast_sine(unsigned int phase) {
  // Map phase into 0–32767 for the half-period, track sign
  int sign = 1;
  unsigned int p = phase & 0xFFFF;
  if (p >= 32768) {
    p -= 32768;
    sign = -1;
  }

  // Bhaskara I approximation: sin(x) ≈ 16x(π - x) / (5π² - 4x(π - x))
  // Scaled to integer: accurate to ~0.2%
  int x = (int)p;                   // 0 to 32767
  int num = x * (32768 - x);        // x * (pi - x), scaled
  int y = (num >> 12) * 315 / 305;  // scale to 0–32767 range
  return sign * y;
}

/*
 * vibrato
 *
 * strength: 0  → dry signal
 *           1  → subtle wobble (~1 Hz rate, shallow depth)
 *           10 → seasick wobble (~6 Hz rate, full depth)
 *
 * Both rate and depth increase with strength.
 */

void vibrato(int* L, int* R, int strength) {
  if (strength <= 0) return;

  // Rate: strength 1 → ~1 Hz, strength 10 → ~6 Hz
  // phase_inc = rate_hz * 65536 / SAMPLE_RATE
  int rate_hz = strength;  // 1–10 Hz (rough)
  unsigned int phase_inc = (unsigned int)(rate_hz * 65536) / SAMPLE_RATE;

  // Depth: how many samples the delay swings by (0 to MAX_DEPTH)
  int depth = (strength * MAX_DEPTH) / 10;  // 20–200 samples

  // Persistent phase accumulator
  static unsigned int phase = 0;
  phase += phase_inc;  // wraps naturally at 65536

  // sine value in range -32767 to +32767, scaled to ±depth
  int sine = fast_sine(phase);
  int offset = (sine * depth) >> 15;  // scale to ±depth samples

  // Read head sits CENTER_DELAY behind write head, then offset by sine
  int read_head = write_head - CENTER_DELAY - offset;
  if (read_head < 0) read_head += BUF_LEN;

  // Write current samples into buffer
  buf_L[write_head] = *L;
  buf_R[write_head] = *R;

  // Read the delayed (pitch-shifted) sample
  *L = buf_L[read_head];
  *R = buf_R[read_head];

  write_head++;
  if (write_head >= BUF_LEN) write_head = 0;
}