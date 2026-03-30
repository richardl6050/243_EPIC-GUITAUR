#include "address_map.h"
#include "effects/effects.h"
#include "setup/init.h"

#define NUM_EFFECTS 6

// function declarations

void display_leds(int val);
int keys_pressed();
int count_switches(int sw);
int which_sw(int sw);
void leds_show_strength(int val);

static int fx_strength[NUM_EFFECTS] = {0};  // defines thes trength of each
int eff_2config = -1;  // which effect is being configured (1 to NUM_EFFECTS)

struct audio_base {
  volatile unsigned int control;
  volatile unsigned char rarc;
  volatile unsigned char ralc;
  volatile unsigned char wsrc;
  volatile unsigned char wslc;
  volatile int ldata;
  volatile int rdata;
};

// array of functions for effects

// address usage
volatile int* KEYS = (int*)KEY_BASE;
volatile int* SW = (int*)SW_BASE;
volatile int* LEDS = (int*)LED_BASE;

static int* LEFT;
static int* RIGHT;

static int LOOP_BUFFER[4194304] = {0};
static int LOOP_WRITER, LOOP_READER, LOOP_END;

int main(void) {
  // audio set up
  init();

  volatile struct audio_base* AUDIO = (struct audio_base*)AUDIO_BASE;

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

  typedef enum { PLAYBACK, CONFIGURE, LOOP, RECORDING, PLAY, PAUSE, CLEAR, OVERDUB} State;
  State state = PLAYBACK;
  State looper_state;

  while (1) {
    // control polling
    int sw = *SW & 0x3FF;
    int keys = keys_pressed();

    //UI STATE MACHINE
    if (state == PLAYBACK) {
        *LEDS = sw;
        if (keys == 0b1000 && count_switches(sw) == 1) {  // if we want to configure an effect, press KEY3
            eff_2config = which_sw(sw);
            leds_show_strength(fx_strength[eff_2config]);
            state = CONFIGURE;
        }
        if((sw & 0b1000000000) != 0){
            looper_state = LOOP;
        }

    } else {  // state == CONFIGURE
        if (keys == 0b0010 && fx_strength[eff_2config] < 10) {  // KEY1 Increments
            fx_strength[eff_2config] += 1;
            leds_show_strength(fx_strength[eff_2config]);
    } else if (keys == 1 &&
                 fx_strength[eff_2config] > 0) {  // KEY0 decrements
                    fx_strength[eff_2config] -= 1;
        leds_show_strength(fx_strength[eff_2config]);
      } else if (keys ==
                 8) {  // KEY3 confirms configuration and exits to playback mode
        eff_2config = -1;
        state = PLAYBACK;
    }
}

    
    //FSMSSSS
    if(looper_state == LOOP && state != CONFIGURE){
        if(keys == 0b100){
            looper_state = RECORDING;
            //start writing to buffer
        }
        if(keys == 0b100 && state == RECORDING){
            //stop recording save the array length here 
            looper_state = PLAY;
        }
        if(looper_state == PLAY){
            if(keys == 0b10){
                looper_state == PAUSED;
                //pause it
            }
            if(keys == 0b1){
                looper_state == CLEAR;
                //clear it
            }
            if(keys = 0b100){
                looper_state == OVERDUB;
                //start recording new stuff and overwrite the buffer with the write ptr going back
            }
        }

        if(keys == 0b100 && state == OVERDUB){
            looper_state = PLAY;
        }

    }

    
    //SIGNAL CHAIN

    if (AUDIO->rarc != 0 && AUDIO->ralc != 0) {
        *LEFT = AUDIO->ldata;
        *RIGHT = AUDIO->rdata;

        if((sw & 0b10)!=0) wah(LEFT, RIGHT, fx_strength[1]);
        if ((sw & 0b100) != 0) distortion(LEFT, RIGHT,fx_strength[2]);
        if((sw&0b1000) != 0) chorus(LEFT, RIGHT, fx_strength[3]);
        if((sw&0b10000) != 0) vibrato(LEFT, RIGHT, fx_strength[4]);
        if((sw&0b100000) != 0) echo(LEFT, RIGHT, fx_strength[5]);
        if((sw&0b1000000) != 0) reverb(LEFT, RIGHT, fx_strength[6]);
        if(sw == 1) mute(LEFT, RIGHT, 0);

        //post processing chain loop pedal activities

        //fsms here
        if(looper_state == RECORDING){
            //start write ptr at 0, start recording
        }
        else if(looper_state == PLAY){
            //add the array output to the throughput 
        }
        else if(looper_state == PAUSE){
            //stop playing, reset read ptr to 0 
        }
        else if(looper_state == CLEAR){
            //GONE
        }
        else if(looper_state == OVERDUB){
            //obtain the output
            //overwrite the array at index 0
        }
    }
  }
  return 0;
}

// helper fuctions

int keys_pressed() {
  volatile int* edge = KEYS + 3;
  int pressed = *edge & 0xF;  // 4 keys
  if (pressed) {
    *edge = pressed;  // reset keys
  }
  return pressed;
}
int count_switches(int sw) {
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
  *LEDS = mask;
}
