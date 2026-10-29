#!/usr/bin/env python3
"""
Generate pixel-accurate watch UI screens and composite photorealistic device mockups
for FreshBreath using the official UNA Watch Penpot templates and Poppins fonts.
"""

import math
import os
from PIL import Image, ImageDraw, ImageFont

UNA_SDK = os.environ.get("UNA_SDK", os.path.expanduser("~/repos/una-sdk"))
FONT_DIR = os.path.join(UNA_SDK, "Examples/Apps/Workout/Software/Apps/TouchGFX-GUI/assets/fonts")
MOCKUP_DIR = "assets/mockups"
SCREENSHOT_DIR = "assets/screenshots"
PREVIEW_DIR = "assets/previews"

# Typography helpers
def get_font(name: str, size: int):
    path = os.path.join(FONT_DIR, f"{name}.ttf")
    if os.path.exists(path):
        return ImageFont.truetype(path, size)
    return ImageFont.load_default()

# Colors (Hex codes aligned with SDK::GUI::Color)
COLOR_BLACK       = (0, 0, 0, 255)
COLOR_WHITE       = (255, 255, 255, 255)
COLOR_GRAY        = (142, 142, 147, 255)
COLOR_GRAY_DARK   = (44, 44, 46, 255)
COLOR_CYAN_PURE   = (0, 229, 255, 255)
COLOR_AQUA        = (0, 176, 255, 255)
COLOR_SKY         = (79, 195, 247, 255)
COLOR_TEAL_DARK   = (0, 77, 64, 255)
COLOR_GREEN       = (64, 192, 0, 255)
COLOR_AMBER       = (192, 128, 0, 255)
COLOR_YELLOW      = (255, 214, 0, 255)
COLOR_RED         = (255, 59, 48, 255)

# Hardware button angles (TouchGFX: 0 = 12 o'clock, clockwise)
# PIL: 0 = 3 o'clock, clockwise -> angle_pil = angle_tg - 90
BUTTON_ARCS = {
    'L1': (291 - 90, 308 - 90), # 201 to 218
    'L2': (232 - 90, 249 - 90), # 142 to 159
    'R1': (52 - 90, 69 - 90),   # -38 to -21 (322 to 339)
    'R2': (112 - 90, 128 - 90), # 22 to 38
}

def draw_hardware_buttons(draw: ImageDraw.ImageDraw, scale: float, buttons: dict):
    cx, cy = 120 * scale, 120 * scale
    radius = 113 * scale
    width = int(round(6 * scale))
    bbox = [cx - radius, cy - radius, cx + radius, cy + radius]
    for key, color in buttons.items():
        if color and key in BUTTON_ARCS:
            start, end = BUTTON_ARCS[key]
            draw.arc(bbox, start=start, end=end, fill=color, width=width)

def draw_heart_icon(draw: ImageDraw.ImageDraw, cx: float, cy: float, size: float, color):
    # Vector heart shape
    points = []
    steps = 40
    for i in range(steps):
        t = i * 2.0 * math.pi / steps
        x = 16 * (math.sin(t) ** 3)
        y = -(13 * math.cos(t) - 5 * math.cos(2*t) - 2 * math.cos(3*t) - math.cos(4*t))
        # scale down
        s = size / 36.0
        points.append((cx + x * s, cy + y * s))
    draw.polygon(points, fill=color)

def draw_checkmark(draw: ImageDraw.ImageDraw, cx: float, cy: float, size: float, color):
    # Vector checkmark
    w = max(2, int(size * 0.15))
    p1 = (cx - size * 0.45, cy)
    p2 = (cx - size * 0.1, cy + size * 0.35)
    p3 = (cx + size * 0.45, cy - size * 0.35)
    draw.line([p1, p2, p3], fill=color, width=w, joint='curve')

