#include "effects.h"

#define SAMPLE_RATE 8000
#define DELAY_PER_LEVEL \
  1600                   // 8000 samples/sec * 0.2 seconds per strength level
#define MAX_DELAY 16000  // 8000 samples/sec * 2.0 seconds (strength 10)

// Buffer sized for the maximum possible delay
static int buf_left[MAX_DELAY];
static int buf_right[MAX_DELAY];
static int head = 0;

/*
 * strength: 0  → no echo (dry signal only)
 *           1  → 0.2 s delay  (1600 samples)
 *           2  → 0.4 s delay  (3200 samples)
 *           ...
 *           10 → 2.0 s delay  (16000 samples)
 *
 * Echo is mixed at half volume (>> 1) regardless of strength
 */

void echo(int* L, int* R, int strength) {
  if (strength <= 0) {
    return;  // nothing. why did I write this? perhaps for safety?
  }

  int delay_samples = strength * DELAY_PER_LEVEL;  // 1600 to 16000

  // get sample from delay_samples ago
  int delayed_left = buf_left[head];
  int delayed_right = buf_right[head];

  // Overwrite that slot with the current input
  buf_left[head] = *L;
  buf_right[head] = *R;

  // Advance head, wrapping within the current delay window
  head++;
  if (head >= delay_samples) {
    head = 0;
  }

  // Mix echo at half volume
  *L += (delayed_left >> 1);
  *R += (delayed_right >> 1);
}