"""
Generates Bug Swarm's sprite art.

Single source of truth: every sprite is drawn once, at its native in-game
pixel size, into an RGBA buffer. That buffer is used for BOTH:
  - a labeled spritesheet PNG (tools/spritesheet_preview.png) for humans, and
  - a C header (source/spritesheet_data.h) with RGB15+alpha pixel arrays
    that source/sprites.c loads directly into VRAM.

Run with: python tools/gen_sprites.py
(No devkitPro/grit dependency - plain Pillow.)
"""

from PIL import Image, ImageDraw, ImageFont
import os

OUT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(OUT_DIR)

# ---- palette (8-bit RGBA; converted to RGB15+alpha for the game) ----
TRANSPARENT = (0, 0, 0, 0)

GREEN_DARK  = (20, 90, 40, 255)
GREEN       = (64, 200, 96, 255)
GREEN_LIGHT = (170, 255, 195, 255)
CYAN        = (90, 235, 235, 255)
WHITE       = (255, 255, 255, 255)

RED_DARK  = (100, 18, 18, 255)
RED       = (216, 56, 56, 255)
RED_LIGHT = (255, 140, 140, 255)
BLACK     = (25, 15, 15, 255)

PURPLE_DARK  = (78, 20, 100, 255)
PURPLE       = (188, 70, 220, 255)
PURPLE_LIGHT = (230, 165, 245, 255)

GOLD       = (230, 190, 60, 255)
GOLD_LIGHT = (255, 230, 140, 255)

ORANGE_DARK = (120, 60, 10, 255)
ORANGE      = (222, 132, 40, 255)
ORANGE_SPOT = (255, 200, 90, 255)
TAN         = (230, 200, 150, 255)
TAN_DARK    = (180, 150, 100, 255)

YELLOW       = (255, 225, 60, 255)
YELLOW_LIGHT = (255, 255, 220, 255)


def new_sprite(w, h):
    return Image.new("RGBA", (w, h), TRANSPARENT)


def draw_player(size=16):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    # turret body: dark outline triangle, lighter fill inset
    d.polygon([(7, 1), (0, 15), (15, 15)], fill=GREEN_DARK)
    d.polygon([(7, 3), (2, 14), (12, 14)], fill=GREEN)
    d.polygon([(7, 5), (4, 13), (10, 13)], fill=GREEN_LIGHT)
    # base fins
    d.rectangle([0, 13, 2, 15], fill=GREEN_DARK)
    d.rectangle([13, 13, 15, 15], fill=GREEN_DARK)
    # muzzle
    d.rectangle([6, 0, 8, 3], fill=CYAN)
    d.point([(7, 0)], fill=WHITE)
    return img


def draw_bullet(size=8):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    d.rectangle([2, 0, 5, 7], fill=YELLOW)
    d.rectangle([3, 0, 4, 6], fill=YELLOW_LIGHT)
    d.point([(3, 0), (4, 0)], fill=WHITE)
    return img


def _bug_legs(d, cx, cy, r, color):
    for dx in (-r, r):
        d.line([(cx + dx * 0.6, cy - 1), (cx + dx, cy + 2)], fill=color)
        d.line([(cx + dx * 0.6, cy + 3), (cx + dx, cy + 6)], fill=color)


def draw_crawler(size=16):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    cx, cy, r = 7.5, 8.5, 6
    _bug_legs(d, cx, cy, r, BLACK)
    d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=RED_DARK)
    d.ellipse([cx - r + 2, cy - r + 2, cx + r - 1, cy + r - 1], fill=RED)
    # shell segments
    d.line([(cx - 4, cy - 1), (cx + 4, cy - 1)], fill=RED_DARK)
    d.line([(cx - 4, cy + 2), (cx + 4, cy + 2)], fill=RED_DARK)
    # eyes
    d.ellipse([cx - 4, cy - 4, cx - 2, cy - 2], fill=WHITE)
    d.ellipse([cx + 2, cy - 4, cx + 4, cy - 2], fill=WHITE)
    d.point([(cx - 3, cy - 3), (cx + 3, cy - 3)], fill=BLACK)
    # antennae
    d.line([(cx - 2, cy - r), (cx - 4, cy - r - 2)], fill=BLACK)
    d.line([(cx + 2, cy - r), (cx + 4, cy - r - 2)], fill=BLACK)
    return img


