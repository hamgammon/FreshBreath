#!/usr/bin/env python3
"""
Generate high-resolution and watch launcher icons for FreshBreath.
"""

import math
import os
from PIL import Image, ImageDraw

def render_freshbreath_icon(size: int) -> Image.Image:
    scale = 4.0
    s = int(size * scale)
    img = Image.new("RGBA", (s, s), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Background squircle / rounded rect
    margin = s * 0.04
    radius = s * 0.22
    rect = [margin, margin, s - margin, s - margin]

    # Draw gradient-like background using concentric rounded rects
    steps = 15
    for i in range(steps):
        t = i / float(steps)
        r = int(6 + t * (2 - 6))
        g = int(28 + t * (58 - 28))
        b = int(36 + t * (68 - 36))
        inset = margin + (s * 0.4 - margin) * (i / float(steps)) * 0.2
        draw.rounded_rectangle([inset, inset, s - inset, s - inset], radius=radius * (1 - t * 0.3), fill=(r, g, b, 255))

    cx = s / 2.0
    cy = s / 2.0

    # Draw concentric breathing wave rings
    ring_radii = [s * 0.40, s * 0.33, s * 0.26]
    ring_alphas = [40, 70, 110]
    for r_val, alpha in zip(ring_radii, ring_alphas):
        draw.ellipse([cx - r_val, cy - r_val, cx + r_val, cy + r_val],
                     outline=(0, 229, 255, alpha), width=max(1, int(s * 0.02)))

    # Draw central stylized dual leaf / lungs motif
    leaf_color = (0, 229, 255, 255)
    leaf_color_light = (128, 245, 255, 255)

    # Left leaf
    left_points = []
    steps_leaf = 30
    for i in range(steps_leaf + 1):
        th = -math.pi / 2.0 + (i / float(steps_leaf)) * math.pi
        lx = cx - s * 0.03 + (math.cos(th) - 1.0) * s * 0.16
        ly = cy + math.sin(th) * s * 0.24
        left_points.append((lx, ly))
    draw.polygon(left_points, fill=leaf_color)

    # Right leaf
    right_points = []
    for i in range(steps_leaf + 1):
        th = -math.pi / 2.0 + (i / float(steps_leaf)) * math.pi
        rx = cx + s * 0.03 + (1.0 - math.cos(th)) * s * 0.16
        ry = cy + math.sin(th) * s * 0.24
        right_points.append((rx, ry))
    draw.polygon(right_points, fill=leaf_color_light)

    # Central stem / breath stream
    draw.line([(cx, cy - s * 0.25), (cx, cy + s * 0.26)], fill=(255, 255, 255, 240), width=max(2, int(s * 0.025)))

    # Downscale with high quality Lanczos resampling
    final_img = img.resize((size, size), Image.Resampling.LANCZOS)
    return final_img

def main():
    os.makedirs("Resources", exist_ok=True)
    icon_512 = render_freshbreath_icon(512)
    icon_512.save("Resources/icon_512x512.png", "PNG")

    icon_60 = render_freshbreath_icon(60)
    icon_60.save("Resources/icon_60x60.png", "PNG")

    icon_30 = render_freshbreath_icon(30)
    icon_30.save("Resources/icon_30x30.png", "PNG")
    print("Icons generated: icon_512x512.png, icon_60x60.png, icon_30x30.png")

if __name__ == "__main__":
    main()
