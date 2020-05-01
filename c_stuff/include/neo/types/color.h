#pragma once
#ifdef NGFX_IS_CORE
#include <stdint.h>
#else
//#include <pebble.h>
#endif
#include "colors.h"

/*-----------------------------------------------------------------------------.
|                                                                              |
|                                   Color                                      |
|                                                                              |
`-----------------------------------------------------------------------------*/

/*! \addtogroup Types
 *  @{
 *  \addtogroup Color
 *  Color-handling code.
 *  @{
 */

/*!
 * Internal representation of colors: now with revolutionary 2-bit-per-channel
 * technology!
 */
typedef union n_GColor8 {
    struct {
        // little endian, so reverse field order (within byte)
        char b:2;
        char g:2;
        char r:2;
        char a:2;
    };
    char argb;
} n_GColor8;

typedef n_GColor8 n_GColor;

/*!
 * Check for the equality of two n_GColor(s).
 */
static inline char n_gcolor_equal(n_GColor8 a, n_GColor8 b) {
   return a.argb == b.argb;
}

/*!
 * Get the ideal text n_GColor over a given background.
 */
static inline n_GColor8 n_gcolor_legible_over(n_GColor8 color) {
   return (color.r + color.g + color.b > 6) ? n_GColorWhite : n_GColorBlack;
}

/*!
 * Blends a n_GColor over another.
 */
static inline n_GColor8 n_gcolor_blend(n_GColor8 dst, n_GColor8 src) {
    dst.r = (src.a * src.r + (3 - src.a) * dst.r) / 3;
    dst.g = (src.a * src.g + (3 - src.a) * dst.g) / 3;
    dst.b = (src.a * src.b + (3 - src.a) * dst.b) / 3;
    dst.a = 3;
    return dst;
}

/*!
 * Convenience macro to create the closest n_GColor to 4 8-bit values.
 */
#define n_GColorFromRGBA(_r, _g, _b, _a) \
    ((n_GColor8) {{.a = ((_a) >> 6) & 0b11, .r = ((_r) >> 6) & 0b11,\
                   .g = ((_g) >> 6) & 0b11, .b = ((_b) >> 6) & 0b11}})

/*!
 * Convenience macro to create the closest n_GColor to 3 8-bit values.
 */
#define n_GColorFromRGB(_r, _g, _b) \
    ((n_GColor8) {{.a =               0b11, .r = ((_r) >> 6) & 0b11,\
                   .g = ((_g) >> 6) & 0b11, .b = ((_b) >> 6) & 0b11}})

/*!
 * Convenience macro to create the closest n_GColor to a hex value (eg 0xFF7700).
 */
#define n_GColorFromHEX(_h) n_GColorFromRGB(((_h) >> 16) & 0b11111111, \
                                            ((_h) >>  8) & 0b11111111, \
                                             (_h)        & 0b11111111)

/*! @}
 *  @}
 */
