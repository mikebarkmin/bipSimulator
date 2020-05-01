#include "circle.h"

void n_graphics_fill_circle_bounded(n_GContext * ctx, n_GPoint p, unsigned short radius,
        short minx, short maxx, short miny, short maxy) {
    int err = 1 - radius,
            err_a = -radius * 2,
            err_b = 0;
    unsigned short a = radius,
             b = 0;
#ifdef PBL_BW
    char bytefill = __ARGB_TO_INTERNAL(ctx->fill_color.argb);
#else
    char bytefill = ctx->fill_color.argb;
#endif
    while (b <= a) {
        n_graphics_prv_draw_row(ctx->fbuf, p.y - b, p.x - a, p.x + a, minx, maxx, miny, maxy, bytefill);
        n_graphics_prv_draw_row(ctx->fbuf, p.y + b, p.x - a, p.x + a, minx, maxx, miny, maxy, bytefill);
        if (err >= 0) {
            n_graphics_prv_draw_row(ctx->fbuf, p.y - a, p.x - b, p.x + b, minx, maxx, miny, maxy, bytefill);
            n_graphics_prv_draw_row(ctx->fbuf, p.y + a, p.x - b, p.x + b, minx, maxx, miny, maxy, bytefill);
            b += 1;
            a -= 1;
            err_a += 2;
            err_b += 2;
            err += err_a + err_b;
        } else {
            b += 1;
            err_b += 2;
            err += err_b + 1;
        }
    }
}

void n_graphics_draw_circle_1px_bounded(n_GContext * ctx, n_GPoint p, unsigned short radius, short minx, short maxx, short miny, short maxy) {
    unsigned short a = radius,
             b = 0;
    short err = 1 - a,
             err_a = -a * 2,
             err_b = 1;
    while (b <= a) {
        if (p.x + a >= minx && p.x + a < maxx) {
            if (p.y + b >= miny && p.y + b < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x + a, p.y + b));
            if (p.y - b >= miny && p.y - b < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x + a, p.y - b));
        }
        if (p.x - a >= minx && p.x - a < maxx) {
            if (p.y + b >= miny && p.y + b < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x - a, p.y + b));
            if (p.y - b >= miny && p.y - b < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x - a, p.y - b));
        }
        if (p.x + b >= minx && p.x + b < maxx) {
            if (p.y + a >= miny && p.y + a < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x + b, p.y + a));
            if (p.y - a >= miny && p.y - a < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x + b, p.y - a));
        }
        if (p.x - b >= minx && p.x - b < maxx) {
            if (p.y + a >= miny && p.y + a < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x - b, p.y + a));
            if (p.y - a >= miny && p.y - a < maxy)
                n_graphics_draw_pixel(ctx, n_GPoint(p.x - b, p.y - a));
        }
        if (err >= 0) {
            a -= 1;
            b += 1;
            err_a += 2;
            err_b += 2;
            err += err_a + err_b;
        } else {
            b += 1;
            err_b += 2;
            err += err_b + 1;
        }
    }
}

