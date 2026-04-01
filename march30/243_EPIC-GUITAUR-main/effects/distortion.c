#include "effects.h"
// LUT[0]   = 0 (expect 0)
// LUT[255] = 8382981 (expect close to 8388607)

// tanh LUT: 256 entries, input magnitude 0..4.0 -> 0..0x7FFFFF
// Index = bits[22:15] of 24-bit signed magnitude
// Sign is applied separately; interpolate with bits[14:0] for smoothness

// source is setup/tanh.py
const static int tanh_lut[256] = {

    0x0000000,  // [  0] x=0.0000
    0x0A09B5C,  // [  1] x=0.0196
    0x141171F,  // [  2] x=0.0392
    0x1E153CB,  // [  3] x=0.0588
    0x2813211,  // [  4] x=0.0784
    0x32092EE,  // [  5] x=0.0980
    0x3BF57BE,  // [  6] x=0.1176
    0x45D625A,  // [  7] x=0.1373
    0x4FA9527,  // [  8] x=0.1569
    0x596D330,  // [  9] x=0.1765
    0x632003A,  // [ 10] x=0.1961
    0x6CC00D8,  // [ 11] x=0.2157
    0x764BA79,  // [ 12] x=0.2353
    0x7FC137F,  // [ 13] x=0.2549
    0x891F34A,  // [ 14] x=0.2745
    0x9264247,  // [ 15] x=0.2941
    0x9B8E9FC,  // [ 16] x=0.3137
    0xA49D514,  // [ 17] x=0.3333
    0xAD8EF68,  // [ 18] x=0.3529
    0xB662604,  // [ 19] x=0.3725
    0xBF16730,  // [ 20] x=0.3922
    0xC7AA275,  // [ 21] x=0.4118
    0xD01C89C,  // [ 22] x=0.4314
    0xD86CBB4,  // [ 23] x=0.4510
    0xE099F0D,  // [ 24] x=0.4706
    0xE8A373D,  // [ 25] x=0.4902
    0xF088A17,  // [ 26] x=0.5098
    0xF848EAB,  // [ 27] x=0.5294
    0xFFE3D40,  // [ 28] x=0.5490
    0x10758F4D,  // [ 29] x=0.5686
    0x10EA7F72,  // [ 30] x=0.5882
    0x115D0973,  // [ 31] x=0.6078
    0x11CD2A2A,  // [ 32] x=0.6275
    0x123ADF82,  // [ 33] x=0.6471
    0x12A6286B,  // [ 34] x=0.6667
    0x130F04CE,  // [ 35] x=0.6863
    0x13757587,  // [ 36] x=0.7059
    0x13D97C54,  // [ 37] x=0.7255
    0x143B1BCB,  // [ 38] x=0.7451
    0x149A5751,  // [ 39] x=0.7647
    0x14F7330A,  // [ 40] x=0.7843
    0x1551B3D0,  // [ 41] x=0.8039
    0x15A9DF27,  // [ 42] x=0.8235
    0x15FFBB2F,  // [ 43] x=0.8431
    0x16534E99,  // [ 44] x=0.8627
    0x16A4A09D,  // [ 45] x=0.8824
    0x16F3B8EF,  // [ 46] x=0.9020
    0x17409FB0,  // [ 47] x=0.9216
    0x178B5D67,  // [ 48] x=0.9412
    0x17D3FAF8,  // [ 49] x=0.9608
    0x181A8193,  // [ 50] x=0.9804
    0x185EFAB4,  // [ 51] x=1.0000
    0x18A17011,  // [ 52] x=1.0196
    0x18E1EB97,  // [ 53] x=1.0392
    0x1920775E,  // [ 54] x=1.0588
    0x195D1DA2,  // [ 55] x=1.0784
    0x1997E8BE,  // [ 56] x=1.0980
    0x19D0E321,  // [ 57] x=1.1176
    0x1A08174A,  // [ 58] x=1.1373
    0x1A3D8FBF,  // [ 59] x=1.1569
    0x1A715709,  // [ 60] x=1.1765
    0x1AA377AF,  // [ 61] x=1.1961
    0x1AD3FC31,  // [ 62] x=1.2157
    0x1B02EEFF,  // [ 63] x=1.2353
    0x1B305A7A,  // [ 64] x=1.2549
    0x1B5C48EE,  // [ 65] x=1.2745
    0x1B86C48E,  // [ 66] x=1.2941
    0x1BAFD771,  // [ 67] x=1.3137
    0x1BD78B8D,  // [ 68] x=1.3333
    0x1BFDEAB8,  // [ 69] x=1.3529
    0x1C22FEA5,  // [ 70] x=1.3725
    0x1C46D0DC,  // [ 71] x=1.3922
    0x1C696AC1,  // [ 72] x=1.4118
    0x1C8AD58C,  // [ 73] x=1.4314
    0x1CAB1A4A,  // [ 74] x=1.4510
    0x1CCA41DB,  // [ 75] x=1.4706
    0x1CE854F0,  // [ 76] x=1.4902
    0x1D055C0E,  // [ 77] x=1.5098
    0x1D215F89,  // [ 78] x=1.5294
    0x1D3C6786,  // [ 79] x=1.5490
    0x1D567BF9,  // [ 80] x=1.5686
    0x1D6FA4A7,  // [ 81] x=1.5882
    0x1D87E923,  // [ 82] x=1.6078
    0x1D9F50D1,  // [ 83] x=1.6275
    0x1DB5E2E4,  // [ 84] x=1.6471
    0x1DCBA660,  // [ 85] x=1.6667
    0x1DE0A218,  // [ 86] x=1.6863
    0x1DF4DCB0,  // [ 87] x=1.7059
    0x1E085C9F,  // [ 88] x=1.7255
    0x1E1B282B,  // [ 89] x=1.7451
    0x1E2D456F,  // [ 90] x=1.7647
    0x1E3EBA57,  // [ 91] x=1.7843
    0x1E4F8CA4,  // [ 92] x=1.8039
    0x1E5FC1EB,  // [ 93] x=1.8235
    0x1E6F5F97,  // [ 94] x=1.8431
    0x1E7E6AE7,  // [ 95] x=1.8627
    0x1E8CE8F4,  // [ 96] x=1.8824
    0x1E9ADEAB,  // [ 97] x=1.9020
    0x1EA850D5,  // [ 98] x=1.9216
    0x1EB54413,  // [ 99] x=1.9412
    0x1EC1BCE0,  // [100] x=1.9608
    0x1ECDBF92,  // [101] x=1.9804
    0x1ED9505D,  // [102] x=2.0000
    0x1EE47351,  // [103] x=2.0196
    0x1EEF2C5A,  // [104] x=2.0392
    0x1EF97F46,  // [105] x=2.0588
    0x1F036FC1,  // [106] x=2.0784
    0x1F0D0158,  // [107] x=2.0980
    0x1F163779,  // [108] x=2.1176
    0x1F1F1574,  // [109] x=2.1373
    0x1F279E7D,  // [110] x=2.1569
    0x1F2FD5AD,  // [111] x=2.1765
    0x1F37BDFF,  // [112] x=2.1961
    0x1F3F5A56,  // [113] x=2.2157
    0x1F46AD7B,  // [114] x=2.2353
    0x1F4DBA1C,  // [115] x=2.2549
    0x1F5482D2,  // [116] x=2.2745
    0x1F5B0A1E,  // [117] x=2.2941
    0x1F615269,  // [118] x=2.3137
    0x1F675E08,  // [119] x=2.3333
    0x1F6D2F39,  // [120] x=2.3529
    0x1F72C826,  // [121] x=2.3725
    0x1F782AE8,  // [122] x=2.3922
    0x1F7D5980,  // [123] x=2.4118
    0x1F8255E0,  // [124] x=2.4314
    0x1F8721E8,  // [125] x=2.4510
    0x1F8BBF65,  // [126] x=2.4706
    0x1F903014,  // [127] x=2.4902
    0x1F9475A2,  // [128] x=2.5098
    0x1F9891AD,  // [129] x=2.5294
    0x1F9C85C4,  // [130] x=2.5490
    0x1FA05364,  // [131] x=2.5686
    0x1FA3FC02,  // [132] x=2.5882
    0x1FA78100,  // [133] x=2.6078
    0x1FAAE3B6,  // [134] x=2.6275
    0x1FAE256E,  // [135] x=2.6471
    0x1FB14767,  // [136] x=2.6667
    0x1FB44AD3,  // [137] x=2.6863
    0x1FB730D9,  // [138] x=2.7059
    0x1FB9FA95,  // [139] x=2.7255
    0x1FBCA91A,  // [140] x=2.7451
    0x1FBF3D6F,  // [141] x=2.7647
    0x1FC1B891,  // [142] x=2.7843
    0x1FC41B75,  // [143] x=2.8039
    0x1FC66706,  // [144] x=2.8235
    0x1FC89C26,  // [145] x=2.8431
    0x1FCABBB0,  // [146] x=2.8627
    0x1FCCC674,  // [147] x=2.8824
    0x1FCEBD3E,  // [148] x=2.9020
    0x1FD0A0CF,  // [149] x=2.9216
    0x1FD271E2,  // [150] x=2.9412
    0x1FD4312C,  // [151] x=2.9608
    0x1FD5DF59,  // [152] x=2.9804
    0x1FD77D10,  // [153] x=3.0000
    0x1FD90AF2,  // [154] x=3.0196
    0x1FDA899A,  // [155] x=3.0392
    0x1FDBF99B,  // [156] x=3.0588
    0x1FDD5B84,  // [157] x=3.0784
    0x1FDEAFE0,  // [158] x=3.0980
    0x1FDFF733,  // [159] x=3.1176
    0x1FE131FB,  // [160] x=3.1373
    0x1FE260B4,  // [161] x=3.1569
    0x1FE383D3,  // [162] x=3.1765
    0x1FE49BCA,  // [163] x=3.1961
    0x1FE5A905,  // [164] x=3.2157
    0x1FE6ABEF,  // [165] x=3.2353
    0x1FE7A4EB,  // [166] x=3.2549
    0x1FE8945B,  // [167] x=3.2745
    0x1FE97A9D,  // [168] x=3.2941
    0x1FEA5809,  // [169] x=3.3137
    0x1FEB2CF8,  // [170] x=3.3333
    0x1FEBF9BB,  // [171] x=3.3529
    0x1FECBEA3,  // [172] x=3.3725
    0x1FED7BFE,  // [173] x=3.3922
    0x1FEE3214,  // [174] x=3.4118
    0x1FEEE12D,  // [175] x=3.4314
    0x1FEF898E,  // [176] x=3.4510
    0x1FF02B79,  // [177] x=3.4706
    0x1FF0C72D,  // [178] x=3.4902
    0x1FF15CE6,  // [179] x=3.5098
    0x1FF1ECE1,  // [180] x=3.5294
    0x1FF27754,  // [181] x=3.5490
    0x1FF2FC76,  // [182] x=3.5686
    0x1FF37C7C,  // [183] x=3.5882
    0x1FF3F797,  // [184] x=3.6078
    0x1FF46DF8,  // [185] x=3.6275
    0x1FF4DFCD,  // [186] x=3.6471
    0x1FF54D43,  // [187] x=3.6667
    0x1FF5B684,  // [188] x=3.6863
    0x1FF61BBB,  // [189] x=3.7059
    0x1FF67D0F,  // [190] x=3.7255
    0x1FF6DAA5,  // [191] x=3.7451
    0x1FF734A3,  // [192] x=3.7647
    0x1FF78B2C,  // [193] x=3.7843
    0x1FF7DE62,  // [194] x=3.8039
    0x1FF82E66,  // [195] x=3.8235
    0x1FF87B56,  // [196] x=3.8431
    0x1FF8C552,  // [197] x=3.8627
    0x1FF90C76,  // [198] x=3.8824
    0x1FF950DE,  // [199] x=3.9020
    0x1FF992A5,  // [200] x=3.9216
    0x1FF9D1E6,  // [201] x=3.9412
    0x1FFA0EB8,  // [202] x=3.9608
    0x1FFA4933,  // [203] x=3.9804
    0x1FFA8170,  // [204] x=4.0000
    0x1FFAB783,  // [205] x=4.0196
    0x1FFAEB82,  // [206] x=4.0392
    0x1FFB1D81,  // [207] x=4.0588
    0x1FFB4D94,  // [208] x=4.0784
    0x1FFB7BCF,  // [209] x=4.0980
    0x1FFBA842,  // [210] x=4.1176
    0x1FFBD301,  // [211] x=4.1373
    0x1FFBFC1A,  // [212] x=4.1569
    0x1FFC239F,  // [213] x=4.1765
    0x1FFC49A0,  // [214] x=4.1961
    0x1FFC6E2A,  // [215] x=4.2157
    0x1FFC914D,  // [216] x=4.2353
    0x1FFCB316,  // [217] x=4.2549
    0x1FFCD392,  // [218] x=4.2745
    0x1FFCF2CF,  // [219] x=4.2941
    0x1FFD10D8,  // [220] x=4.3137
    0x1FFD2DBA,  // [221] x=4.3333
    0x1FFD497F,  // [222] x=4.3529
    0x1FFD6434,  // [223] x=4.3725
    0x1FFD7DE1,  // [224] x=4.3922
    0x1FFD9692,  // [225] x=4.4118
    0x1FFDAE4F,  // [226] x=4.4314
    0x1FFDC523,  // [227] x=4.4510
    0x1FFDDB17,  // [228] x=4.4706
    0x1FFDF032,  // [229] x=4.4902
    0x1FFE047D,  // [230] x=4.5098
    0x1FFE1801,  // [231] x=4.5294
    0x1FFE2AC5,  // [232] x=4.5490
    0x1FFE3CD0,  // [233] x=4.5686
    0x1FFE4E29,  // [234] x=4.5882
    0x1FFE5ED8,  // [235] x=4.6078
    0x1FFE6EE2,  // [236] x=4.6275
    0x1FFE7E4F,  // [237] x=4.6471
    0x1FFE8D24,  // [238] x=4.6667
    0x1FFE9B66,  // [239] x=4.6863
    0x1FFEA91D,  // [240] x=4.7059
    0x1FFEB64C,  // [241] x=4.7255
    0x1FFEC2FA,  // [242] x=4.7451
    0x1FFECF2B,  // [243] x=4.7647
    0x1FFEDAE3,  // [244] x=4.7843
    0x1FFEE629,  // [245] x=4.8039
    0x1FFEF0FF,  // [246] x=4.8235
    0x1FFEFB6B,  // [247] x=4.8431
    0x1FFF0570,  // [248] x=4.8627
    0x1FFF0F13,  // [249] x=4.8824
    0x1FFF1857,  // [250] x=4.9020
    0x1FFF213F,  // [251] x=4.9216
    0x1FFF29D0,  // [252] x=4.9412
    0x1FFF320C,  // [253] x=4.9608
    0x1FFF39F8,  // [254] x=4.9804
    0x1FFF4195  // [255] x=5.0000
};



