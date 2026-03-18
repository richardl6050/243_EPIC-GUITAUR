#include "..\address_map.h"
// Map each register at its correct byte offset
#define AV_CTRL    ((volatile int*)(AV_CONFIG_BASE + 0))  // offset 0: control/go
#define AV_STATUS  ((volatile int*)(AV_CONFIG_BASE + 4))  // offset 4: status
#define AV_ADDR    ((volatile int*)(AV_CONFIG_BASE + 8))  // offset 8: I2C register address
#define AV_DATA    ((volatile int*)(AV_CONFIG_BASE + 12)) // offset 12: I2C data

//this is for WM8317 registers 7 bit address
//I2C ADDRESS IS 0x34 , but it could be 0x00 
//DATA IS 8 BITS
#define RESET 0xF
#define ACTIVE 0x9
#define SAMPLING 0x8
#define DA_INTERFACE_FORMAT 7
#define POWER_DOWN 0x6
#define DA_PATH_CONTROL 0x5
#define AA_PATH_CONTROL 0x4
#define R_HEADPHONE 0x3
#define L_HEADPHONE 0x2
#define R_LINE_IN 0x1
#define L_LINE_IN 0x0

int poll_ready();

void av_write(int AUDIO_REG, int DATA){
    *AV_CTRL = 0x00340000; //this is I2C address of WM8731
    *AV_ADDR = AUDIO_REG; //whatever register it is mapped to
    *AV_DATA = DATA; //data!!!
    poll_ready();

}

void init(){

    //reset
    av_write(RESET, 0);
    av_write(L_LINE_IN, 0b00001011); //documentation bro, turns on L_IN
    av_write(R_LINE_IN, 0b00001011); //same with right, 
    
    //schematics say that LHPOUT AND RHPOUT ARE LINKED SO TURN THEM BOTH ON
    av_write(L_HEADPHONE, 0b001111001); //default values
    av_write(R_HEADPHONE, 0b001111001);

    //path control
    av_write(AA_PATH_CONTROL, 0b00010000); //dacsel on, bypass off, no input leakage
    av_write(DA_PATH_CONTROL, 0b01000); //default DA path

    av_write(POWER_DOWN, 0); //turn everything on

    av_write(DA_INTERFACE_FORMAT, 0b1110); //32 bit ON
    //sampling
    av_write(SAMPLING, 0b00000000); // 48kHz sampling rate at 12.88 MHz external clock

    av_write(ACTIVE, 1); //turn it on baby
}

int poll_ready(){
    int ACK;
    while(1){
        int status = *AV_STATUS; // poll status register
        int RDY = status & 0b10;
        ACK = status & 0b1;

        //wait until the
       if(RDY){
        break;
       }
    }

    if(ACK){
        //tells us if its error
        return 0;
    }
    else{
        return 1;
    }


}