def draw_splitter(size=16):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    cx, cy, r = 7.5, 8.5, 6
    _bug_legs(d, cx, cy, r, BLACK)
    d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=PURPLE_DARK)
    d.ellipse([cx - r + 2, cy - r + 2, cx + r - 1, cy + r - 1], fill=PURPLE)
    d.ellipse([cx - 4, cy - 4, cx - 2, cy - 2], fill=WHITE)
    d.ellipse([cx + 2, cy - 4, cx + 4, cy - 2], fill=WHITE)
    d.point([(cx - 3, cy - 3), (cx + 3, cy - 3)], fill=BLACK)
    # split seam down the middle - cut a transparent gap through the body
    d.line([(cx, cy - r + 2), (cx, cy + r - 2)], fill=TRANSPARENT, width=2)
    d.line([(cx - 1, cy - r + 3), (cx - 1, cy + r - 3)], fill=PURPLE_LIGHT)
    d.line([(cx + 1, cy - r + 3), (cx + 1, cy + r - 3)], fill=PURPLE_LIGHT)
    return img


def draw_mini(size=8):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    cx, cy, r = 3.5, 4, 3
    d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=PURPLE_DARK)
    d.ellipse([cx - r + 1, cy - r + 1, cx + r - 1, cy + r - 1], fill=PURPLE_LIGHT)
    d.point([(cx - 1, cy - 1), (cx + 1, cy - 1)], fill=BLACK)
    return img


def draw_queen(size=32):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    cx, cy, r = 16, 18, 13
    _bug_legs(d, cx, cy + 4, r, BLACK)
    _bug_legs(d, cx, cy - 2, r + 2, BLACK)
    d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=RED_DARK)
    d.ellipse([cx - r + 3, cy - r + 3, cx + r - 2, cy + r - 2], fill=RED)
    d.ellipse([cx - r + 6, cy - r + 6, cx + r - 5, cy + r - 5], fill=RED_LIGHT)
    # shell segments
    for yy in (cy - 5, cy - 1, cy + 3, cy + 7):
        d.line([(cx - 8, yy), (cx + 8, yy)], fill=RED_DARK)
    # crown
    d.polygon([(cx - 11, cy - r + 2), (cx - 7, cy - r - 7), (cx - 4, cy - r + 2)], fill=GOLD)
    d.polygon([(cx - 3, cy - r + 1), (cx, cy - r - 10), (cx + 3, cy - r + 1)], fill=GOLD)
    d.polygon([(cx + 4, cy - r + 2), (cx + 7, cy - r - 7), (cx + 11, cy - r + 2)], fill=GOLD)
    d.point([(cx - 7, cy - r - 6), (cx, cy - r - 9), (cx + 7, cy - r - 6)], fill=GOLD_LIGHT)
    # eyes
    d.ellipse([cx - 7, cy - 3, cx - 2, cy + 2], fill=YELLOW)
    d.ellipse([cx + 2, cy - 3, cx + 7, cy + 2], fill=YELLOW)
    d.ellipse([cx - 6, cy - 2, cx - 3, cy + 1], fill=BLACK)
    d.ellipse([cx + 3, cy - 2, cx + 6, cy + 1], fill=BLACK)
    return img


def draw_obstacle(size=16):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    # cap
    d.ellipse([0, 0, 15, 12], fill=ORANGE_DARK)
    d.ellipse([1, 1, 14, 11], fill=ORANGE)
    d.ellipse([3, 5, 5, 7], fill=ORANGE_SPOT)
    d.ellipse([9, 3, 11, 5], fill=ORANGE_SPOT)
    d.ellipse([7, 7, 9, 9], fill=ORANGE_SPOT)
    # stem
    d.rectangle([5, 9, 10, 15], fill=TAN_DARK)
    d.rectangle([6, 9, 9, 14], fill=TAN)
    return img


