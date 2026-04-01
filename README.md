# 243_EPIC-GUITAUR

# Project Overview
* Functionality: Multiple guitar effects, controlled using switches and keys
* Control structure:
  * Switches act as selectors, if only one effect is on, allow changes to the effect parameter
  * Keys are main adjuster knobs basically
      * Interface with LED as a meter

* main: Audio interaction, control signals and main loop
-> distortion.c
-> reverb.c
-> delay.c
-> Chorus.c

  Function Parameters:
  * (Signal_IN, setting) 
* Ensure parallel structure


Timeline:
* Wednesday 18/03/2025
  *   get audio passthrough, maybe a simple distortion effect
  *   Complete program skeleton
  *   Basic control structure done
  Richard: Control structure, LEDs and distortion
  Vitto: Audio pass-through, sampling rate increase
* Monday: 24/03/2025
  * Finish time domain effects: Reverb, distortion, chorus, delay, tremolo
  * Began developing an IIR biquad structure for autowah
  * Richard: Tuned delay and reverb to suit 48k sampling rate, developed comb filtering techniques demonstrated on video
  * Vitto: Made chorus and distortion better using tanh clipping, configured the Wm8317 audio codec to support LINE_IN input @ 48kHz
* Wednesday: 25/03/2025 
    * next steps: finish up IIR filter for auto wah, maybe add tremolo and more time domain effects, setup tuning control configuration
    * more tuning with distortion and chorus parameters (IRL testing required)








  
 
 
