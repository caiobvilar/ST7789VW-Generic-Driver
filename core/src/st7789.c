#include "st7789.h"

/* ST7789 command codes (subset) */
#define ST7789_CMD_SWRESET  0x01
#define ST7789_CMD_SLPOUT   0x11
#define ST7789_CMD_DISPON   0x29
#define ST7789_CMD_CASET    0x2A
#define ST7789_CMD_RASET    0x2B
#define ST7789_CMD_RAMWR    0x2C
#define ST7789_CMD_MADCTL   0x36
#define ST7789_CMD_COLMOD   0x3A

/* MADCTL bits */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_BGR 0x08

static void st7789_hw_reset(st7789_t *lcd)
{
    lcd->reset_assert();
    lcd->delay_ms(10);      /* TRW ≥ 10 µs, be generous */
    lcd->reset_release();
    lcd->delay_ms(120);     /* tRT up to 120 ms: internal init/NVM load */
}

static void st7789_set_madctl(st7789_t *lcd)
{
    uint8_t mad = MADCTL_BGR;   /* ST7789 modules usually wired BGR */

    switch (lcd->orientation) {
    case ST7789_ORIENT_PORTRAIT:
        mad |= MADCTL_MX;
        break;
    case ST7789_ORIENT_LANDSCAPE:
        mad |= MADCTL_MV;
        break;
    case ST7789_ORIENT_PORTRAIT_INV:
        mad |= MADCTL_MY;
        break;
    case ST7789_ORIENT_LANDSCAPE_INV:
        mad |= MADCTL_MX | MADCTL_MY | MADCTL_MV;
        break;
    }

    lcd->write_cmd1(ST7789_CMD_MADCTL, mad);
}

bool st7789_init(st7789_t *lcd)
{
    if (!lcd || !lcd->write_cmd || !lcd->write_cmd1 ||
        !lcd->write_cmdN || !lcd->write_data ||
        !lcd->reset_assert || !lcd->reset_release || !lcd->delay_ms) {
        return false;
    }

    st7789_hw_reset(lcd);

    /* Sleep out */
    lcd->write_cmd(ST7789_CMD_SLPOUT);
    lcd->delay_ms(120);     /* datasheet requirement after SLPOUT */

    /* Pixel format (3Ah) – just pass lower byte of enum */
    lcd->write_cmd1(ST7789_CMD_COLMOD, lcd->color_mode & 0xFF);

    /* Memory access control */
    st7789_set_madctl(lcd);

    /* Full window as default (240x320) */
    st7789_set_window(lcd, 0, 0, lcd->width - 1, lcd->height - 1);

    /* Display ON */
    lcd->write_cmd(ST7789_CMD_DISPON);
    lcd->delay_ms(20);

    return true;
}

void st7789_set_orientation(st7789_t *lcd, st7789_orientation_t o)
{
    lcd->orientation = o;
    st7789_set_madctl(lcd);
}

/* Column/row address set – CASET/RASET */
void st7789_set_window(st7789_t *lcd,
                       uint16_t x0, uint16_t y0,
                       uint16_t x1, uint16_t y1)
{
    uint8_t buf[4];

    buf[0] = (x0 >> 8) & 0xFF;
    buf[1] = x0 & 0xFF;
    buf[2] = (x1 >> 8) & 0xFF;
    buf[3] = x1 & 0xFF;
    lcd->write_cmdN(ST7789_CMD_CASET, buf, 4);

    buf[0] = (y0 >> 8) & 0xFF;
    buf[1] = y0 & 0xFF;
    buf[2] = (y1 >> 8) & 0xFF;
    buf[3] = y1 & 0xFF;
    lcd->write_cmdN(ST7789_CMD_RASET, buf, 4);
}

void st7789_fill_rect(st7789_t *lcd,
                      uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h,
                      uint16_t color)
{
    if (!w || !h) return;

    st7789_set_window(lcd, x, y, x + w - 1, y + h - 1);
    lcd->write_cmd(ST7789_CMD_RAMWR);

    uint32_t pixels = (uint32_t)w * h;
    uint8_t px[2] = { color >> 8, color & 0xFF };

    /* Small stack buffer reused to avoid huge allocations */
    uint8_t buf[64];
    for (unsigned i = 0; i < sizeof(buf); i += 2) {
        buf[i]   = px[0];
        buf[i+1] = px[1];
    }

    while (pixels) {
        uint32_t chunk = pixels;
        if (chunk > (sizeof(buf) / 2))
            chunk = sizeof(buf) / 2;
        lcd->write_data(buf, chunk * 2);
        pixels -= chunk;
    }
}

void st7789_draw_pixels(st7789_t *lcd,
                        uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h,
                        const uint16_t *pixels)
{
    if (!w || !h) return;

    st7789_set_window(lcd, x, y, x + w - 1, y + h - 1);
    lcd->write_cmd(ST7789_CMD_RAMWR);

    /* Assume RGB565 input; just push as byte stream */
    uint32_t count = (uint32_t)w * h;
    const uint8_t *p = (const uint8_t *)pixels;
    lcd->write_data(p, count * 2);
}
