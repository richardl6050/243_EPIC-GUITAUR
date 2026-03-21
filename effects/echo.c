#include "effects.h"

#define SAMPLE_RATE 8000
#define DELAY_SAMPLES 3200  // 8000 samples/sec * 0.4 seconds
#define MAX_STRENGTH 10

// Persistent state — zeroed automatically at startup
static int buf_left[DELAY_SAMPLES];
static int buf_right[DELAY_SAMPLES];
static int head = 0;

/*
 * echo — applies a 0.4-second circular-buffer echo to *L and *R.
 *
 * strength: 0  → no echo (dry signal only)
 *           1  → quietest echo  (~10% of delayed sample)
 *           10 → loudest echo   (100% of delayed sample, i.e. >> 0)
 *
 * The delayed sample is right-shifted by (MAX_STRENGTH - strength),
 * so strength 10 adds the full delayed amplitude and strength 0 adds none.
 */
void echo(int* L, int* R, int strength) {
  // Retrieve the sample stored 0.4 seconds ago
  int delayed_left = buf_left[head];
  int delayed_right = buf_right[head];

  // Overwrite that slot with the current input sample
  buf_left[head] = *L;
  buf_right[head] = *R;

  // Advance head, wrapping around the circular buffer
  head++;
  if (head >= DELAY_SAMPLES) {
    head = 0;
  }

  // Mix: scale the echo by strength (0 = silent, 10 = full volume)
  // Guard against strength == 0 to avoid undefined behaviour on >> 10
  if (strength > 0) {
    int shift = MAX_STRENGTH -
                strength;  // strength 10 → shift 0 (full), strength 1 → shift 9
    *L += (delayed_left >> shift);
    *R += (delayed_right >> shift);
  }
  // strength == 0: output is unchanged (no echo added)
}