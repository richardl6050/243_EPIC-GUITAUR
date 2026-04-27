// https://www.youtube.com/watch?v=XLyrEKX4Q2g&t=80s

#include "effects.h"

#define SAMPLE_RATE 48000
#define MAX_STRENGTH 10

// Comb filter delay lengths (in samples) chosen to be mutually prime to avoid
// resonance buildup
#define COMB1_LEN 6618   // ~138ms
#define COMB2_LEN 7962   // ~166ms
#define COMB3_LEN 9354   // ~195ms
#define COMB4_LEN 10194  // ~212ms

// Allpass delay lengths
#define AP1_LEN 2082  // ~43ms
#define AP2_LEN 678   // ~14ms

// comb filter buffers
static int comb1_L[COMB1_LEN];
static int comb2_L[COMB2_LEN];
static int comb3_L[COMB3_LEN];
static int comb4_L[COMB4_LEN];

// Allpass buffers
static int ap1_L[AP1_LEN];
static int ap2_L[AP2_LEN];

// Heads for each buffer
static int c1 = 0, c2 = 0, c3 = 0, c4 = 0, a1 = 0, a2 = 0;

// Single comb filter step
// output = delayed + (feedback * input); store new input+feedback in buffer
// ex: comb(comb1_L, &c1, COMB1_LEN, *L, feedback_shift)
static int comb(int* buf, int* head, int len, int input, int feedback_shift) {
  int delayed = buf[*head];
  buf[*head] = input + (delayed >> feedback_shift);
  (*head)++;
  if (*head >= len) *head = 0;
  return delayed;
}

// ex: wet_L = allpass(ap1_L, &a1, AP1_LEN, wet_L);
static int allpass(int* buf, int* head, int len, int input) {
  int delayed = buf[*head];
  buf[*head] = input + (delayed >> 1);
  (*head)++;
  if (*head >= len) *head = 0;
  return delayed - (input >> 1);
}

// reverb: strength controls feedback_shift: how much each comb feeds back (more
// = longer tail) and wet/dry mix: how much reverb is blended into the output

void reverb(int* L, int* R, int strength) {
  if (strength <= 0) return;

  // feedback_shift: strength 1 -> shift 3 (divide sound by 8), strength 10 ->
  // shift 1 (divide sound by 2)
  int feedback_shift = 4 - (strength / 3);  // ranges ~3 down to 1
  if (feedback_shift < 1) feedback_shift = 1;

  // Run input through 4 parallel comb filters and sum them
  int wet_L = comb(comb1_L, &c1, COMB1_LEN, *L, feedback_shift) +
              comb(comb2_L, &c2, COMB2_LEN, *L, feedback_shift) +
              comb(comb3_L, &c3, COMB3_LEN, *L, feedback_shift) +
              comb(comb4_L, &c4, COMB4_LEN, *L, feedback_shift);

 
  // int wet_R = comb(comb1_R, &c1, COMB1_LEN, *R, feedback_shift) +
  //             comb(comb2_R, &c2, COMB2_LEN, *R, feedback_shift) +
  //             comb(comb3_R, &c3, COMB3_LEN, *R, feedback_shift) +
  //             comb(comb4_R, &c4, COMB4_LEN, *R, feedback_shift);



  // Run through 2 allpass filters in series to diffuse the sound
  wet_L = allpass(ap1_L, &a1, AP1_LEN, wet_L);
  wet_L = allpass(ap2_L, &a2, AP2_LEN, wet_L);

  // wet_R = allpass(ap1_R, &a1, AP1_LEN, wet_R);
  // wet_R = allpass(ap2_R, &a2, AP2_LEN, wet_R);

  // Blend wet and dry based on strength
  // strength 1: mostly dry, strength 10: equal mix
  int wet_mix = strength-2;                 // 1–10
  int dry_mix = MAX_STRENGTH - strength+2;  // 9–0

  *L = (*L * dry_mix + wet_L * wet_mix) / MAX_STRENGTH;
  *R = *L;
}