#include "st7789.h"

/* MCU‑specific GPIO/SPI here */
static void hal_reset_assert(void)   { /* drive RESET low */ }
static void hal_reset_release(void)  { /* drive RESET high */ }
static void hal_delay_ms(uint32_t ms){ /* platform delay */ }

static void spi_cs_low(void)  {}
static void spi_cs_high(void) {}
static void spi_dc_cmd(void)  {}   /* DCX=0 */
static void spi_dc_data(void) {}   /* DCX=1 */
static void spi_write(const uint8_t *data, uint32_t len) {}

static void hal_write_cmd(uint8_t cmd)
{
    spi_cs_low();
    spi_dc_cmd();
    spi_write(&cmd, 1);
    spi_cs_high();
}

static void hal_write_cmd1(uint8_t cmd, uint8_t data)
{
    uint8_t buf[2] = { cmd, data };
    spi_cs_low();
    spi_dc_cmd();
    spi_write(&buf[0], 1);
    spi_dc_data();
    spi_write(&buf[1], 1);
    spi_cs_high();
}

static void hal_write_cmdN(uint8_t cmd, const uint8_t *data, uint16_t len)
{
    spi_cs_low();
    spi_dc_cmd();
    spi_write(&cmd, 1);
    if (len) {
        spi_dc_data();
        spi_write(data, len);
    }
    spi_cs_high();
}

static void hal_write_data(const uint8_t *data, uint32_t len)
{
    spi_cs_low();
    spi_dc_data();
    spi_write(data, len);
    spi_cs_high();
}

void st7789_port_init_spi(st7789_t *lcd)
{
    lcd->width       = 240;
    lcd->height      = 320;
    lcd->orientation = ST7789_ORIENT_PORTRAIT;
    lcd->color_mode  = ST7789_COLOR_MODE_RGB565;

    lcd->reset_assert   = hal_reset_assert;
    lcd->reset_release  = hal_reset_release;
    lcd->delay_ms       = hal_delay_ms;
    lcd->write_cmd      = hal_write_cmd;
    lcd->write_cmd1     = hal_write_cmd1;
    lcd->write_cmdN     = hal_write_cmdN;
    lcd->write_data     = hal_write_data;
}
