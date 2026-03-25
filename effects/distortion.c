#include "effects.h"
// LUT[0]   = 0 (expect 0)
// LUT[255] = 8382981 (expect close to 8388607)

// tanh LUT: 256 entries, input magnitude 0..4.0 -> 0..0x7FFFFF
// Index = bits[22:15] of 24-bit signed magnitude
// Sign is applied separately; interpolate with bits[14:0] for smoothness

// source is setup/tanh.py
const static int tanh_lut[256] = {
    0x0000000,  // [  0] x=0.0000
    0x0A08B90,  // [  1] x=0.0392
    0x1409909,  // [  2] x=0.0784
    0x1DFABDF,  // [  3] x=0.1176
    0x27D4A93,  // [  4] x=0.1569
    0x319001D,  // [  5] x=0.1961
    0x3B25D3C,  // [  6] x=0.2353
    0x448F9A5,  // [  7] x=0.2745
    0x4DC74FE,  // [  8] x=0.3137
    0x56C77B4,  // [  9] x=0.3529
    0x5F8B398,  // [ 10] x=0.3922
    0x680E44E,  // [ 11] x=0.4314
    0x704CF86,  // [ 12] x=0.4706
    0x784450B,  // [ 13] x=0.5098
    0x7FF1EA0,  // [ 14] x=0.5490
    0x8753FB9,  // [ 15] x=0.5882
    0x8E69515,  // [ 16] x=0.6275
    0x9531435,  // [ 17] x=0.6667
    0x9BABAC3,  // [ 18] x=0.7059
    0xA1D8DE5,  // [ 19] x=0.7451
    0xA7B9984,  // [ 20] x=0.7843
    0xAD4EF93,  // [ 21] x=0.8235
    0xB29A74C,  // [ 22] x=0.8627
    0xB79DC77,  // [ 23] x=0.9020
    0xBC5AEB3,  // [ 24] x=0.9412
    0xC0D40C9,  // [ 25] x=0.9804
    0xC50B808,  // [ 26] x=1.0196
    0xC903BAE,  // [ 27] x=1.0588
    0xCCBF45F,  // [ 28] x=1.0980
    0xD040BA5,  // [ 29] x=1.1373
    0xD38AB84,  // [ 30] x=1.1765
    0xD69FE18,  // [ 31] x=1.2157
    0xD982D3D,  // [ 32] x=1.2549
    0xDC36247,  // [ 33] x=1.2941
    0xDEBC5C6,  // [ 34] x=1.3333
    0xE117F52,  // [ 35] x=1.3725
    0xE34B560,  // [ 36] x=1.4118
    0xE558D25,  // [ 37] x=1.4510
    0xE742A78,  // [ 38] x=1.4902
    0xE90AFC4,  // [ 39] x=1.5294
    0xEAB3DFC,  // [ 40] x=1.5686
    0xEC3F491,  // [ 41] x=1.6078
    0xEDAF172,  // [ 42] x=1.6471
    0xEF0510B,  // [ 43] x=1.6863
    0xF042E4F,  // [ 44] x=1.7255
    0xF16A2B7,  // [ 45] x=1.7647
    0xF27C652,  // [ 46] x=1.8039
    0xF37AFCB,  // [ 47] x=1.8431
    0xF467479,  // [ 48] x=1.8824
    0xF54286A,  // [ 49] x=1.9216
    0xF60DE6F,  // [ 50] x=1.9608
    0xF6CA82E,  // [ 51] x=2.0000
    0xF77962D,  // [ 52] x=2.0392
    0xF81B7E0,  // [ 53] x=2.0784
    0xF8B1BBC,  // [ 54] x=2.1176
    0xF93CF3E,  // [ 55] x=2.1569
    0xF9BDEFF,  // [ 56] x=2.1961
    0xFA356BD,  // [ 57] x=2.2353
    0xFAA4169,  // [ 58] x=2.2745
    0xFB0A934,  // [ 59] x=2.3137
    0xFB6979C,  // [ 60] x=2.3529
    0xFBC1573,  // [ 61] x=2.3922
    0xFC12AF0,  // [ 62] x=2.4314
    0xFC5DFB2,  // [ 63] x=2.4706
    0xFCA3AD1,  // [ 64] x=2.5098
    0xFCE42E1,  // [ 65] x=2.5490
    0xFD1FE00,  // [ 66] x=2.5882
    0xFD571DA,  // [ 67] x=2.6275
    0xFD8A3B3,  // [ 68] x=2.6667
    0xFDB986C,  // [ 69] x=2.7059
    0xFDE548D,  // [ 70] x=2.7451
    0xFE0DC48,  // [ 71] x=2.7843
    0xFE33382,  // [ 72] x=2.8235
    0xFE55DD7,  // [ 73] x=2.8627
    0xFE75E9E,  // [ 74] x=2.9020
    0xFE938F0,  // [ 75] x=2.9412
    0xFEAEFAC,  // [ 76] x=2.9804
    0xFEC8579,  // [ 77] x=3.0196
    0xFEDFCCD,  // [ 78] x=3.0588
    0xFEF57F0,  // [ 79] x=3.0980
    0xFF098FD,  // [ 80] x=3.1373
    0xFF1C1E9,  // [ 81] x=3.1765
    0xFF2D482,  // [ 82] x=3.2157
    0xFF3D275,  // [ 83] x=3.2549
    0xFF4BD4E,  // [ 84] x=3.2941
    0xFF5967B,  // [ 85] x=3.3333
    0xFF65F51,  // [ 86] x=3.3725
    0xFF71909,  // [ 87] x=3.4118
    0xFF7C4C7,  // [ 88] x=3.4510
    0xFF86396,  // [ 89] x=3.4902
    0xFF8F670,  // [ 90] x=3.5294
    0xFF97E3B,  // [ 91] x=3.5686
    0xFF9FBCB,  // [ 92] x=3.6078
    0xFFA6FE6,  // [ 93] x=3.6471
    0xFFADB42,  // [ 94] x=3.6863
    0xFFB3E87,  // [ 95] x=3.7255
    0xFFB9A51,  // [ 96] x=3.7647
    0xFFBEF31,  // [ 97] x=3.8039
    0xFFC3DAB,  // [ 98] x=3.8431
    0xFFC863A,  // [ 99] x=3.8824
    0xFFCC952,  // [100] x=3.9216
    0xFFD075B,  // [101] x=3.9608
    0xFFD40B7,  // [102] x=4.0000
    0xFFD75C0,  // [103] x=4.0392
    0xFFDA6CA,  // [104] x=4.0784
    0xFFDD421,  // [105] x=4.1176
    0xFFDFE0D,  // [106] x=4.1569
    0xFFE24CF,  // [107] x=4.1961
    0xFFE48A6,  // [108] x=4.2353
    0xFFE69C8,  // [109] x=4.2745
    0xFFE886C,  // [110] x=4.3137
    0xFFEA4BF,  // [111] x=4.3529
    0xFFEBEF0,  // [112] x=4.3922
    0xFFED727,  // [113] x=4.4314
    0xFFEED8B,  // [114] x=4.4706
    0xFFF023E,  // [115] x=4.5098
    0xFFF1562,  // [116] x=4.5490
    0xFFF2714,  // [117] x=4.5882
    0xFFF3771,  // [118] x=4.6275
    0xFFF4691,  // [119] x=4.6667
    0xFFF548E,  // [120] x=4.7059
    0xFFF617C,  // [121] x=4.7451
    0xFFF6D71,  // [122] x=4.7843
    0xFFF787F,  // [123] x=4.8235
    0xFFF82B8,  // [124] x=4.8627
    0xFFF8C2B,  // [125] x=4.9020
    0xFFF94E7,  // [126] x=4.9412
    0xFFF9CFB,  // [127] x=4.9804
    0xFFFA473,  // [128] x=5.0196
    0xFFFAB5B,  // [129] x=5.0588
    0xFFFB1BE,  // [130] x=5.0980
    0xFFFB7A5,  // [131] x=5.1373
    0xFFFBD1A,  // [132] x=5.1765
    0xFFFC226,  // [133] x=5.2157
    0xFFFC6D0,  // [134] x=5.2549
    0xFFFCB20,  // [135] x=5.2941
    0xFFFCF1D,  // [136] x=5.3333
    0xFFFD2CD,  // [137] x=5.3725
    0xFFFD636,  // [138] x=5.4118
    0xFFFD95D,  // [139] x=5.4510
    0xFFFDC47,  // [140] x=5.4902
    0xFFFDEF9,  // [141] x=5.5294
    0xFFFE176,  // [142] x=5.5686
    0xFFFE3C4,  // [143] x=5.6078
    0xFFFE5E5,  // [144] x=5.6471
    0xFFFE7DD,  // [145] x=5.6863
    0xFFFE9AF,  // [146] x=5.7255
    0xFFFEB5E,  // [147] x=5.7647
    0xFFFECEC,  // [148] x=5.8039
    0xFFFEE5D,  // [149] x=5.8431
    0xFFFEFB1,  // [150] x=5.8824
    0xFFFF0EC,  // [151] x=5.9216
    0xFFFF20F,  // [152] x=5.9608
    0xFFFF31C,  // [153] x=6.0000
    0xFFFF415,  // [154] x=6.0392
    0xFFFF4FB,  // [155] x=6.0784
    0xFFFF5D0,  // [156] x=6.1176
    0xFFFF695,  // [157] x=6.1569
    0xFFFF74A,  // [158] x=6.1961
    0xFFFF7F3,  // [159] x=6.2353
    0xFFFF88E,  // [160] x=6.2745
    0xFFFF91E,  // [161] x=6.3137
    0xFFFF9A3,  // [162] x=6.3529
    0xFFFFA1D,  // [163] x=6.3922
    0xFFFFA8F,  // [164] x=6.4314
    0xFFFFAF8,  // [165] x=6.4706
    0xFFFFB59,  // [166] x=6.5098
    0xFFFFBB3,  // [167] x=6.5490
    0xFFFFC06,  // [168] x=6.5882
    0xFFFFC53,  // [169] x=6.6275
    0xFFFFC99,  // [170] x=6.6667
    0xFFFFCDB,  // [171] x=6.7059
    0xFFFFD18,  // [172] x=6.7451
    0xFFFFD50,  // [173] x=6.7843
    0xFFFFD84,  // [174] x=6.8235
    0xFFFFDB4,  // [175] x=6.8627
    0xFFFFDE0,  // [176] x=6.9020
    0xFFFFE09,  // [177] x=6.9412
    0xFFFFE2F,  // [178] x=6.9804
    0xFFFFE52,  // [179] x=7.0196
    0xFFFFE72,  // [180] x=7.0588
    0xFFFFE90,  // [181] x=7.0980
    0xFFFFEAC,  // [182] x=7.1373
    0xFFFFEC5,  // [183] x=7.1765
    0xFFFFEDD,  // [184] x=7.2157
    0xFFFFEF3,  // [185] x=7.2549
    0xFFFFF07,  // [186] x=7.2941
    0xFFFFF1A,  // [187] x=7.3333
    0xFFFFF2B,  // [188] x=7.3725
    0xFFFFF3B,  // [189] x=7.4118
    0xFFFFF4A,  // [190] x=7.4510
    0xFFFFF58,  // [191] x=7.4902
    0xFFFFF64,  // [192] x=7.5294
    0xFFFFF70,  // [193] x=7.5686
    0xFFFFF7B,  // [194] x=7.6078
    0xFFFFF85,  // [195] x=7.6471
    0xFFFFF8E,  // [196] x=7.6863
    0xFFFFF96,  // [197] x=7.7255
    0xFFFFF9E,  // [198] x=7.7647
    0xFFFFFA6,  // [199] x=7.8039
    0xFFFFFAC,  // [200] x=7.8431
    0xFFFFFB3,  // [201] x=7.8824
    0xFFFFFB8,  // [202] x=7.9216
    0xFFFFFBE,  // [203] x=7.9608
    0xFFFFFC3,  // [204] x=8.0000
    0xFFFFFC7,  // [205] x=8.0392
    0xFFFFFCB,  // [206] x=8.0784
    0xFFFFFCF,  // [207] x=8.1176
    0xFFFFFD3,  // [208] x=8.1569
    0xFFFFFD6,  // [209] x=8.1961
    0xFFFFFD9,  // [210] x=8.2353
    0xFFFFFDC,  // [211] x=8.2745
    0xFFFFFDF,  // [212] x=8.3137
    0xFFFFFE1,  // [213] x=8.3529
    0xFFFFFE3,  // [214] x=8.3922
    0xFFFFFE6,  // [215] x=8.4314
    0xFFFFFE7,  // [216] x=8.4706
    0xFFFFFE9,  // [217] x=8.5098
    0xFFFFFEB,  // [218] x=8.5490
    0xFFFFFEC,  // [219] x=8.5882
    0xFFFFFEE,  // [220] x=8.6275
    0xFFFFFEF,  // [221] x=8.6667
    0xFFFFFF0,  // [222] x=8.7059
    0xFFFFFF1,  // [223] x=8.7451
    0xFFFFFF2,  // [224] x=8.7843
    0xFFFFFF3,  // [225] x=8.8235
    0xFFFFFF4,  // [226] x=8.8627
    0xFFFFFF5,  // [227] x=8.9020
    0xFFFFFF6,  // [228] x=8.9412
    0xFFFFFF6,  // [229] x=8.9804
    0xFFFFFF7,  // [230] x=9.0196
    0xFFFFFF8,  // [231] x=9.0588
    0xFFFFFF8,  // [232] x=9.0980
    0xFFFFFF9,  // [233] x=9.1373
    0xFFFFFF9,  // [234] x=9.1765
    0xFFFFFFA,  // [235] x=9.2157
    0xFFFFFFA,  // [236] x=9.2549
    0xFFFFFFA,  // [237] x=9.2941
    0xFFFFFFB,  // [238] x=9.3333
    0xFFFFFFB,  // [239] x=9.3725
    0xFFFFFFB,  // [240] x=9.4118
    0xFFFFFFC,  // [241] x=9.4510
    0xFFFFFFC,  // [242] x=9.4902
    0xFFFFFFC,  // [243] x=9.5294
    0xFFFFFFC,  // [244] x=9.5686
    0xFFFFFFD,  // [245] x=9.6078
    0xFFFFFFD,  // [246] x=9.6471
    0xFFFFFFD,  // [247] x=9.6863
    0xFFFFFFD,  // [248] x=9.7255
    0xFFFFFFD,  // [249] x=9.7647
    0xFFFFFFD,  // [250] x=9.8039
    0xFFFFFFD,  // [251] x=9.8431
    0xFFFFFFE,  // [252] x=9.8824
    0xFFFFFFE,  // [253] x=9.9216
    0xFFFFFFE,  // [254] x=9.9608
    0xFFFFFFE  // [255] x=10.0000
};