// bitshift adjust these parameters for driving
const float drive_levels[10] = {
    1.0f, 1.5f, 2.0f, 2.3f, 2.6f, 2.9f, 3.2f, 3.4f, 3.6f, 3.8f
};

void distortion(int *L, int *R, int effectStrength) {
  // interpret 24 bit data LEFT UPPER SB
  // given 24 bit data, clip it following a tanh curve
  // implement this using a look up table

  // consider 10 levels, a higher effect strength will clip more aggresively

  // arctan clipping, the upper bits of the signal will determine the x-input
  // into arctan

  // the higher the effect strength, we multiply the index coefficient by
  // effectStrength

  // final approach:
  /*
  1. develop a 256 entry LUT for bits 30-23 - sign extend after clipping

  2. Upper MSB will be adjusted by a factor based on "effectStrength"

  3. The bits 30-23 will index the LUT

  4. Take into account the sign of the signal

  //note this will sound like shit due to undersampling, desired sampling freq
  is at least 48k ideally 96k

  it kinda sounds bad

  */

    int drive = drive_levels[effectStrength];

    int signal = *L;
    //just use mono processing

    int sign = (signal < 0) ? -1 : 1;
    int abs_value = (signal < 0) ? -signal : signal; //mask
    
    //noise gate
    if(abs_value < 0xFFFF){
        return;
    }
    
    float scaled = abs_value * drive * 1.5f; //scaled signal
   

    int index = (int) scaled >> (23); //shift the bits 
    index = index & 0xFF; //only take the upper 8 bits
    signal = tanh_lut[index] << 0; //indexed by upper 8 bits
    //sign extend
    signal *= sign;

    *L = signal;
    *R = signal;

    //tune distortion knee and values
}
