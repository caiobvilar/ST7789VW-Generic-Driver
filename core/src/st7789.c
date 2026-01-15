#include "st7789.h"

/* ST7789 command codes (subset) */
#define ST7789_CMD_SWRESET 0x01
#define ST7789_CMD_SLPOUT 0x11
#define ST7789_CMD_DISPON 0x29
#define ST7789_CMD_CASET 0x2A
#define ST7789_CMD_RASET 0x2B
#define ST7789_CMD_RAMWR 0x2C
#define ST7789_CMD_MADCTL 0x36
#define ST7789_CMD_COLMOD 0x3A

/* MADCTL bits */
#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_BGR 0x08

static void
st7789_set_madctl(st7789_t* lcd)
{
    uint8_t mad = 0; // RGB order (no BGR flag)

    switch (lcd->orientation)
    {
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

bool
st7789_init(st7789_t* lcd)
{
    if (!lcd || !lcd->write_cmd || !lcd->write_cmd1 || !lcd->write_cmdN || !lcd->write_data || !lcd->reset_assert ||
        !lcd->reset_release || !lcd->delay_ms)
    {
        return false;
    }

    /* Hardware Reset sequence - same as the working main.c code */
    lcd->reset_assert();
    lcd->delay_ms(10);
    lcd->reset_release();
    lcd->delay_ms(120);

    /* ST7789V Initialization Sequence
       CRITICAL: This configuration works with:
       - SPI Mode 3 (CPOL=1, CPHA=1)
       - INVON (0x21) - Display inversion ON
       - COLMOD = 0x66 (18-bit RGB888)
       - MADCTL = 0x00 (RGB mode, no BGR flag)
       - Pixel bytes sent as {R, G, B} order (standard RGB)
       Example: pure red = {0xFF, 0x00, 0x00} */

    /* 0x01 - SWRESET (Software Reset) */
    lcd->write_cmd(0x01);
    lcd->delay_ms(150);

    /* 0x11 - SLPOUT (Sleep Out) */
    lcd->write_cmd(0x11);
    lcd->delay_ms(120);

    /* 0x21 - INVON (Display Inversion On) */
    lcd->write_cmd(0x21);

    /* 0x3A - COLMOD (Interface Pixel Format) - 18-bit RGB666 */
    lcd->write_cmd1(0x3A, 0x66); // 18-bit

    /* 0x36 - MADCTL (Memory Access Control) - RGB mode (no BGR) */
    lcd->write_cmd1(0x36, 0x00); // RGB mode

    /* 0x2A - CASET (Column Address Set) - 0 to 239 */
    {
        uint8_t caset[4] = {0x00, 0x00, 0x00, 0xEF}; // 239
        lcd->write_cmdN(0x2A, caset, 4);
    }

    /* 0x2B - RASET (Row Address Set) - 0 to 319 */
    {
        uint8_t raset[4] = {0x00, 0x00, 0x01, 0x3F}; // 319
        lcd->write_cmdN(0x2B, raset, 4);
    }

    /* 0x29 - DISPON (Display ON) */
    lcd->write_cmd(0x29);
    lcd->delay_ms(100);

    return true;
}

void
st7789_set_orientation(st7789_t* lcd, st7789_orientation_t o)
{
    lcd->orientation = o;
    st7789_set_madctl(lcd);
}

/* Column/row address set – CASET/RASET */
void
st7789_set_window(st7789_t* lcd, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t buf[4];

    /* When MV bit is set (landscape), CASET and RASET are swapped */
    bool landscape = (lcd->orientation == ST7789_ORIENT_LANDSCAPE || lcd->orientation == ST7789_ORIENT_LANDSCAPE_INV);

    if (landscape)
    {
        /* In landscape: CASET controls Y, RASET controls X */
        /* 0x2A – CASET (now Y coordinate) */
        buf[0] = (uint8_t)(y0 >> 8);
        buf[1] = (uint8_t)(y0 & 0xFF);
        buf[2] = (uint8_t)(y1 >> 8);
        buf[3] = (uint8_t)(y1 & 0xFF);
        lcd->write_cmdN(ST7789_CMD_CASET, buf, 4);

        /* 0x2B – RASET (now X coordinate) */
        buf[0] = (uint8_t)(x0 >> 8);
        buf[1] = (uint8_t)(x0 & 0xFF);
        buf[2] = (uint8_t)(x1 >> 8);
        buf[3] = (uint8_t)(x1 & 0xFF);
        lcd->write_cmdN(ST7789_CMD_RASET, buf, 4);
    }
    else
    {
        /* In portrait: CASET controls X, RASET controls Y */
        /* 0x2A – CASET (Column Address Set) */
        buf[0] = (uint8_t)(x0 >> 8);
        buf[1] = (uint8_t)(x0 & 0xFF);
        buf[2] = (uint8_t)(x1 >> 8);
        buf[3] = (uint8_t)(x1 & 0xFF);
        lcd->write_cmdN(ST7789_CMD_CASET, buf, 4);

        /* 0x2B – RASET (Row Address Set) */
        buf[0] = (uint8_t)(y0 >> 8);
        buf[1] = (uint8_t)(y0 & 0xFF);
        buf[2] = (uint8_t)(y1 >> 8);
        buf[3] = (uint8_t)(y1 & 0xFF);
        lcd->write_cmdN(ST7789_CMD_RASET, buf, 4);
    }
}

void
st7789_fill_rect(st7789_t* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (!w || !h)
        return;

    st7789_set_window(lcd, x, y, x + w - 1, y + h - 1);
    lcd->write_cmd(ST7789_CMD_RAMWR);

    uint32_t pixels = (uint32_t)w * h;

    /* Convert RGB565 to 18-bit GBR format (3 bytes per pixel)
       RGB565: RRRRRGGGGGGBBBBB
       Extract to 8-bit: R = (color >> 11) << 3, G = ((color >> 5) & 0x3F) << 2, B = (color & 0x1F) << 3
       Send as {G, B, R} - matches working main.c code */
    uint8_t r = ((color >> 11) & 0x1F) << 3; // Red 5 bits -> 8 bits
    uint8_t g = ((color >> 5) & 0x3F) << 2;  // Green 6 bits -> 8 bits
    uint8_t b = (color & 0x1F) << 3;         // Blue 5 bits -> 8 bits

    uint8_t px[3] = {g, b, r}; // GBR order (verified working)

    /* Small stack buffer reused to avoid huge allocations */
    uint8_t buf[96]; // 32 pixels worth of GBR data
    for (unsigned i = 0; i < sizeof(buf); i += 3)
    {
        buf[i] = px[0];     // G
        buf[i + 1] = px[1]; // B
        buf[i + 2] = px[2]; // R
    }

    while (pixels)
    {
        uint32_t chunk = pixels;
        if (chunk > (sizeof(buf) / 3))
            chunk = sizeof(buf) / 3;
        lcd->write_data(buf, chunk * 3);
        pixels -= chunk;
    }
}

void
st7789_fill_rect_rgb(st7789_t* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b)
{
    if (!w || !h)
        return;

    st7789_set_window(lcd, x, y, x + w - 1, y + h - 1);
    lcd->write_cmd(ST7789_CMD_RAMWR);

    uint32_t pixels = (uint32_t)w * h;

    /* Direct RGB888 format (3 bytes per pixel) - RGB order */
    uint8_t px[3] = {r, g, b}; // RGB order

    /* Small stack buffer reused to avoid huge allocations */
    uint8_t buf[96]; // 32 pixels worth of RGB data
    for (unsigned i = 0; i < sizeof(buf); i += 3)
    {
        buf[i] = px[0];     // R
        buf[i + 1] = px[1]; // G
        buf[i + 2] = px[2]; // B
    }

    while (pixels)
    {
        uint32_t chunk = pixels;
        if (chunk > (sizeof(buf) / 3))
            chunk = sizeof(buf) / 3;
        lcd->write_data(buf, chunk * 3);
        pixels -= chunk;
    }
}

void
st7789_draw_pixels(st7789_t* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* pixels)
{
    if (!w || !h)
        return;

    st7789_set_window(lcd, x, y, x + w - 1, y + h - 1);
    lcd->write_cmd(ST7789_CMD_RAMWR);

    /* Assume RGB565 input; just push as byte stream */
    uint32_t count = (uint32_t)w * h;
    const uint8_t* p = (const uint8_t*)pixels;
    lcd->write_data(p, count * 2);
}
