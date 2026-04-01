import numpy as np

sine_lut = np.round(np.sin(np.linspace(0,2*np.pi, 256, endpoint=False))* 0x7FFF).astype(np.int32)

print("const int16_t sine_lut[256] = {")
for i, val in enumerate(sine_lut):
    comma = "," if i < 255 else ""
    print(f"    {val}, // [{i:3d}] {i/256*360:.1f}deg")
print("};")