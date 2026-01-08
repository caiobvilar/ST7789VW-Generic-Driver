#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ST7789_ORIENT_PORTRAIT = 0,
    ST7789_ORIENT_LANDSCAPE,
    ST7789_ORIENT_PORTRAIT_INV,
    ST7789_ORIENT_LANDSCAPE_INV,
} st7789_orientation_t;

typedef enum {
    ST7789_COLOR_MODE_RGB565 = 0x55,  /* 16‑bpp, 65K colors (3Ah, DB[6:4]=101) */
    ST7789_COLOR_MODE_RGB666 = 0x66   /* 18‑bpp, 262K colors (3Ah, DB[6:4]=110) */
} st7789_color_mode_t;

typedef struct {
    uint16_t width;           /* logical width (e.g. 240) */
    uint16_t height;          /* logical height (e.g. 320) */
    st7789_orientation_t orientation;
    st7789_color_mode_t  color_mode;

    /* HAL callbacks – you implement these per MCU/interface */

    void (*reset_assert)(void);
    void (*reset_release)(void);
    void (*delay_ms)(uint32_t ms);

    /* Write a single 8‑bit command */
    void (*write_cmd)(uint8_t cmd);

    /* Write command + 8‑bit parameter(s) */
    void (*write_cmd1)(uint8_t cmd, uint8_t data);
    void (*write_cmdN)(uint8_t cmd, const uint8_t *data, uint16_t len);

    /* Write pixel data stream (GRAM write) */
    void (*write_data)(const uint8_t *data, uint32_t len);
} st7789_t;

/* Core API */
bool st7789_init(st7789_t *lcd);
void st7789_set_orientation(st7789_t *lcd, st7789_orientation_t o);
void st7789_set_window(st7789_t *lcd,
                       uint16_t x0, uint16_t y0,
                       uint16_t x1, uint16_t y1);
void st7789_fill_rect(st7789_t *lcd,
                      uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h,
                      uint16_t color);      /* RGB565 */

void st7789_draw_pixels(st7789_t *lcd,
                        uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h,
                        const uint16_t *pixels); /* RGB565 buffer */

#endif /* ST7789_H */
