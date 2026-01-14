// st7789_port_spi.c
#include "st7789.h"

/* MCU-specific helpers */

static void
hal_reset_assert(void)
{
    //    LCD_RST_LOW();
}

static void
hal_reset_release(void)
{
    //    LCD_RST_HIGH();
}

static void
hal_delay_ms(uint32_t ms)
{
    //    HAL_Delay(ms);
}

static void
spi_cs_low(void)
{
    //    LCD_CS_LOW();
}

static void
spi_cs_high(void)
{
    //    LCD_CS_HIGH();
}

static void
spi_dc_cmd(void)
{
    //    LCD_DC_LOW();
}

static void
spi_dc_data(void)
{
    // LCD_DC_HIGH();
}

static void
spi_write(const uint8_t* data, uint32_t len)
{
    if (len == 0)
    {
        return;
    }
    // HAL_SPI_Transmit(&hspi1, (uint8_t*)data, len, HAL_MAX_DELAY);
}

/* Driver-facing callbacks */

static void
hal_write_cmd(uint8_t cmd)
{
    spi_cs_low();
    spi_dc_cmd();
    spi_write(&cmd, 1);
    spi_cs_high();
}

static void
hal_write_cmd1(uint8_t cmd, uint8_t data)
{
    uint8_t buf[2] = {cmd, data};

    spi_cs_low();
    spi_dc_cmd();
    spi_write(&buf[0], 1);
    spi_dc_data();
    spi_write(&buf[1], 1);
    spi_cs_high();
}

static void
hal_write_cmdN(uint8_t cmd, const uint8_t* data, uint16_t len)
{
    spi_cs_low();
    spi_dc_cmd();
    spi_write(&cmd, 1);

    if (len)
    {
        spi_dc_data();
        spi_write(data, len);
    }

    spi_cs_high();
}

static void
hal_write_data(const uint8_t* data, uint32_t len)
{
    spi_cs_low();
    spi_dc_data();
    spi_write(data, len);
    spi_cs_high();
}

/* Public port init */

void
st7789_port_init_spi(st7789_t* lcd)
{
    lcd->width = 240;
    lcd->height = 320;
    lcd->orientation = ST7789_ORIENT_PORTRAIT;
    lcd->color_mode = ST7789_COLOR_MODE_RGB565;

    lcd->reset_assert = hal_reset_assert;
    lcd->reset_release = hal_reset_release;
    lcd->delay_ms = hal_delay_ms;

    lcd->write_cmd = hal_write_cmd;
    lcd->write_cmd1 = hal_write_cmd1;
    lcd->write_cmdN = hal_write_cmdN;
    lcd->write_data = hal_write_data;
}