def draw_powerup(size=8):
    img = new_sprite(size, size)
    d = ImageDraw.Draw(img)
    d.polygon([(4, 0), (7, 4), (4, 7), (1, 4)], fill=CYAN)
    d.polygon([(4, 1), (6, 4), (4, 6), (2, 4)], fill=WHITE)
    d.point([(4, 2)], fill=CYAN)
    return img


SPRITES = [
    ("PLAYER",   draw_player(16),   16),
    ("BULLET",   draw_bullet(8),     8),
    ("CRAWLER",  draw_crawler(16),  16),
    ("SPLITTER", draw_splitter(16), 16),
    ("MINI",     draw_mini(8),       8),
    ("QUEEN",    draw_queen(32),    32),
    ("OBSTACLE", draw_obstacle(16), 16),
    ("POWERUP",  draw_powerup(8),    8),
]


def to_rgb15(img):
    """Row-major (y*w+x) RGB15+alpha values, matching the DS's 1D bitmap OBJ layout."""
    w, h = img.size
    px = img.load()
    out = []
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            r5 = (r * 31 + 127) // 255
            g5 = (g * 31 + 127) // 255
            b5 = (b * 31 + 127) // 255
            alpha_bit = 0x8000 if a > 127 else 0
            out.append(alpha_bit | (b5 << 10) | (g5 << 5) | r5)
    return out


def write_header():
    path = os.path.join(PROJECT, "source", "spritesheet_data.h")
    with open(path, "w") as f:
        f.write("// Generated by tools/gen_sprites.py - do not edit by hand.\n")
        f.write("#ifndef BUGSWARM_SPRITESHEET_DATA_H\n#define BUGSWARM_SPRITESHEET_DATA_H\n\n")
        f.write('#include "globals.h"\n\n')
        for name, img, size in SPRITES:
            data = to_rgb15(img)
            f.write(f"static const u16 SPR_{name}[{len(data)}] = {{\n")
            for i in range(0, len(data), 12):
                row = ", ".join(f"0x{v:04X}" for v in data[i:i + 12])
                f.write(f"    {row},\n")
            f.write("};\n\n")
        f.write("#endif\n")
    print("wrote", path)


def write_preview():
    scale = 10
    pad = 2
    label_h = 12
    cell = max(size for _, _, size in SPRITES)
    cols = 4
    rows = (len(SPRITES) + cols - 1) // cols
    cell_px = cell * scale
    sheet_w = cols * (cell_px + pad * 2)
    sheet_h = rows * (cell_px + pad * 2 + label_h)

    sheet = Image.new("RGBA", (sheet_w, sheet_h), (24, 24, 32, 255))
    d = ImageDraw.Draw(sheet)
    try:
        font = ImageFont.load_default()
    except Exception:
        font = None

    for idx, (name, img, size) in enumerate(SPRITES):
        col = idx % cols
        row = idx // cols
        ox = col * (cell_px + pad * 2) + pad
        oy = row * (cell_px + pad * 2 + label_h) + pad

        # checkerboard so transparency is visible
        check = Image.new("RGBA", (cell_px, cell_px), (40, 40, 52, 255))
        cd = ImageDraw.Draw(check)
        step = scale * 2
        for yy in range(0, cell_px, step):
            for xx in range(0, cell_px, step):
                if ((xx // step) + (yy // step)) % 2 == 0:
                    cd.rectangle([xx, yy, xx + step - 1, yy + step - 1], fill=(52, 52, 66, 255))
        sheet.alpha_composite(check, (ox, oy))

        big = img.resize((size * scale, size * scale), Image.NEAREST)
        inset = (cell_px - size * scale) // 2
        sheet.alpha_composite(big, (ox + inset, oy + inset))
        d.rectangle([ox, oy, ox + cell_px - 1, oy + cell_px - 1], outline=(90, 90, 110, 255))

        label = f"{name} {size}x{size}"
        tw = d.textlength(label, font=font) if font else len(label) * 6
        d.text((ox + (cell_px - tw) / 2, oy + cell_px + 1), label, fill=(230, 230, 240, 255), font=font)

    out_path = os.path.join(OUT_DIR, "spritesheet_preview.png")
    sheet.convert("RGB").save(out_path)
    print("wrote", out_path)


if __name__ == "__main__":
    write_header()
    write_preview()
