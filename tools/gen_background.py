"""
Generates Bug Swarm's top-screen background: an autumn forest floor.

Single source of truth, same idea as gen_sprites.py: one RGBA pixel buffer
feeds both a PNG preview (tools/background_preview.png) and the raw RGB15
binary the DS loads as a 16-bit bitmap background (data/forest_floor.bg.bin,
picked up automatically by the Makefile's DATA dir and embedded via bin2s).

Kept deliberately muted/low-contrast - the DS screen is small, and the
gameplay sprites (bright red/purple bugs, green cannon, yellow bullets) need
to read clearly on top of it.

Run with: python tools/gen_background.py
"""

import os
import random
import struct
from PIL import Image, ImageDraw, ImageFilter

W, H = 256, 192
OUT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(OUT_DIR)

random.seed(42)

# ---- muted autumn palette (kept darker/desaturated so foreground sprites pop) ----
SOIL_LO = (46, 34, 24)
SOIL_HI = (78, 56, 34)

LEAF_COLORS = [
    (120, 45, 24),   # dark red
    (150, 82, 22),   # burnt orange
    (146, 110, 34),  # mustard
    (96, 60, 28),    # brown
    (104, 88, 30),   # olive
    (168, 96, 30),   # bright orange (sparingly)
]

TWIG = (52, 38, 24)


def make_soil():
    img = Image.new("RGB", (W, H))
    px = img.load()
    for y in range(H):
        # soft vertical gradient (slightly darker toward the top)
        t = y / (H - 1)
        base = tuple(int(SOIL_LO[i] + (SOIL_HI[i] - SOIL_LO[i]) * t) for i in range(3))
        for x in range(W):
            n = random.randint(-10, 10)
            px[x, y] = tuple(max(0, min(255, base[c] + n)) for c in range(3))
    return img


def draw_leaf(d, cx, cy, size, color):
    # a small rotated diamond reads as a leaf at this pixel scale
    pts = [(cx, cy - size), (cx + size, cy), (cx, cy + size), (cx - size, cy)]
    d.polygon(pts, fill=color)
    d.point([(cx, cy)], fill=tuple(max(0, c - 25) for c in color))


def draw_twig(d, x, y, length, angle_deg):
    import math
    a = math.radians(angle_deg)
    x2 = x + length * math.cos(a)
    y2 = y + length * math.sin(a)
    d.line([(x, y), (x2, y2)], fill=TWIG, width=1)


def make_background():
    img = make_soil()
    d = ImageDraw.Draw(img)

    for _ in range(9):
        draw_twig(
            d,
            random.uniform(0, W),
            random.uniform(0, H),
            random.uniform(10, 26),
            random.uniform(0, 360),
        )

    # scattered leaves, slightly denser toward the edges so the play-lane
    # down the middle stays a touch calmer for reading bullets/bugs
    for _ in range(340):
        x = random.uniform(0, W)
        y = random.uniform(0, H)
        edge_bias = abs(x - W / 2) / (W / 2)
        if random.random() > 0.35 + 0.5 * edge_bias:
            continue
        size = random.uniform(1.6, 3.4)
        color = random.choice(LEAF_COLORS)
        draw_leaf(d, x, y, size, color)

    img = img.filter(ImageFilter.SMOOTH_MORE)
    # re-punch a few crisp leaves on top after smoothing so it doesn't go mushy
    d = ImageDraw.Draw(img)
    for _ in range(120):
        x = random.uniform(0, W)
        y = random.uniform(0, H)
        size = random.uniform(1.4, 3.0)
        color = random.choice(LEAF_COLORS)
        draw_leaf(d, x, y, size, color)

    return img


def to_rgb15_bytes(img):
    w, h = img.size
    px = img.load()
    out = bytearray()
    for y in range(h):
        for x in range(w):
            r, g, b = px[x, y]
            r5 = (r * 31 + 127) // 255
            g5 = (g * 31 + 127) // 255
            b5 = (b * 31 + 127) // 255
            val = 0x8000 | (b5 << 10) | (g5 << 5) | r5  # alpha bit always set: fully opaque
            out += struct.pack("<H", val)
    return bytes(out)


def main():
    img = make_background()

    data_dir = os.path.join(PROJECT, "data")
    os.makedirs(data_dir, exist_ok=True)
    bin_path = os.path.join(data_dir, "forest_floor.bg.bin")
    with open(bin_path, "wb") as f:
        f.write(to_rgb15_bytes(img))
    print("wrote", bin_path, f"({W * H * 2} bytes)")

    preview_path = os.path.join(OUT_DIR, "background_preview.png")
    img.resize((W * 2, H * 2), Image.NEAREST).save(preview_path)
    print("wrote", preview_path)


if __name__ == "__main__":
    main()