void n_graphics_prv_draw_quarter_circle_bounded(n_GContext * ctx, n_GPoint p,
        unsigned short radius, unsigned short width, signed char x_dir, signed char y_dir,
        short minx, short maxx, short miny, short maxy) {
    unsigned short line_radius = (width - 1) / 2;
    unsigned short a1 = __BOUND_NUM(0, radius - line_radius, radius),
             b1 = 0,
             a2 = radius + line_radius,
             b2 = 0;
    short err1 = 1 - a1,
            err_a1 = -a1 * 2,
            err_b1 = 1,
            // ---
            err2 = 1 - a2,
            err_a2 = -a2 * 2,
            err_b2 = 1;
#ifdef PBL_BW
    char color = __ARGB_TO_INTERNAL(ctx->stroke_color.argb);
#else
    char color = ctx->stroke_color.argb;
#endif
    if (y_dir == 1)
        n_graphics_prv_draw_col(ctx->fbuf, p.x + b2 * x_dir, p.y + a1, p.y + a2,
                                minx, maxx, miny, maxy, color);
    else
        n_graphics_prv_draw_col(ctx->fbuf, p.x + b2 * x_dir, p.y - a2, p.y - a1,
                                minx, maxx, miny, maxy, color);
    if (x_dir == 1)
        n_graphics_prv_draw_row(ctx->fbuf, p.y + b2 * y_dir, p.x + a1, p.x + a2,
                                minx, maxx, miny, maxy, color);
    else
        n_graphics_prv_draw_row(ctx->fbuf, p.y + b2 * y_dir, p.x - a2, p.x - a1,
                                minx, maxx, miny, maxy, color);
    while ((b1 <= a1 && a1 != 0) || b2 <= a2) {
        if (b2 <= a2 && a2 != 0) {
            if (err2 >= 0) {
                b2 += 1;
                a2 -= 1;
                err_a2 += 2;
                err_b2 += 2;
                err2 += err_a2 + err_b2;
            } else {
                b2 += 1;
                err_b2 += 2;
                err2 += err_b2 + 1;
            }
        }
        if (b1 <= a1 && a1 != 0) {
            if (err1 >= 0) {
                a1 -= 1;
                b1 += 1;
                err_a1 += 2;
                err_b1 += 2;
                err1 += err_a1 + err_b1;
            } else {
                b1 += 1;
                err_b1 += 2;
                err1 += err_b1 + 1;
            }
        }
        // TODO possible optimization by not rendering so far
        if (y_dir == 1)
            n_graphics_prv_draw_col(ctx->fbuf, p.x + b2 * x_dir, p.y + a1, p.y + a2,
                                    minx, maxx, miny, maxy, color);
        else
            n_graphics_prv_draw_col(ctx->fbuf, p.x + b2 * x_dir, p.y - a2, p.y - a1,
                                    minx, maxx, miny, maxy, color);
        if (x_dir == 1)
            n_graphics_prv_draw_row(ctx->fbuf, p.y + b2 * y_dir, p.x + a1, p.x + a2,
                                    minx, maxx, miny, maxy, color);
        else
            n_graphics_prv_draw_row(ctx->fbuf, p.y + b2 * y_dir, p.x - a2, p.x - a1,
                                    minx, maxx, miny, maxy, color);
    }
}

void n_graphics_prv_fill_quarter_circle_bounded(n_GContext * ctx, n_GPoint p,
        unsigned short radius, signed char x_dir, signed char y_dir,
        short minx, short maxx, short miny, short maxy) {
    int err = 1 - radius,
            err_a = -radius * 2,
            err_b = 0;
    unsigned short a = radius,
             b = 0;
#ifdef PBL_BW
    char bytefill = __ARGB_TO_INTERNAL(ctx->fill_color.argb);
#else
    char bytefill = ctx->fill_color.argb;
#endif
    while (b <= a) {
        if (x_dir == 1) {
            n_graphics_prv_draw_row(ctx->fbuf, p.y + b * y_dir, p.x, p.x + a, minx, maxx, miny, maxy, bytefill);
        } else {
            n_graphics_prv_draw_row(ctx->fbuf, p.y + b * y_dir, p.x - a, p.x, minx, maxx, miny, maxy, bytefill);
        }

        if (err >= 0) {
            if (x_dir == 1) {
                n_graphics_prv_draw_row(ctx->fbuf, p.y + a * y_dir, p.x, p.x + b, minx, maxx, miny, maxy, bytefill);
            } else {
                n_graphics_prv_draw_row(ctx->fbuf, p.y + a * y_dir, p.x - b, p.x, minx, maxx, miny, maxy, bytefill);
            }
            a -= 1;
            b += 1;
            err_a += 2;
            err_b += 2;
            err += err_a + err_b;
        } else {
            b += 1;
            err_b += 2;
            err += err_b + 1;
        }
    }
}

