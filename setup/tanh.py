import numpy as np

# Configuration
LUT_SIZE = 256          # 8-bit index from upper bits of magnitude
FRAC_BITS = 23         # Remaining bits used for interpolation (bits 14..0)
MAG_BITS = 31           # Magnitude bits (24-bit signed, sign excluded)
INDEX_BITS = 8          # MSBs used for LUT index
MAX_VAL = (1 << MAG_BITS) - 1   # 0x7FFFFF

# The 8 MSBs of the 23-bit magnitude span 0..255.
# Each index step = 2^15 in the original 23-bit fixed-point.
# So index N represents normalised input N/255.
# You need to decide: what input value does index 255 correspond to?
# Here we map index 255 -> x=4.0 (tanh(4) ≈ 0.9993, effectively clipped).
# Adjust DRIVE_MAX to taste.
DRIVE_MAX = 4


def generate_tanh_lut(size, drive_max, use_atan=False):
    # Index 0..255 maps to input magnitude 0..drive_max
    x = np.linspace(0.0, drive_max, size)

    y = np.tanh(x)

    # y is in [0, 1), scale to 23-bit signed magnitude
    lut = np.round(y * MAX_VAL).astype(np.int32)

    return lut

lut_values = generate_tanh_lut(LUT_SIZE, DRIVE_MAX, use_atan=False)

# Sanity check
print(f"LUT[0]   = {lut_values[0]} (expect 0)")
print(f"LUT[255] = {lut_values[255]} (expect close to {MAX_VAL})")
print()

# C/C++ header
print(f"// tanh LUT: 256 entries, input magnitude 0..{DRIVE_MAX} -> 0..0x{MAX_VAL:X}")
print(f"// Index = bits[22:15] of 24-bit signed magnitude")
print(f"// Sign is applied separately; interpolate with bits[14:0] for smoothness")
print(f"const int32_t tanh_lut[{LUT_SIZE}] = {{")
for i, val in enumerate(lut_values):
    comma = "," if i < LUT_SIZE - 1 else ""
    print(f"    0x{val:07X}{comma}  // [{i:3d}] x={i/255*DRIVE_MAX:.4f}")
print("};")