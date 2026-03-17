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
* Monday: 23/03/2025
  * Finish time domain effects: Reverb, distortion, chorus, delay, tremolo
  * Progress check on frequency domain effects
  * Richard: Reverb, delay, distortion settings
  * Vitto: Dynamic Wah pedal, chorus, tremolo, LFOs, FFT





  
 
 
