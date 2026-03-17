#define AUDIO_BASE ((volatile int *)0xFF203040)
#define AUDIO_CONTROL (*(AUDIO_BASE + 0))
#define AUDIO_FIFOSPACE (*(AUDIO_BASE + 1))
#define AUDIO_LEFT (*(AUDIO_BASE + 2))
#define AUDIO_RIGHT (*(AUDIO_BASE + 3))

#define LEDS_BASE ((volatile int *)0xFF200000)
#define SW_BASE ((volatile int *)0xFF200040)
#define KEYS_BASE ((volatile int *)0xFF200050)
#define TIMER_BASE ((volatile int *)0xFF202000)

#define NUM_EFFECTS 9

void display_leds(int val);
int keys_pressed();
int count_switches(sw);
int which_sw(int sw);
void leds_show_strength(int val);

static int fx_strength[NUM_EFFECTS] = {0};
int eff_2config = -1;  // which effect is being configured (1 to NUM_EFFECTS)

int main(void) {
  // To set the strength of an effect, only the switch corresoinding to that
  // effect can be up. To configure it, press KEY3. This turns program to
  // configure mode, which displays the current strength of that effect on the
  // LEDS.(ex. 0=all off; 1=LED9 on; 2=LED9 and LED8 on;... 10=all LEDS on)
  // Toggling other switches here will not do anything. Use KEY1 and KEY0 to
  // increment and decrement the strength of that effect from 0-10. The value
  // will be shown on the LEDS . Once desired strength reached, press KEY3 again
  // to confirm.

  // That puts program back in playback mode, where effects can be turned on or
  // off using the switches.

  typedef enum { PLAYBACK, CONFIGURE } State;
  State state = PLAYBACK;

  while (1) {
    int sw = *SW_BASE & 0x3FF;
    int keys = keys_pressed();

    if (state == PLAYBACK) {
      *LEDS_BASE = sw;

      if (keys == 8 &&
          count_switches(sw) == 1) {  // if we want to configure an effect
        eff_2config = which_sw(sw);
        leds_show_strength(fx_strength[eff_2config]);
        state = CONFIGURE;
      }

    } else {                                             // state == CONFIGURE
      if (keys == 2 && fx_strength[eff_2config] < 10) {  // KEY1 Increments
        fx_strength[eff_2config] += 1;
        leds_show_strength(fx_strength[eff_2config]);
      } else if (keys == 1 && fx_strength[eff_2config] > 0) {  // KEY0
                                                               // Decrements
        fx_strength[eff_2config] -= 1;
        leds_show_strength(fx_strength[eff_2config]);
      } else if (keys == 8) {
        eff_2config = -1;
        state = PLAYBACK;
      }
    }
  }
  return 0;
}

void display_leds(int val) {}
int keys_pressed() {
  volatile int *edge = KEYS_BASE + 3;
  int pressed = *edge & 0xF;  // 4 keys
  if (pressed) {
    *edge = pressed;  // reset keys
  }
  return pressed;
}
int count_switches(sw) {
  int n = 0;
  for (int i = 0; i < NUM_EFFECTS; i++) {
    if (sw & (1 << i)) {
      n++;
    }
  }
  return n;
}
int which_sw(int sw) {  // finds the singular switch on. used to specify which
                        // effect strength to change in configure mode
  for (int i = NUM_EFFECTS; i > 0; i--) {
    if ((sw >> (i - 1)) & 1)
      return i - 1;  // also return 0-indexed to match fx_strength[]
                     // if ((sw & (1 << (i - 1))) == 1) {
    //  return i;
  }
}

void leds_show_strength(int val) {
  /* val == 0  → 0b0000000000 (all off)
   * val == 1  → 0b1000000000 (LED9 on)
   * val == 10 → 0b1111111111 (all on)      */
  int mask = 0;
  for (int i = 0; i < val; i++) mask |= (1 << (9 - i));
  *LEDS_BASE = mask;
}
