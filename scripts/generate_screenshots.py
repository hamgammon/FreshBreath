#!/usr/bin/env python3
"""
Generate pixel-accurate watch UI screens (v1.0.1 WheelMenu + TimerRing UI)
and composite photorealistic device mockups for FreshBreath using the official
UNA Watch Penpot templates and Poppins fonts.
"""

import math
import os
from PIL import Image, ImageDraw, ImageFont

UNA_SDK = os.environ.get("UNA_SDK", os.path.expanduser("~/repos/una-sdk"))
FONT_DIR = os.path.join(UNA_SDK, "Examples/Apps/Workout/Software/Apps/TouchGFX-GUI/assets/fonts")
MOCKUP_DIR = "assets/mockups"
SCREENSHOT_DIR = "assets/screenshots"
PREVIEW_DIR = "assets/previews"


def get_font(name: str, size: int):
    path = os.path.join(FONT_DIR, f"{name}.ttf")
    if os.path.exists(path):
        return ImageFont.truetype(path, size)
    return ImageFont.load_default()


# Colors aligned with Theme::Color (RGB888)
COLOR_BLACK       = (0, 0, 0, 255)
COLOR_WHITE       = (255, 255, 255, 255)
COLOR_GRAY_LIGHT  = (170, 170, 170, 255)
COLOR_GRAY_DARK   = (102, 102, 102, 255)
COLOR_CARD_BG     = (34, 34, 34, 255)
COLOR_RING_BG     = (32, 32, 32, 255)
COLOR_CYAN_PURE   = (0, 229, 255, 255)
COLOR_AQUA        = (0, 176, 255, 255)
COLOR_SKY         = (79, 195, 247, 255)
COLOR_GREEN       = (64, 192, 0, 255)
COLOR_AMBER       = (255, 171, 0, 255)
COLOR_YELLOW      = (255, 214, 0, 255)
COLOR_RED         = (255, 64, 64, 255)

# Hardware button angles (0 = 12 o'clock clockwise -> PIL: angle - 90)
BUTTON_ARCS = {
    'L1': (291 - 90, 308 - 90),
    'L2': (232 - 90, 249 - 90),
    'R1': (52 - 90, 69 - 90),
    'R2': (112 - 90, 128 - 90),
}


def draw_hardware_buttons(draw: ImageDraw.ImageDraw, scale: float, buttons: dict):
    cx, cy = 120 * scale, 120 * scale
    radius = 115 * scale
    width = int(round(5 * scale))
    bbox = [cx - radius, cy - radius, cx + radius, cy + radius]
    for key, color in buttons.items():
        if color and key in BUTTON_ARCS:
            start, end = BUTTON_ARCS[key]
            draw.arc(bbox, start=start, end=end, fill=color, width=width)


def draw_title_bar(draw: ImageDraw.ImageDraw, scale: float, title: str, time_str: str = "10:08"):
    f_reg_14 = get_font("Poppins-Regular", int(14 * scale))
    f_semi_16 = get_font("Poppins-SemiBold", int(16 * scale))
    cx = 120 * scale

    # Time + battery row at top (y=8)
    tb = draw.textbbox((0, 0), time_str, font=f_reg_14)
    tw = tb[2] - tb[0]
    draw.text((cx - tw / 2 - 10 * scale, 7 * scale), time_str, font=f_reg_14, fill=COLOR_GRAY_LIGHT)

    # Small battery icon next to time
    bx = cx + tw / 2 - 4 * scale
    by = 10 * scale
    bw, bh = 16 * scale, 9 * scale
    draw.rounded_rectangle([bx, by, bx + bw, by + bh], radius=2 * scale, outline=COLOR_GRAY_LIGHT, width=max(1, int(1.2 * scale)))
    draw.rectangle([bx + bw, by + 2.5 * scale, bx + bw + 2 * scale, by + bh - 2.5 * scale], fill=COLOR_GRAY_LIGHT)
    draw.rectangle([bx + 2 * scale, by + 2 * scale, bx + bw - 3 * scale, by + bh - 2 * scale], fill=COLOR_GREEN)

    # Title label (y=24)
    ttb = draw.textbbox((0, 0), title, font=f_semi_16)
    ttw = ttb[2] - ttb[0]
    draw.text((cx - ttw / 2, 24 * scale), title, font=f_semi_16, fill=COLOR_WHITE)


