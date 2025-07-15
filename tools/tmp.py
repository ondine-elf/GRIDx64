#!/usr/bin/env python3

import sys

def psf_to_c_header(psf_file, output_file, array_name="font8x16"):

    with open(psf_file, "rb") as f:
        data = f.read()

    # PSF version 1 header:
    # 2 bytes: magic (0x36 0x04)
    # 1 byte: mode (bits)
    # 1 byte: char size (bytes per glyph)
    if data[0] != 0x36 or data[1] != 0x04:
        print("Not a PSF v1 font or unsupported format")
        return

    mode = data[2]
    charsize = data[3]

    print(f"PSF v1 detected. Mode: {mode}, Char size: {charsize}")

    # Number of glyphs depends on mode bit 0x01 (512 or 256)
    num_glyphs = 512 if (mode & 0x01) else 256

    print(f"Number of glyphs: {num_glyphs}")

    glyphs = data[4:4 + num_glyphs * charsize]

    with open(output_file, "w") as out:
        out.write(f"// Generated from {psf_file}\n")
        out.write(f"// PSF v1 font, {num_glyphs} glyphs, {charsize} bytes per glyph\n\n")
        out.write(f"const unsigned char {array_name}[{num_glyphs}][{charsize}] = {{\n")

        for i in range(num_glyphs):
            out.write("    { ")
            glyph_bytes = glyphs[i*charsize:(i+1)*charsize]
            out.write(", ".join(f"0x{b:02X}" for b in glyph_bytes))
            out.write(" }")
            if i != num_glyphs - 1:
                out.write(",\n")
            else:
                out.write("\n")
        out.write("};\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: psf_to_c_header.py input.psf output.h")
        sys.exit(1)

    psf_to_c_header(sys.argv[1], sys.argv[2])

