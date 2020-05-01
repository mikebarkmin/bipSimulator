#pragma once
//#include <pebble.h>
#include "../macros.h"
#include "../common.h"
#include "line.h"

/*! \addtogroup circle
 *  Functions for drawing or filling circles.
 *  @{
 */

/*!
 * Fills a circle.
 *
 * The circle is centered on a point, so it always has an odd diameter.
 */
void n_graphics_fill_circle(n_GContext * ctx, n_GPoint p, unsigned short radius);
/*!
 * Draws the outline of a circle.
 *
 * The circle is centered on a point, so it always has an odd diameter.
 */
void n_graphics_draw_circle(n_GContext * ctx, n_GPoint p, unsigned short radius);

/*!
 * Fills a circle within the given bounds.
 */
void n_graphics_fill_circle_bounded(n_GContext * ctx, n_GPoint p, unsigned short radius, short minx, short maxx, short miny, short maxy);
/*!
 * Draws the outline of a circle within the given bounds.
 */
void n_graphics_draw_circle_bounded(n_GContext * ctx, n_GPoint p, unsigned short radius, short minx, short maxx, short miny, short maxy);


/*!
 * Fills a quadrant of a circle.
 */
void n_graphics_prv_draw_quarter_circle_bounded(n_GContext * ctx, n_GPoint p,
    unsigned short radius, unsigned short width, signed char x_dir, signed char y_dir,
    short minx, short maxx, short miny, short maxy);
/*!
 * Draws the outline of a quadrant of a circle.
 */
void n_graphics_prv_fill_quarter_circle_bounded(n_GContext * ctx, n_GPoint p,
    unsigned short radius, signed char x_dir, signed char y_dir,
    short minx, short maxx, short miny, short maxy);

/*! @}
 */