def draw_wheel_menu(draw: ImageDraw.ImageDraw, scale: float,
                    prev_title: str, curr_title: str, curr_sub: str, next_title: str,
                    accent_color=COLOR_CYAN_PURE):
    f_reg_16 = get_font("Poppins-Regular", int(16 * scale))
    f_semi_20 = get_font("Poppins-SemiBold", int(20 * scale))
    f_reg_14 = get_font("Poppins-Regular", int(14 * scale))
    cx = 120 * scale

    # Previous item (top, dimmed)
    if prev_title:
        pb = draw.textbbox((0, 0), prev_title, font=f_reg_16)
        draw.text((cx - (pb[2] - pb[0]) / 2, 55 * scale), prev_title, font=f_reg_16, fill=COLOR_GRAY_DARK)

    # Center selected pill card (x=14..226, y=88..154)
    pill_rect = [14 * scale, 88 * scale, 226 * scale, 154 * scale]
    draw.rounded_rectangle(pill_rect, radius=16 * scale, fill=COLOR_CARD_BG, outline=(58, 58, 60, 255), width=int(1.5 * scale))

    # Left accent bar inside pill
    bar_rect = [22 * scale, 100 * scale, 26 * scale, 142 * scale]
    draw.rounded_rectangle(bar_rect, radius=2 * scale, fill=accent_color)

    # Selected title + subtitle
    if curr_sub:
        cb = draw.textbbox((0, 0), curr_title, font=f_semi_20)
        draw.text((cx - (cb[2] - cb[0]) / 2, 95 * scale), curr_title, font=f_semi_20, fill=COLOR_WHITE)
        sb = draw.textbbox((0, 0), curr_sub, font=f_reg_14)
        draw.text((cx - (sb[2] - sb[0]) / 2, 126 * scale), curr_sub, font=f_reg_14, fill=accent_color)
    else:
        cb = draw.textbbox((0, 0), curr_title, font=f_semi_20)
        draw.text((cx - (cb[2] - cb[0]) / 2, 108 * scale), curr_title, font=f_semi_20, fill=COLOR_WHITE)

    # Next item (bottom, dimmed)
    if next_title:
        nb = draw.textbbox((0, 0), next_title, font=f_reg_16)
        draw.text((cx - (nb[2] - nb[0]) / 2, 168 * scale), next_title, font=f_reg_16, fill=COLOR_GRAY_DARK)


def draw_timer_ring(draw: ImageDraw.ImageDraw, scale: float, permille: int, color):
    cx, cy = 120 * scale, 120 * scale
    r = 109 * scale
    w = int(6 * scale)
    bbox = [cx - r, cy - r, cx + r, cy + r]
    draw.arc(bbox, start=0, end=360, fill=COLOR_RING_BG, width=w)
    if permille > 0:
        end_deg = -90 + (360.0 * permille / 1000.0)
        draw.arc(bbox, start=-90, end=end_deg, fill=color, width=w)


def draw_heart_icon(draw: ImageDraw.ImageDraw, cx: float, cy: float, size: float, color):
    points = []
    steps = 40
    for i in range(steps):
        t = i * 2.0 * math.pi / steps
        x = 16 * (math.sin(t) ** 3)
        y = -(13 * math.cos(t) - 5 * math.cos(2 * t) - 2 * math.cos(3 * t) - math.cos(4 * t))
        s = size / 36.0
        points.append((cx + x * s, cy + y * s))
    draw.polygon(points, fill=color)


def draw_pause_bars(draw: ImageDraw.ImageDraw, cx: float, cy: float, scale: float, color):
    w = 4 * scale
    h = 14 * scale
    gap = 3 * scale
    draw.rounded_rectangle([cx - gap - w, cy - h / 2, cx - gap, cy + h / 2], radius=1.5 * scale, fill=color)
    draw.rounded_rectangle([cx + gap, cy - h / 2, cx + gap + w, cy + h / 2], radius=1.5 * scale, fill=color)


