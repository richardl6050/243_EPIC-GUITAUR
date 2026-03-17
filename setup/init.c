#include "..\address_map.h"

volatile int* AV_CONFIG = (int*) AV_CONFIG_BASE; // set it by default
void init(){
    
    //control register cofiguration
    int control = 0b1; //trigger config of audio card
    //its fine to poll it
    
    //7 bits for register location of audio
    //8 bits for on board audio data register
    int sampling_register = 0b1000;
    //control registers are 9 bits only
    int sampling_control = 0b000000000; //set it back to default man
    *AV_CONFIG = control; //
    poll_ready();
    *(AV_CONFIG+2) = sampling_register;
    poll_ready();
    *(AV_CONFIG+3) = sampling_control;
    poll_ready();
    
    /* 0 means , 48kHz sampling rate*/
    int path_control_register = 0b0000100;
    int path_control = 0b00001010;

    *AV_CONFIG= control; //
    poll_ready();
    *(AV_CONFIG+2) = sampling_register;
    poll_ready();
    *(AV_CONFIG+3) = sampling_control;
    poll_ready();

    int activate_register = 0b1001;
    int activate = 0b1;

    *AV_CONFIG = control;
    poll_ready();
    *(AV_CONFIG+2) = activate_register;
    poll_ready();
    *(AV_CONFIG+3) = activate;
    poll_ready();
}

void poll_ready(){
    while(1){
        int status = *(AV_CONFIG_BASE+1); // poll status register
        int RDY = status & 0b10;
        int ACK = status & 0b1;

        //wait until the
        if(RDY){
            break;
        }
    }
}