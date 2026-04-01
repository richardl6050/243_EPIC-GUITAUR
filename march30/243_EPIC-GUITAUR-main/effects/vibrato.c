#include <math.h>

#include "effects.h"

#define SAMPLE_RATE 48000
#define MAX_DEPTH 200
#define CENTER_DELAY 400
#define BUF_LEN (CENTER_DELAY + MAX_DEPTH + 1)

#define PI 3.14159265f  // f suffix = float literal

static int buf_L[BUF_LEN];
static int buf_R[BUF_LEN];
static int write_head = 0;

void vibrato(int* L, int* R, int strength) {
  if (strength <= 0) return;

  float rate_hz = 1.0f + (strength - 1) * (5.0f / 9.0f);
  int depth = (strength * MAX_DEPTH) / 10;

  static float phase = 0.0f;
  phase += (2.0f * PI * rate_hz) / SAMPLE_RATE;
  if (phase >= 2.0f * PI) phase -= 2.0f * PI;

  int offset = (int)(sinf(phase) * depth);  // sinf() = float version of sin()

  int read_head = write_head - CENTER_DELAY - offset;
  if (read_head < 0) read_head += BUF_LEN;

  buf_L[write_head] = *L;
  buf_R[write_head] = *R;

  *L = buf_L[read_head];
  *R = buf_R[read_head];

  write_head++;
  if (write_head >= BUF_LEN) write_head = 0;
}