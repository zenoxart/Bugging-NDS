"""
Imports tools/titlescreen.png (hand-drawn) as the top-screen backdrop shown
during STATE_TITLE. Same idea as import_background.py/import_gameover.py:
downsample to the DS's native 256x192 with a quality filter, emit a raw
RGB15 binary the Makefile embeds via bin2s, plus a true-size preview.

Run with: python tools/import_titlescreen.py
"""

import os
import struct
from PIL import Image

OUT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(OUT_DIR)
SRC_PATH = os.path.join(OUT_DIR, "titlescreen.png")

W, H = 256, 192


def to_rgb15_bytes(img):
    w, h = img.size
    px = img.load()
    out = bytearray()
    for y in range(h):
        for x in range(w):
            r, g, b = px[x, y][:3]
            r5 = (r * 31 + 127) // 255
            g5 = (g * 31 + 127) // 255
            b5 = (b * 31 + 127) // 255
            val = 0x8000 | (b5 << 10) | (g5 << 5) | r5  # opaque
            out += struct.pack("<H", val)
    return bytes(out)


def main():
    src = Image.open(SRC_PATH).convert("RGB")
    print("loaded", SRC_PATH, src.size)

    resized = src.resize((W, H), Image.LANCZOS)

    data_dir = os.path.join(PROJECT, "data")
    os.makedirs(data_dir, exist_ok=True)
    bin_path = os.path.join(data_dir, "titlescreen.bg.bin")
    with open(bin_path, "wb") as f:
        f.write(to_rgb15_bytes(resized))
    print("wrote", bin_path, f"({W * H * 2} bytes)")

    preview_path = os.path.join(OUT_DIR, "titlescreen_ingame_preview.png")
    resized.resize((W * 2, H * 2), Image.NEAREST).save(preview_path)
    print("wrote", preview_path)


if __name__ == "__main__":
    main()
