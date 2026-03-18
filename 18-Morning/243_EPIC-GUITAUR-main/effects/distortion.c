#include "effects.h"

void distortion(int *L, int *R, int effectStrength){
    //interpret 24 bit data
    int threshold = 0x7FFFFF >> 1; //clip at half input set to less for more distortion

    if(*L > threshold){
        *L = threshold;
    }
    else if(*L < -threshold){
        *L = -threshold;
    }


    *R = *L;
    
}