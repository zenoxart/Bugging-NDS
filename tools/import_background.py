"""
Imports a hand-reworked tools/background_preview.png back into the game.

Unlike gen_background.py (which procedurally paints the preview), this
reads whatever art is currently in background_preview.png, downsamples it
to the DS top screen's native 256x192 with a quality filter, and writes:

  - data/forest_floor.bg.bin        raw RGB15 pixels, embedded via bin2s
    (same contract as gen_background.py - source/background.c is unchanged)
  - tools/background_ingame_preview.png   the downsampled 256x192 result,
    upscaled 2x with nearest-neighbor so it's inspectable at the size that
    actually ships, since a big detailed source image can turn to mush once
    it's squeezed onto a 256x192 screen.

Run with: python tools/import_background.py
"""

import os
import struct
from PIL import Image

OUT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(OUT_DIR)
SRC_PATH = os.path.join(OUT_DIR, "background_preview.png")

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
            val = 0x8000 | (b5 << 10) | (g5 << 5) | r5  # opaque - BG3 has no transparency here
            out += struct.pack("<H", val)
    return bytes(out)


def main():
    src = Image.open(SRC_PATH).convert("RGB")
    print("loaded", SRC_PATH, src.size)

    resized = src.resize((W, H), Image.LANCZOS)

    data_dir = os.path.join(PROJECT, "data")
    os.makedirs(data_dir, exist_ok=True)
    bin_path = os.path.join(data_dir, "forest_floor.bg.bin")
    with open(bin_path, "wb") as f:
        f.write(to_rgb15_bytes(resized))
    print("wrote", bin_path, f"({W * H * 2} bytes)")

    preview_path = os.path.join(OUT_DIR, "background_ingame_preview.png")
    resized.resize((W * 2, H * 2), Image.NEAREST).save(preview_path)
    print("wrote", preview_path)


if __name__ == "__main__":
    main()
