import os
import struct

# Framebuffer device path
framebuffer_path = "/dev/fb1"

# Set your framebuffer dimensions (should match your driver's settings)
width = 1024
height = 768
bpp = 4  # 32 bits per pixel = 4 bytes

# Color to fill the screen (RGBA - Red, Green, Blue, Alpha)
color = (0, 0, 255, 255)  # Solid blue

# Convert color to binary format
color_bin = struct.pack('BBBB', *color)

# Open the framebuffer device
with open(framebuffer_path, 'wb') as fb:
    for _ in range(width * height):
        fb.write(color_bin)

print("Framebuffer filled with color!")

