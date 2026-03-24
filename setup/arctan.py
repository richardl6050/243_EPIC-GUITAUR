import numpy as np

# Configuration
LUT_SIZE = 256          # 8-bit index from your MSB
BIT_DEPTH = 23          # Target signal depth
MAX_VAL = (2**23) - 1   # Max positive value for 24-bit signed

def generate_tanh_lut(size):
    # Create input range from -1.0 to 1.0
    x = np.linspace(0, 16, size)
    
    y = np.arctan(x)
    
    # Scale to 24-bit integer range
    lut = (y * MAX_VAL).astype(np.int32)

    
    return lut

# Generate
lut_values = generate_tanh_lut(LUT_SIZE)

# Format for C/C++ Header
print(f"const int32_t drive_lut[{LUT_SIZE}] = ")
for i, val in enumerate(lut_values):
    print(f"0x{val:x},")
print("\n};")