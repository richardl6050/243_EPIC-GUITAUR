#include "effects.h"
// LUT[0]   = 0 (expect 0)
// LUT[255] = 8382981 (expect close to 8388607)

// tanh LUT: 256 entries, input magnitude 0..4.0 -> 0..0x7FFFFF
// Index = bits[22:15] of 24-bit signed magnitude
// Sign is applied separately; interpolate with bits[14:0] for smoothness

// source is setup/tanh.py
const static int tanh_lut[256] = {
    0x0000000,  // [  0] x=0.0000
    0x201F737,  // [  1] x=0.0157
    0x403ADB5,  // [  2] x=0.0314
    0x604E2E1,  // [  3] x=0.0471
    0x8055663,  // [  4] x=0.0627
    0xA04C845,  // [  5] x=0.0784
    0xC02F911,  // [  6] x=0.0941
    0xDFFA9F1,  // [  7] x=0.1098
    0xFFA9CD0,  // [  8] x=0.1255
    0x11F39474,  // [  9] x=0.1412
    0x13EA549B,  // [ 10] x=0.1569
    0x15DEA21C,  // [ 11] x=0.1725
    0x17D042FB,  // [ 12] x=0.1882
    0x19BEFE87,  // [ 13] x=0.2039
    0x1BAA9D72,  // [ 14] x=0.2196
    0x1D92E9E5,  // [ 15] x=0.2353
    0x1F77AF97,  // [ 16] x=0.2510
    0x2158BBE5,  // [ 17] x=0.2667
    0x2335DDDD,  // [ 18] x=0.2824
    0x250EE653,  // [ 19] x=0.2980
    0x26E3A7F1,  // [ 20] x=0.3137
    0x28B3F743,  // [ 21] x=0.3294
    0x2A7FAAC1,  // [ 22] x=0.3451
    0x2C469ADD,  // [ 23] x=0.3608
    0x2E08A20A,  // [ 24] x=0.3765
    0x2FC59CC2,  // [ 25] x=0.3922
    0x317D698B,  // [ 26] x=0.4078
    0x332FE8FB,  // [ 27] x=0.4235
    0x34DCFDBC,  // [ 28] x=0.4392
    0x36848C8B,  // [ 29] x=0.4549
    0x38267C36,  // [ 30] x=0.4706
    0x39C2B59B,  // [ 31] x=0.4863
    0x3B5923A5,  // [ 32] x=0.5020
    0x3CE9B343,  // [ 33] x=0.5176
    0x3E745369,  // [ 34] x=0.5333
    0x3FF8F502,  // [ 35] x=0.5490
    0x41778AEA,  // [ 36] x=0.5647
    0x42F009E6,  // [ 37] x=0.5804
    0x44626897,  // [ 38] x=0.5961
    0x45CE9F70,  // [ 39] x=0.6118
    0x4734A8AB,  // [ 40] x=0.6275
    0x4894803A,  // [ 41] x=0.6431
    0x49EE23BD,  // [ 42] x=0.6588
    0x4B419270,  // [ 43] x=0.6745
    0x4C8ECD22,  // [ 44] x=0.6902
    0x4DD5D61F,  // [ 45] x=0.7059
    0x4F16B12A,  // [ 46] x=0.7216
    0x50516365,  // [ 47] x=0.7373
    0x5185F348,  // [ 48] x=0.7529
    0x52B4688F,  // [ 49] x=0.7686
    0x53DCCC28,  // [ 50] x=0.7843
    0x54FF2829,  // [ 51] x=0.8000
    0x561B87BA,  // [ 52] x=0.8157
    0x5731F70B,  // [ 53] x=0.8314
    0x58428340,  // [ 54] x=0.8471
    0x594D3A65,  // [ 55] x=0.8627
    0x5A522B5D,  // [ 56] x=0.8784
    0x5B5165D7,  // [ 57] x=0.8941
    0x5C4AFA38,  // [ 58] x=0.9098
    0x5D3EF994,  // [ 59] x=0.9255
    0x5E2D759F,  // [ 60] x=0.9412
    0x5F16809B,  // [ 61] x=0.9569
    0x5FFA2D51,  // [ 62] x=0.9725
    0x60D88EFE,  // [ 63] x=0.9882
    0x61B1B94E,  // [ 64] x=1.0039
    0x6285C048,  // [ 65] x=1.0196
    0x6354B84B,  // [ 66] x=1.0353
    0x641EB5FB,  // [ 67] x=1.0510
    0x64E3CE3C,  // [ 68] x=1.0667
    0x65A41626,  // [ 69] x=1.0824
    0x665FA2FB,  // [ 70] x=1.0980
    0x67168A1F,  // [ 71] x=1.1137
    0x67C8E10C,  // [ 72] x=1.1294
    0x6876BD50,  // [ 73] x=1.1451
    0x6920347C,  // [ 74] x=1.1608
    0x69C55C26,  // [ 75] x=1.1765
    0x6A6649DC,  // [ 76] x=1.1922
    0x6B03131D,  // [ 77] x=1.2078
    0x6B9BCD58,  // [ 78] x=1.2235
    0x6C308DE1,  // [ 79] x=1.2392
    0x6CC169EB,  // [ 80] x=1.2549
    0x6D4E768A,  // [ 81] x=1.2706
    0x6DD7C8A4,  // [ 82] x=1.2863
    0x6E5D74F6,  // [ 83] x=1.3020
    0x6EDF900B,  // [ 84] x=1.3176
    0x6F5E2E36,  // [ 85] x=1.3333
    0x6FD96397,  // [ 86] x=1.3490
    0x7051440D,  // [ 87] x=1.3647
    0x70C5E33B,  // [ 88] x=1.3804
    0x71375485,  // [ 89] x=1.3961
    0x71A5AB08,  // [ 90] x=1.4118
    0x7210F99D,  // [ 91] x=1.4275
    0x727952D7,  // [ 92] x=1.4431
    0x72DEC8FE,  // [ 93] x=1.4588
    0x73416E11,  // [ 94] x=1.4745
    0x73A153C3,  // [ 95] x=1.4902
    0x73FE8B7A,  // [ 96] x=1.5059
    0x7459264F,  // [ 97] x=1.5216
    0x74B1350D,  // [ 98] x=1.5373
    0x7506C831,  // [ 99] x=1.5529
    0x7559EFE7,  // [100] x=1.5686
    0x75AABC0E,  // [101] x=1.5843
    0x75F93C35,  // [102] x=1.6000
    0x76457F9A,  // [103] x=1.6157
    0x768F952E,  // [104] x=1.6314
    0x76D78B93,  // [105] x=1.6471
    0x771D711C,  // [106] x=1.6627
    0x776153CD,  // [107] x=1.6784
    0x77A3415F,  // [108] x=1.6941
    0x77E3473A,  // [109] x=1.7098
    0x7821727E,  // [110] x=1.7255
    0x785DCFFD,  // [111] x=1.7412
    0x78986C3F,  // [112] x=1.7569
    0x78D15381,  // [113] x=1.7725
    0x790891B9,  // [114] x=1.7882
    0x793E3294,  // [115] x=1.8039
    0x79724177,  // [116] x=1.8196
    0x79A4C983,  // [117] x=1.8353
    0x79D5D592,  // [118] x=1.8510
    0x7A05703B,  // [119] x=1.8667
    0x7A33A3D2,  // [120] x=1.8824
    0x7A607A69,  // [121] x=1.8980
    0x7A8BFDD3,  // [122] x=1.9137
    0x7AB637A2,  // [123] x=1.9294
    0x7ADF3129,  // [124] x=1.9451
    0x7B06F381,  // [125] x=1.9608
    0x7B2D8784,  // [126] x=1.9765
    0x7B52F5D2,  // [127] x=1.9922
    0x7B7746D2,  // [128] x=2.0078
    0x7B9A82B3,  // [129] x=2.0235
    0x7BBCB16B,  // [130] x=2.0392
    0x7BDDDABC,  // [131] x=2.0549
    0x7BFE0631,  // [132] x=2.0706
    0x7C1D3B24,  // [133] x=2.0863
    0x7C3B80B9,  // [134] x=2.1020
    0x7C58DDE5,  // [135] x=2.1176
    0x7C75596C,  // [136] x=2.1333
    0x7C90F9E2,  // [137] x=2.1490
    0x7CABC5AE,  // [138] x=2.1647
    0x7CC5C308,  // [139] x=2.1804
    0x7CDEF800,  // [140] x=2.1961
    0x7CF76A76,  // [141] x=2.2118
    0x7D0F2023,  // [142] x=2.2275
    0x7D261E97,  // [143] x=2.2431
    0x7D3C6B3A,  // [144] x=2.2588
    0x7D520B4D,  // [145] x=2.2745
    0x7D6703E9,  // [146] x=2.2902
    0x7D7B5A07,  // [147] x=2.3059
    0x7D8F1276,  // [148] x=2.3216
    0x7DA231E7,  // [149] x=2.3373
    0x7DB4BCE6,  // [150] x=2.3529
    0x7DC6B7DE,  // [151] x=2.3686
    0x7DD8271A,  // [152] x=2.3843
    0x7DE90EC7,  // [153] x=2.4000
    0x7DF972F0,  // [154] x=2.4157
    0x7E095784,  // [155] x=2.4314
    0x7E18C055,  // [156] x=2.4471
    0x7E27B119,  // [157] x=2.4627
    0x7E362D68,  // [158] x=2.4784
    0x7E4438C2,  // [159] x=2.4941
    0x7E51D68C,  // [160] x=2.5098
    0x7E5F0A12,  // [161] x=2.5255
    0x7E6BD685,  // [162] x=2.5412
    0x7E783F01,  // [163] x=2.5569
    0x7E844689,  // [164] x=2.5725
    0x7E8FF00A,  // [165] x=2.5882
    0x7E9B3E5B,  // [166] x=2.6039
    0x7EA6343D,  // [167] x=2.6196
    0x7EB0D45C,  // [168] x=2.6353
    0x7EBB2150,  // [169] x=2.6510
    0x7EC51D9E,  // [170] x=2.6667
    0x7ECECBB8,  // [171] x=2.6824
    0x7ED82DFC,  // [172] x=2.6980
    0x7EE146B7,  // [173] x=2.7137
    0x7EEA1823,  // [174] x=2.7294
    0x7EF2A46C,  // [175] x=2.7451
    0x7EFAEDAB,  // [176] x=2.7608
    0x7F02F5EA,  // [177] x=2.7765
    0x7F0ABF22,  // [178] x=2.7922
    0x7F124B3E,  // [179] x=2.8078
    0x7F199C1B,  // [180] x=2.8235
    0x7F20B386,  // [181] x=2.8392
    0x7F279340,  // [182] x=2.8549
    0x7F2E3CFA,  // [183] x=2.8706
    0x7F34B25B,  // [184] x=2.8863
    0x7F3AF4FB,  // [185] x=2.9020
    0x7F410666,  // [186] x=2.9176
    0x7F46E81E,  // [187] x=2.9333
    0x7F4C9B96,  // [188] x=2.9490
    0x7F522239,  // [189] x=2.9647
    0x7F577D66,  // [190] x=2.9804
    0x7F5CAE6F,  // [191] x=2.9961
    0x7F61B6A1,  // [192] x=3.0118
    0x7F669739,  // [193] x=3.0275
    0x7F6B516E,  // [194] x=3.0431
    0x7F6FE66E,  // [195] x=3.0588
    0x7F74575B,  // [196] x=3.0745
    0x7F78A550,  // [197] x=3.0902
    0x7F7CD160,  // [198] x=3.1059
    0x7F80DC95,  // [199] x=3.1216
    0x7F84C7F0,  // [200] x=3.1373
    0x7F88946A,  // [201] x=3.1529
    0x7F8C42F8,  // [202] x=3.1686
    0x7F8FD483,  // [203] x=3.1843
    0x7F9349EF,  // [204] x=3.2000
    0x7F96A419,  // [205] x=3.2157
    0x7F99E3D7,  // [206] x=3.2314
    0x7F9D09F9,  // [207] x=3.2471
    0x7FA01749,  // [208] x=3.2627
    0x7FA30C88,  // [209] x=3.2784
    0x7FA5EA75,  // [210] x=3.2941
    0x7FA8B1C7,  // [211] x=3.3098
    0x7FAB632F,  // [212] x=3.3255
    0x7FADFF5A,  // [213] x=3.3412
    0x7FB086EF,  // [214] x=3.3569
    0x7FB2FA91,  // [215] x=3.3725
    0x7FB55ADB,  // [216] x=3.3882
    0x7FB7A866,  // [217] x=3.4039
    0x7FB9E3C6,  // [218] x=3.4196
    0x7FBC0D8A,  // [219] x=3.4353
    0x7FBE263D,  // [220] x=3.4510
    0x7FC02E64,  // [221] x=3.4667
    0x7FC22683,  // [222] x=3.4824
    0x7FC40F17,  // [223] x=3.4980
    0x7FC5E89B,  // [224] x=3.5137
    0x7FC7B386,  // [225] x=3.5294
    0x7FC9704A,  // [226] x=3.5451
    0x7FCB1F57,  // [227] x=3.5608
    0x7FCCC11A,  // [228] x=3.5765
    0x7FCE55FA,  // [229] x=3.5922
    0x7FCFDE5E,  // [230] x=3.6078
    0x7FD15AA8,  // [231] x=3.6235
    0x7FD2CB38,  // [232] x=3.6392
    0x7FD43069,  // [233] x=3.6549
    0x7FD58A96,  // [234] x=3.6706
    0x7FD6DA15,  // [235] x=3.6863
    0x7FD81F3B,  // [236] x=3.7020
    0x7FD95A59,  // [237] x=3.7176
    0x7FDA8BBE,  // [238] x=3.7333
    0x7FDBB3B8,  // [239] x=3.7490
    0x7FDCD28F,  // [240] x=3.7647
    0x7FDDE88E,  // [241] x=3.7804
    0x7FDEF5F8,  // [242] x=3.7961
    0x7FDFFB12,  // [243] x=3.8118
    0x7FE0F81E,  // [244] x=3.8275
    0x7FE1ED5B,  // [245] x=3.8431
    0x7FE2DB07,  // [246] x=3.8588
    0x7FE3C15D,  // [247] x=3.8745
    0x7FE4A098,  // [248] x=3.8902
    0x7FE578EF,  // [249] x=3.9059
    0x7FE64A99,  // [250] x=3.9216
    0x7FE715CA,  // [251] x=3.9373
    0x7FE7DAB6,  // [252] x=3.9529
    0x7FE8998E,  // [253] x=3.9686
    0x7FE95282,  // [254] x=3.9843
    0x7FEA05C1  // [255] x=4.0000
};



// bitshift adjust these parameters for driving
const float drive_levels[10] = {
    1.0f, 1.2f, 1.3f, 1.5f, 1.7f, 1.9f, 2.1f, 2.3f, 2.7f, 3.0f
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
    
    float scaled = abs_value * drive; //scaled signal
   

    int index = (int) scaled >> (23); //shift the bits 
    index = index & 0xFF; //only take the upper 8 bits
    signal = tanh_lut[index] >> 0; //indexed by upper 8 bits
    //sign extend
    signal *= sign;

    *L = signal;
    *R = signal;

    //tune distortion knee and values
}