//q15 floats
const int drive_levels[10] = {
    0x8000,  0xA14A,  0xCB30,  0x10000, 0x1428A,
    0x19660, 0x20000, 0x28514, 0x32CC0, 0x40000,
};

void distortion(int *L, int *R, int effectStrength) {
  // interpret 24 bit data
  // given 24 bit data, clip it following a tanh curve
  // implement this using a look up table

  // consider 10 levels, a higher effect strength will clip more aggresively

  // arctan clipping, the upper bits of the signal will determine the x-input
  // into arctan

  // the higher the effect strength, we multiply the index coefficient by
  // effectStrength

  // final approach:
  /*
  1. develop a 256 entry LUT for bits 16-23 - sign extend after clipping

  2. Upper MSB will be adjusted by a factor based on "effectStrength"

  3. The bits 16-23 will index the LUT

  4. Take into account the sign of the signal

  //note this will sound like shit due to undersampling, desired sampling freq
  is at least 48k ideally 96k

  it kinda sounds bad

  */

  // just use mono processing
  int drive = drive_levels[effectStrength];

  int signal = *L;
    //just use mono processing
    if(signal < 0xFFFF){
        return;
    }



    int sign = (signal < 0) ? -1 : 1;
    int abs_value = (signal < 0) ? -signal : signal; //mask
    
    long long scaled = ((long)abs_value * drive) >> 23; //scaled signal
   

    int index = scaled >> 23; //shift the bits 
    index = index & 0xFF; //only take the upper 8 bits
    signal = tanh_lut[index] >> 2; //indexed by upper 8 bits
    //sign extend
    signal *= sign;

    *L = signal;
    *R = signal;

    //tune distortion knee and values
}