def render_screen(screen_type: str, scale: float = 4.0) -> Image.Image:
    """
    Renders 240x240 screen at the given scale for antialiasing.
    """
    dim = int(240 * scale)
    img = Image.new("RGBA", (dim, dim), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Base black circular display
    draw.ellipse([0, 0, dim - 1, dim - 1], fill=(0, 0, 0, 255))

    # Helper fonts
    f_reg_14 = get_font("Poppins-Regular", int(14 * scale))
    f_reg_16 = get_font("Poppins-Regular", int(16 * scale))
    f_semi_20 = get_font("Poppins-SemiBold", int(20 * scale))
    f_semi_25 = get_font("Poppins-SemiBold", int(25 * scale))
    f_semi_35 = get_font("Poppins-SemiBold", int(35 * scale))
    f_semi_40 = get_font("Poppins-SemiBold", int(40 * scale))
    f_italic_18 = get_font("Poppins-Italic", int(18 * scale))

    cx = 120 * scale
    cy = 120 * scale

    def draw_text_center(text: str, y: float, font, color):
        bbox = draw.textbbox((0, 0), text, font=font)
        w = bbox[2] - bbox[0]
        draw.text((cx - w / 2, y * scale), text, font=font, fill=color)

    def draw_rounded_card(x: float, y: float, w: float, h: float, radius: float, bg_color):
        rect = [x * scale, y * scale, (x + w) * scale, (y + h) * scale]
        draw.rounded_rectangle(rect, radius=radius * scale, fill=bg_color)

    if screen_type == "main":
        draw_hardware_buttons(draw, scale, {
            'L1': COLOR_WHITE,
            'L2': COLOR_AMBER,
            'R1': COLOR_GREEN,
            'R2': COLOR_AMBER
        })
        draw_text_center("FRESH BREATH", 15, f_semi_20, COLOR_CYAN_PURE)
        draw_text_center("10:08", 38, f_reg_14, COLOR_GRAY)

        # Pattern Card
        draw_rounded_card(20, 62, 200, 72, 8, (0, 77, 64, 128))
        draw_text_center("PATTERN [L2]", 68, f_reg_14, COLOR_AQUA)
        draw_text_center("Box 4-4-4-4", 86, f_semi_20, COLOR_WHITE)
        draw_text_center("4s : 4s : 4s : 4s", 110, f_reg_14, COLOR_GRAY)

        # Duration Card
        draw_rounded_card(20, 140, 200, 46, 8, (44, 44, 46, 120))
        draw_text_center("DURATION [R2]", 144, f_reg_14, COLOR_AMBER)
        draw_text_center("5 min", 162, f_semi_20, COLOR_WHITE)

        # Start Prompt
        draw_text_center("Press R1 to Start", 198, f_semi_20, COLOR_GREEN)

    elif screen_type == "pattern_select":
        draw_hardware_buttons(draw, scale, {
            'L1': COLOR_WHITE,
            'L2': COLOR_WHITE,
            'R1': COLOR_GREEN,
            'R2': COLOR_RED
        })
        draw_text_center("SELECT PATTERN", 15, f_reg_14, COLOR_GRAY)

        # Pattern Details Card
        draw_rounded_card(15, 38, 210, 150, 10, (0, 77, 64, 100))
        draw_text_center("Box 4-4-4-4", 48, f_semi_25, COLOR_CYAN_PURE)
        draw_text_center("4 : 4 : 4 : 4", 80, f_semi_20, COLOR_WHITE)
        draw_text_center("Inhale · Hold · Exhale · Hold", 110, f_reg_14, COLOR_AQUA)
        draw_text_center("Focus & Acute Stress Reset", 146, f_italic_18, COLOR_YELLOW)

        # Navigation hint
        draw_text_center("L1/L2: Change • R1: Select", 198, f_reg_14, COLOR_GRAY)

    elif screen_type.startswith("breath_"):
        draw_hardware_buttons(draw, scale, {
            'L1': COLOR_RED,
            'R1': COLOR_AMBER,
            'R2': COLOR_GREEN
        })
        # Background arc ring
        arc_r = 95 * scale
        arc_bbox = [cx - arc_r, cy - arc_r, cx + arc_r, cy + arc_r]
        draw.arc(arc_bbox, start=0, end=360, fill=(0, 56, 64, 255), width=int(8 * scale))

        if screen_type == "breath_inhale":
            # Active Cyan arc (0 to 90 degrees, angle relative to 12 o'clock: -90 to 0 in PIL)
            draw.arc(arc_bbox, start=-90, end=0, fill=COLOR_CYAN_PURE, width=int(10 * scale))
            # Header
            draw_heart_icon(draw, 36 * scale, 42 * scale, 20 * scale, COLOR_RED)
            draw.text((54 * scale, 33 * scale), "68 bpm", font=f_reg_16, fill=COLOR_WHITE)
            bbox = draw.textbbox((0, 0), "Cycle 1", font=f_reg_16)
            draw.text(((216 * scale) - (bbox[2] - bbox[0]), 33 * scale), "Cycle 1", font=f_reg_16, fill=COLOR_AQUA)

            draw_text_center("INHALE", 78, f_semi_35, COLOR_CYAN_PURE)
            draw_text_center("4", 122, f_semi_40, COLOR_WHITE)
            draw_text_center("00:01", 185, f_reg_16, COLOR_GRAY)

        elif screen_type == "breath_hold":
            # Full glowing yellow/amber arc
            draw.arc(arc_bbox, start=0, end=360, fill=COLOR_YELLOW, width=int(10 * scale))
            # Header
            draw_heart_icon(draw, 36 * scale, 42 * scale, 20 * scale, COLOR_RED)
            draw.text((54 * scale, 33 * scale), "64 bpm", font=f_reg_16, fill=COLOR_WHITE)
            bbox = draw.textbbox((0, 0), "Cycle 3", font=f_reg_16)
            draw.text(((216 * scale) - (bbox[2] - bbox[0]), 33 * scale), "Cycle 3", font=f_reg_16, fill=COLOR_AQUA)

            draw_text_center("HOLD", 78, f_semi_35, COLOR_YELLOW)
            draw_text_center("3", 122, f_semi_40, COLOR_WHITE)
            draw_text_center("00:39", 185, f_reg_16, COLOR_GRAY)

        elif screen_type == "breath_exhale":
            # Contracting Sky Blue arc (-90 to 90 degrees)
            draw.arc(arc_bbox, start=-90, end=90, fill=COLOR_SKY, width=int(10 * scale))
            # Header
            draw_heart_icon(draw, 36 * scale, 42 * scale, 20 * scale, COLOR_RED)
            draw.text((54 * scale, 33 * scale), "61 bpm", font=f_reg_16, fill=COLOR_WHITE)
            bbox = draw.textbbox((0, 0), "Cycle 7", font=f_reg_16)
            draw.text(((216 * scale) - (bbox[2] - bbox[0]), 33 * scale), "Cycle 7", font=f_reg_16, fill=COLOR_AQUA)

            draw_text_center("EXHALE", 78, f_semi_35, COLOR_SKY)
            draw_text_center("2", 122, f_semi_40, COLOR_WHITE)
            draw_text_center("01:42", 185, f_reg_16, COLOR_GRAY)

    elif screen_type == "summary":
        draw_hardware_buttons(draw, scale, {
            'R1': COLOR_GREEN
        })
        draw_checkmark(draw, cx, 20 * scale, 22 * scale, COLOR_GREEN)
        draw_text_center("WORKOUT SAVED", 32, f_reg_14, COLOR_AQUA)
        draw_text_center("Breathwork - 4:4:4:4", 48, f_semi_20, COLOR_CYAN_PURE)

        # Summary Card
        draw_rounded_card(15, 72, 210, 116, 8, (0, 77, 64, 80))
        draw_text_center("05:00", 78, f_semi_25, COLOR_WHITE)
        draw_text_center("18 Cycles Completed", 108, f_reg_16, COLOR_AQUA)
        draw_text_center("Avg HR: 62 bpm", 136, f_reg_14, COLOR_WHITE)
        draw_text_center("Calories: 28 kcal", 160, f_reg_14, COLOR_GRAY)

        draw_text_center("Press R1 to Finish", 198, f_semi_20, COLOR_GREEN)

    # Circular mask to ensure strict circular boundary
    mask = Image.new("L", (dim, dim), 0)
    mask_draw = ImageDraw.Draw(mask)
    mask_draw.ellipse([0, 0, dim - 1, dim - 1], fill=255)
    img.putalpha(mask)

    return img

def composite_mockup(screen_img: Image.Image, watch_case: str) -> Image.Image:
    """
    Composites the circular screen into the 2000x2000 watch frame underneath the bezel.
    Center is at (1000, 1010), diameter is 734 px.
    """
    watch_path = os.path.join(MOCKUP_DIR, f"watch_{watch_case}.png")
    watch_img = Image.open(watch_path).convert("RGBA")

    # Resize screen cleanly to 734 x 734 using high-quality Lanczos resampling
    screen_resized = screen_img.resize((734, 734), Image.Resampling.LANCZOS)

    # Create composite canvas
    canvas = Image.new("RGBA", (2000, 2000), (0, 0, 0, 0))
    # Paste screen at center (1000 - 367, 1010 - 367) = (633, 643)
    canvas.paste(screen_resized, (633, 643), screen_resized)

    # Overlay watch mockup on top (the bezel and glass reflections overlay the screen)
    canvas.alpha_composite(watch_img)
    return canvas

def main():
    os.makedirs(SCREENSHOT_DIR, exist_ok=True)
    os.makedirs(PREVIEW_DIR, exist_ok=True)

    configs = [
        ("01_main_teal",            "main",           "teal",  "Main Screen (Teal Watch)"),
        ("02_pattern_select_black", "pattern_select", "black", "Pattern Selection Screen (Black Watch)"),
        ("03_breath_inhale_white",  "breath_inhale",  "white", "Active Breathing Inhale (White Watch)"),
        ("04_breath_hold_teal",     "breath_hold",    "teal",  "Active Breathing Hold (Teal Watch)"),
        ("05_breath_exhale_black",  "breath_exhale",  "black", "Active Breathing Exhale (Black Watch)"),
        ("06_summary_white",        "summary",        "white", "Workout Summary Screen (White Watch)"),
    ]

    for filename, screen_type, watch_case, title in configs:
        print(f"Generating {title}...")
        # 1. Render high-resolution screen (4x supersampled = 960x960)
        hi_screen = render_screen(screen_type, scale=4.0)

        # 2. Save 240x240 preview
        preview_240 = hi_screen.resize((240, 240), Image.Resampling.LANCZOS)
        preview_path = os.path.join(PREVIEW_DIR, f"{filename}.png")
        preview_240.save(preview_path, "PNG")

        # 3. Composite into 2000x2000 device mockup
        mockup_img = composite_mockup(hi_screen, watch_case)

        # Also save web-optimized 1000x1000 screenshot alongside 2000x2000 full-res
        mockup_path = os.path.join(SCREENSHOT_DIR, f"screenshot_{filename}.png")
        mockup_img.save(mockup_path, "PNG", optimize=True)

        print(f"  -> Saved {mockup_path} and {preview_path}")

    print("\nAll screenshots generated successfully!")

if __name__ == "__main__":
    main()