void n_graphics_draw_thick_circle_bounded(n_GContext * ctx, n_GPoint p, unsigned short radius, unsigned short width, short minx, short maxx, short miny, short maxy) {
    unsigned short line_radius = (width - 1) / 2;
    unsigned short a1 = __BOUND_NUM(0, radius - line_radius, radius),
             b1 = 0,
             a2 = radius + line_radius,
             b2 = 0;
    short err1 = 1 - a1,
            err_a1 = -a1 * 2,
            err_b1 = 1,
            // ---
            err2 = 1 - a2,
            err_a2 = -a2 * 2,
            err_b2 = 1;
#ifdef PBL_BW
    char color = __ARGB_TO_INTERNAL(ctx->stroke_color.argb);
#else
    char color = ctx->stroke_color.argb;
#endif

    n_graphics_prv_draw_col(ctx->fbuf, p.x + b2, p.y - a2, p.y - a1,
                            minx, maxx, miny, maxy, color);
    n_graphics_prv_draw_col(ctx->fbuf, p.x + b2, p.y + a1, p.y + a2,
                            minx, maxx, miny, maxy, color);
    n_graphics_prv_draw_col(ctx->fbuf, p.x - b2, p.y - a2, p.y - a1,
                            minx, maxx, miny, maxy, color);
    n_graphics_prv_draw_col(ctx->fbuf, p.x - b2, p.y + a1, p.y + a2,
                            minx, maxx, miny, maxy, color);

    n_graphics_prv_draw_row(ctx->fbuf, p.y + b2, p.x - a2, p.x - a1,
                            minx, maxx, miny, maxy, color);
    n_graphics_prv_draw_row(ctx->fbuf, p.y + b2, p.x + a1, p.x + a2,
                            minx, maxx, miny, maxy, color);
    n_graphics_prv_draw_row(ctx->fbuf, p.y - b2, p.x - a2, p.x - a1,
                            minx, maxx, miny, maxy, color);
    n_graphics_prv_draw_row(ctx->fbuf, p.y - b2, p.x + a1, p.x + a2,
                            minx, maxx, miny, maxy, color);
    while ((b1 <= a1 && a1 != 0) || b2 <= a2) {
        if (b2 <= a2 && a2 != 0) {
            if (err2 >= 0) {
                b2 += 1;
                a2 -= 1;
                err_a2 += 2;
                err_b2 += 2;
                err2 += err_a2 + err_b2;
            } else {
                b2 += 1;
                err_b2 += 2;
                err2 += err_b2 + 1;
            }
        }
        if (b1 <= a1 && a1 != 0) {
            if (err1 >= 0) {
                a1 -= 1;
                b1 += 1;
                err_a1 += 2;
                err_b1 += 2;
                err1 += err_a1 + err_b1;
            } else {
                b1 += 1;
                err_b1 += 2;
                err1 += err_b1 + 1;
            }
        }
        // TODO possible optimization by not rendering so far
        n_graphics_prv_draw_col(ctx->fbuf, p.x + b2, p.y - a2, p.y - a1,
                                minx, maxx, miny, maxy, color);
        n_graphics_prv_draw_col(ctx->fbuf, p.x + b2, p.y + a1, p.y + a2,
                                minx, maxx, miny, maxy, color);
        n_graphics_prv_draw_col(ctx->fbuf, p.x - b2, p.y - a2, p.y - a1,
                                minx, maxx, miny, maxy, color);
        n_graphics_prv_draw_col(ctx->fbuf, p.x - b2, p.y + a1, p.y + a2,
                                minx, maxx, miny, maxy, color);

        n_graphics_prv_draw_row(ctx->fbuf, p.y + b2, p.x - a2, p.x - a1,
                                minx, maxx, miny, maxy, color);
        n_graphics_prv_draw_row(ctx->fbuf, p.y + b2, p.x + a1, p.x + a2,
                                minx, maxx, miny, maxy, color);
        n_graphics_prv_draw_row(ctx->fbuf, p.y - b2, p.x - a2, p.x - a1,
                                minx, maxx, miny, maxy, color);
        n_graphics_prv_draw_row(ctx->fbuf, p.y - b2, p.x + a1, p.x + a2,
                                minx, maxx, miny, maxy, color);
    }
}

void n_graphics_draw_circle(n_GContext * ctx, n_GPoint p, unsigned short radius) {
    if (radius == 0 || !(ctx->stroke_color.argb & (0b11 << 6)))
        return;
    if (ctx->stroke_width == 1) {
        n_graphics_draw_circle_1px_bounded(ctx, p, radius, 0, __SCREEN_WIDTH, 0, __SCREEN_HEIGHT);
    } else {
        // naive approach; testing for speed
        n_graphics_draw_thick_circle_bounded(ctx, p, radius, ctx->stroke_width, 0, __SCREEN_WIDTH, 0, __SCREEN_HEIGHT);
    }
}

void n_graphics_fill_circle(n_GContext * ctx, n_GPoint p, unsigned short radius) {
    if (ctx->fill_color.argb & (0b11 << 6))
        n_graphics_fill_circle_bounded(ctx, p, radius, 0, __SCREEN_WIDTH, 0, __SCREEN_HEIGHT);
}