def render_screen(screen_type: str, scale: float = 4.0) -> Image.Image:
    dim = int(240 * scale)
    img = Image.new("RGBA", (dim, dim), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Base black circular display
    draw.ellipse([0, 0, dim - 1, dim - 1], fill=(0, 0, 0, 255))

    f_reg_14 = get_font("Poppins-Regular", int(14 * scale))
    f_reg_16 = get_font("Poppins-Regular", int(16 * scale))
    f_semi_20 = get_font("Poppins-SemiBold", int(20 * scale))
    f_semi_25 = get_font("Poppins-SemiBold", int(25 * scale))
    f_semi_35 = get_font("Poppins-SemiBold", int(35 * scale))
    f_semi_48 = get_font("Poppins-SemiBold", int(48 * scale))

    cx = 120 * scale
    cy = 120 * scale

    def draw_text_center(text: str, y: float, font, color):
        bbox = draw.textbbox((0, 0), text, font=font)
        w = bbox[2] - bbox[0]
        draw.text((cx - w / 2, y * scale), text, font=font, fill=color)

    if screen_type == "main":
        draw_hardware_buttons(draw, scale, {
            'L1': COLOR_WHITE,
            'L2': COLOR_WHITE,
            'R1': COLOR_CYAN_PURE,
            'R2': COLOR_WHITE,
        })
        draw_title_bar(draw, scale, "Fresh Breath")
        draw_wheel_menu(
            draw, scale,
            prev_title="Alert Mode",
            curr_title="Start Breath",
            curr_sub="Box 4-4-4-4",
            next_title="Pattern",
            accent_color=COLOR_CYAN_PURE,
        )

    elif screen_type == "pattern_select":
        draw_hardware_buttons(draw, scale, {
            'L1': COLOR_WHITE,
            'L2': COLOR_WHITE,
            'R1': COLOR_GREEN,
            'R2': COLOR_WHITE,
        })
        draw_title_bar(draw, scale, "Pattern")
        draw_wheel_menu(
            draw, scale,
            prev_title="Equal Flow",
            curr_title="Box 4-4-4-4",
            curr_sub="[Active] 4-4-4-4s",
            next_title="4-7-8 Relax",
            accent_color=COLOR_GREEN,
        )

    elif screen_type == "countdown":
        draw_hardware_buttons(draw, scale, {
            'R1': COLOR_AMBER,
            'R2': COLOR_WHITE,
        })
        draw_timer_ring(draw, scale, 800, COLOR_CYAN_PURE)
        f_med_18 = get_font("Poppins-Medium", int(18 * scale))
        f_italic_18 = get_font("Poppins-Italic", int(18 * scale))
        f_semi_60 = get_font("Poppins-SemiBold", int(60 * scale))

        # Horizontal separator lines
        draw.line([(35 * scale, 81 * scale), (205 * scale, 81 * scale)], fill=(58, 58, 60, 255), width=max(1, int(1.5 * scale)))
        draw.line([(35 * scale, 113 * scale), (205 * scale, 113 * scale)], fill=(58, 58, 60, 255), width=max(1, int(1.5 * scale)))

        # Tick icon at (178, 62) and Cross icon at (180, 162)
        draw.line([(178 * scale, 69 * scale), (184 * scale, 75 * scale), (195 * scale, 62 * scale)],
                  fill=COLOR_GREEN, width=max(2, int(2.5 * scale)))
        draw.line([(180 * scale, 162 * scale), (192 * scale, 174 * scale)], fill=COLOR_WHITE, width=max(2, int(2.2 * scale)))
        draw.line([(192 * scale, 162 * scale), (180 * scale, 174 * scale)], fill=COLOR_WHITE, width=max(2, int(2.2 * scale)))

        draw_text_center("Box 4-4-4-4", 53, f_semi_20, COLOR_CYAN_PURE)
        draw_text_center("5 min", 85, f_med_18, COLOR_WHITE)
        draw_text_center("Get Ready", 117, f_italic_18, COLOR_GRAY_LIGHT)
        draw_text_center("5", 138, f_semi_60, COLOR_WHITE)

    elif screen_type.startswith("breath_"):
        draw_hardware_buttons(draw, scale, {
            'R1': COLOR_AMBER,
            'R2': COLOR_WHITE,
        })
        arc_r = 102 * scale
        arc_bbox = [cx - arc_r, cy - arc_r, cx + arc_r, cy + arc_r]
        draw.arc(arc_bbox, start=0, end=360, fill=COLOR_RING_BG, width=int(10 * scale))

        if screen_type == "breath_inhale":
            draw.arc(arc_bbox, start=-90, end=15, fill=COLOR_CYAN_PURE, width=int(10 * scale))
            draw_heart_icon(draw, 98 * scale, 48 * scale, 16 * scale, COLOR_RED)
            draw.text((112 * scale, 35 * scale), "68", font=f_semi_20, fill=COLOR_WHITE)
            draw_text_center("INHALE", 76, f_semi_35, COLOR_CYAN_PURE)
            draw_text_center("4", 116, f_semi_48, COLOR_WHITE)
            draw_text_center("00:42 / 05:00", 176, f_reg_16, COLOR_GRAY_LIGHT)
            draw_text_center("Cycle 3", 198, f_reg_14, COLOR_AQUA)

        elif screen_type == "breath_hold":
            draw.arc(arc_bbox, start=0, end=360, fill=COLOR_YELLOW, width=int(10 * scale))
            draw_heart_icon(draw, 98 * scale, 48 * scale, 16 * scale, COLOR_RED)
            draw.text((112 * scale, 35 * scale), "64", font=f_semi_20, fill=COLOR_WHITE)
            draw_text_center("HOLD", 76, f_semi_35, COLOR_YELLOW)
            draw_text_center("3", 116, f_semi_48, COLOR_WHITE)
            draw_text_center("01:18 / 05:00", 176, f_reg_16, COLOR_GRAY_LIGHT)
            draw_text_center("Cycle 5", 198, f_reg_14, COLOR_AQUA)

        elif screen_type == "breath_exhale":
            draw.arc(arc_bbox, start=-90, end=110, fill=COLOR_SKY, width=int(10 * scale))
            draw_heart_icon(draw, 98 * scale, 48 * scale, 16 * scale, COLOR_RED)
            draw.text((112 * scale, 35 * scale), "61", font=f_semi_20, fill=COLOR_WHITE)
            draw_text_center("EXHALE", 76, f_semi_35, COLOR_SKY)
            draw_text_center("2", 116, f_semi_48, COLOR_WHITE)
            draw_text_center("02:45 / 05:00", 176, f_reg_16, COLOR_GRAY_LIGHT)
            draw_text_center("Cycle 11", 198, f_reg_14, COLOR_AQUA)

    elif screen_type == "hold_confirm":
        draw_hardware_buttons(draw, scale, {
            'R1': COLOR_GREEN,
        })
        draw_timer_ring(draw, scale, 666, COLOR_GREEN)
        f_med_18 = get_font("Poppins-Medium", int(18 * scale))
        f_semi_60 = get_font("Poppins-SemiBold", int(60 * scale))
        draw.line([(178 * scale, 69 * scale), (184 * scale, 75 * scale), (195 * scale, 62 * scale)],
                  fill=COLOR_GREEN, width=max(2, int(2.5 * scale)))
        draw_text_center("Hold to", 62, f_med_18, COLOR_WHITE)
        draw_text_center("Finish", 82, f_med_18, COLOR_WHITE)
        draw_text_center("2", 108, f_semi_60, COLOR_WHITE)

    elif screen_type == "summary":
        draw_hardware_buttons(draw, scale, {
            'R1': COLOR_AMBER,
            'R2': COLOR_WHITE,
        })
        draw_title_bar(draw, scale, "SUMMARY")
        f_italic_18 = get_font("Poppins-Italic", int(18 * scale))
        draw_text_center("Box 4-4-4-4", 48, f_semi_20, COLOR_CYAN_PURE)
        draw_text_center("05:00", 76, f_semi_35, COLOR_WHITE)
        draw_text_center("Duration", 114, f_italic_18, COLOR_GRAY_LIGHT)

        draw.line([(30 * scale, 140 * scale), (210 * scale, 140 * scale)], fill=(58, 58, 60, 255), width=max(1, int(1.5 * scale)))

        # Left column: Cycles
        b1 = draw.textbbox((0, 0), "19", font=f_semi_25)
        draw.text(((68 * scale) - (b1[2] - b1[0]) / 2, 148 * scale), "19", font=f_semi_25, fill=COLOR_AQUA)
        b2 = draw.textbbox((0, 0), "Cycles", font=f_italic_18)
        draw.text(((68 * scale) - (b2[2] - b2[0]) / 2, 178 * scale), "Cycles", font=f_italic_18, fill=COLOR_GRAY_LIGHT)

        # Right column: Avg HR
        b3 = draw.textbbox((0, 0), "62", font=f_semi_25)
        draw.text(((172 * scale) - (b3[2] - b3[0]) / 2, 148 * scale), "62", font=f_semi_25, fill=COLOR_WHITE)
        b4 = draw.textbbox((0, 0), "Avg HR", font=f_italic_18)
        draw.text(((172 * scale) - (b4[2] - b4[0]) / 2, 178 * scale), "Avg HR", font=f_italic_18, fill=COLOR_GRAY_LIGHT)

        # ScrollIndicator on right side (x=222)
        draw.ellipse([219 * scale, 110 * scale, 225 * scale, 116 * scale], fill=COLOR_CYAN_PURE)
        draw.ellipse([220 * scale, 124 * scale, 224 * scale, 128 * scale], fill=COLOR_GRAY_DARK)

    # Circular mask
    mask = Image.new("L", (dim, dim), 0)
    mask_draw = ImageDraw.Draw(mask)
    mask_draw.ellipse([0, 0, dim - 1, dim - 1], fill=255)
    img.putalpha(mask)

    return img


def composite_mockup(screen_img: Image.Image, watch_case: str) -> Image.Image:
    watch_path = os.path.join(MOCKUP_DIR, f"watch_{watch_case}.png")
    watch_img = Image.open(watch_path).convert("RGBA")
    screen_resized = screen_img.resize((734, 734), Image.Resampling.LANCZOS)

    canvas = Image.new("RGBA", (2000, 2000), (0, 0, 0, 0))
    canvas.paste(screen_resized, (633, 643), screen_resized)
    canvas.alpha_composite(watch_img)
    return canvas


def main():
    os.makedirs(SCREENSHOT_DIR, exist_ok=True)
    os.makedirs(PREVIEW_DIR, exist_ok=True)

    configs = [
        ("01_main_teal",            "main",           "teal",  "Main WheelMenu Screen (Teal Watch)"),
        ("02_pattern_select_black", "pattern_select", "black", "Pattern Selection WheelMenu (Black Watch)"),
        ("03_countdown_white",      "countdown",      "white", "5s Preparation Countdown (White Watch)"),
        ("04_breath_inhale_white",  "breath_inhale",  "white", "Active Breathing Inhale (White Watch)"),
        ("05_breath_hold_teal",     "breath_hold",    "teal",  "Active Breathing Hold (Teal Watch)"),
        ("06_breath_exhale_black",  "breath_exhale",  "black", "Active Breathing Exhale (Black Watch)"),
        ("07_hold_confirm_teal",    "hold_confirm",   "teal",  "3s Hold Confirm Save & End (Teal Watch)"),
        ("08_summary_white",        "summary",        "white", "Workout Summary Screen (White Watch)"),
        # Keep legacy filename alias for backwards compatibility
        ("03_breath_inhale_white",  "breath_inhale",  "white", "Legacy Alias Inhale"),
        ("04_breath_hold_teal",     "breath_hold",    "teal",  "Legacy Alias Hold"),
        ("05_breath_exhale_black",  "breath_exhale",  "black", "Legacy Alias Exhale"),
        ("06_summary_white",        "summary",        "white", "Legacy Alias Summary"),
    ]

    for filename, screen_type, watch_case, title in configs:
        print(f"Generating {title} -> {filename}...")
        hi_screen = render_screen(screen_type, scale=4.0)

        preview_240 = hi_screen.resize((240, 240), Image.Resampling.LANCZOS)
        preview_path = os.path.join(PREVIEW_DIR, f"{filename}.png")
        preview_240.save(preview_path, "PNG")

        mockup_img = composite_mockup(hi_screen, watch_case)
        mockup_path = os.path.join(SCREENSHOT_DIR, f"screenshot_{filename}.png")
        mockup_img.save(mockup_path, "PNG", optimize=True)

    print("\nAll v1.0.1 screenshots generated successfully!")


if __name__ == "__main__":
    main()
