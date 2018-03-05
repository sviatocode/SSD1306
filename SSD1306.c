/*
 *******************************************************************************
 * Copyright © 2018 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
/* Includes */
#include "SSD1306.h"
#include <string.h>
#include "../c-soft-i2c/src/ul_i2c.h"

#define SSD1306_ADDR                                    0x78
#define SSD1306_COMMAND                                 0x00
#define SSD1306_DATA                                    0xC0
#define SSD1306_DATA_CONTINUE                           0x40

// SSD1306 Commandset
// ------------------
// Fundamental Commands
#define SSD1306_SET_CONTRAST_CONTROL                    0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME                   0xA4
#define SSD1306_DISPLAY_ALL_ON                          0xA5
#define SSD1306_NORMAL_DISPLAY                          0xA6
#define SSD1306_INVERT_DISPLAY                          0xA7
#define SSD1306_DISPLAY_OFF                             0xAE
#define SSD1306_DISPLAY_ON                              0xAF
#define SSD1306_NOP                                     0xE3
// Scrolling Commands
#define SSD1306_HORIZONTAL_SCROLL_RIGHT                 0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT                  0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT    0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT     0x2A
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
// Addressing Setting Commands
#define SSD1306_SET_LOWER_COLUMN                        0x00
#define SSD1306_SET_HIGHER_COLUMN                       0x10
#define SSD1306_MEMORY_ADDR_MODE                        0x20
#define SSD1306_SET_COLUMN_ADDR                         0x21
#define SSD1306_SET_PAGE_ADDR                           0x22
// Hardware Configuration Commands
#define SSD1306_SET_START_LINE                          0x40
#define SSD1306_SET_SEGMENT_REMAP                       0xA0
#define SSD1306_SET_MULTIPLEX_RATIO                     0xA8
#define SSD1306_COM_SCAN_DIR_INC                        0xC0
#define SSD1306_COM_SCAN_DIR_DEC                        0xC8
#define SSD1306_SET_DISPLAY_OFFSET                      0xD3
#define SSD1306_SET_COM_PINS                            0xDA
#define SSD1306_CHARGE_PUMP                             0x8D
// Timing & Driving Scheme Setting Commands
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO             0xD5
#define SSD1306_SET_PRECHARGE_PERIOD                    0xD9
#define SSD1306_SET_VCOM_DESELECT                       0xDB

#define SYM_HEIGHT 8
#define SYM_WIDTH  6

#define LINE_WIDTH  ((SSD1306_WIDTH)/(SYM_WIDTH))
#define LINE_OFFSET (((SSD1306_WIDTH)%(SYM_WIDTH))/2)

#define FONT_CHARS 10

#include <avr/eeprom.h>

uint8_t EEMEM font[FONT_CHARS*SYM_WIDTH] = {
        0b00000000,
        0b00111100,
        0b01010010,
        0b01001010,
        0b00111100,
        0b00000000,

        0b00000000,
        0b01000100,
        0b01111110,
        0b01000000,
        0b00000000,
        0b00000000,

        0b00000000,
        0b01000100,
        0b01100010,
        0b01010010,
        0b01001100,
        0b00000000,

        0b00000000,
        0b00100100,
        0b01000010,
        0b01001010,
        0b00110100,
        0b00000000,

        0b00000000,
        0b00011000,
        0b00010100,
        0b01111110,
        0b00010000,
        0b00000000,

        0b00000000,
        0b00101110,
        0b01001010,
        0b01001010,
        0b00110010,
        0b00000000,

        0b00000000,
        0b00111100,
        0b01001010,
        0b01001010,
        0b00110000,
        0b00000000,

        0b00000000,
        0b00000010,
        0b01110010,
        0b00001010,
        0b00000110,
        0b00000000,

        0b00000000,
        0b00110100,
        0b01001010,
        0b01001010,
        0b00110100,
        0b00000000,

        0b00000000,
        0b00001100,
        0b01010010,
        0b01010010,
        0b00111100,
        0b00000000,
};

static uint8_t cursor = 0;

static void SSD1306_sendCmd(uint8_t cmd)
{
    uint8_t data[1];
    data[0] = cmd;
    i2c_wr_a8_buf8(SSD1306_ADDR, SSD1306_COMMAND, data, 1);
}

static void SSD1306_setPage(uint8_t add)
{
    /*add+=32;
    i2c_start();
    i2c_wr_U8(SSD1306_ADDR);
    i2c_wr_U8(0x80);
    add=0xb0|add;
    i2c_wr_U8(add);*/
    SSD1306_sendCmd(0xB0|(32+add));
}

static void SSD1306_setColumn(uint8_t add)
{
    /*add+=32;
    i2c_start();
    i2c_wr_U8(SSD1306_ADDR);
    i2c_wr_U8(0x80);
    i2c_wr_U8((0x10|(add>>4)));
    i2c_wr_U8(0x80);
    i2c_wr_U8((0x0f&add));
    i2c_stop();*/
    SSD1306_sendCmd(0x10|((32+add)>>4));
    SSD1306_sendCmd(0x0F&(32+add));
}

void SSD1306_init()
{
    SSD1306_sendCmd(SSD1306_DISPLAY_OFF);

    SSD1306_sendCmd(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
    SSD1306_sendCmd(0x80);

    SSD1306_sendCmd(SSD1306_SET_MULTIPLEX_RATIO);
    SSD1306_sendCmd(0x1F);

    SSD1306_sendCmd(SSD1306_SET_DISPLAY_OFFSET);
    SSD1306_sendCmd(0x0);

    //SSD1306_sendCmd(SSD1306_SET_START_LINE | 0x0);

    SSD1306_sendCmd(SSD1306_CHARGE_PUMP);
    SSD1306_sendCmd(0x14);

    //SSD1306_sendCmd(SSD1306_MEMORY_ADDR_MODE);
    //SSD1306_sendCmd(0x00);

    SSD1306_sendCmd(SSD1306_SET_START_LINE);

    SSD1306_sendCmd(SSD1306_NORMAL_DISPLAY);

    SSD1306_sendCmd(SSD1306_DISPLAY_ALL_ON_RESUME);

    SSD1306_sendCmd(SSD1306_SET_SEGMENT_REMAP | 0x1);

    SSD1306_sendCmd(SSD1306_COM_SCAN_DIR_DEC);

    SSD1306_sendCmd(SSD1306_SET_COM_PINS);
    SSD1306_sendCmd(0x12);

    SSD1306_sendCmd(SSD1306_SET_CONTRAST_CONTROL);
    SSD1306_sendCmd(0x8F);

    SSD1306_sendCmd(SSD1306_SET_PRECHARGE_PERIOD);
    SSD1306_sendCmd(0xF1);

    SSD1306_sendCmd(SSD1306_SET_VCOM_DESELECT);
    SSD1306_sendCmd(0x40);

    //SSD1306_sendCmd(SSD1306_DISPLAY_ALL_ON_RESUME);

    //SSD1306_sendCmd(SSD1306_NORMAL_DISPLAY);

    SSD1306_sendCmd(SSD1306_DISPLAY_ON);
}

void SSD1306_clear()
{
    for (uint8_t i = 0; i < SSD1306_HEIGHT/8; i++)
    {
        SSD1306_setPage(i);
        SSD1306_setColumn(0);

        i2c_start();
        i2c_wr_U8(SSD1306_ADDR);
        i2c_wr_U8(SSD1306_DATA_CONTINUE);
        for(uint8_t j=0; j<SSD1306_WIDTH; j++)
            i2c_wr_U8(0x00);
        i2c_stop();
    }
}

void SSD1306_chess()
{
    /*SSD1306_sendCmd(SSD1306_SET_COLUMN_ADDR);
    SSD1306_sendCmd(0);
    SSD1306_sendCmd(SSD1306_WIDTH-1);

    //i2c_wr_a8_buf8(SSD1306_ADDR, SSD1306_DATA_CONTINUE, 0, 1);
    i2c_start();
    i2c_wr_U8(SSD1306_ADDR);
    i2c_wr_U8(SSD1306_DATA_CONTINUE);
    uint16_t buf_size = (SSD1306_WIDTH/8)*SSD1306_HEIGHT;
    while (buf_size--)
        i2c_wr_U8(0);
    i2c_stop();*/

    unsigned char i,j,k;
    for(i=0;i<0x01;i++)
    {
        uint8_t page = i, column = 0;

        //SSD1306_setPage(i);
        SSD1306_sendCmd(0xB0|(32+page));

        //SSD1306_setColumn(0x00);
        SSD1306_sendCmd(0x10|((32+column)>>4));
        SSD1306_sendCmd(0x0F&(32+column));

        i2c_start();
        i2c_wr_U8(SSD1306_ADDR);
        i2c_wr_U8(SSD1306_DATA_CONTINUE);
        for(j=0;j<0x08;j++)
        {
            for(k=0;k<0x04;k++)
                i2c_wr_U8(0x0F);
            for(k=0;k<0x04;k++)
                i2c_wr_U8(~0x0F);
        }
        i2c_stop();
    }
}

void SSD1306_putchar(char c)
{
    uint8_t page = cursor/LINE_WIDTH;
    uint8_t column = (cursor%LINE_WIDTH)*SYM_WIDTH + LINE_OFFSET;
    uint8_t index = c - '0';
    uint8_t character[SYM_WIDTH];

    // Check borders
    if (page >= SSD1306_HEIGHT/8) return;

    if (c == ':')
    {
        eeprom_read_block(character, &font[5], SYM_WIDTH);
        character[3] = 0;
        character[4] = 0;
    }
    else
    {
        if (index >= FONT_CHARS) return;
        eeprom_read_block(character, &font[index*SYM_WIDTH], SYM_WIDTH);
    }

    SSD1306_setPage(page);
    SSD1306_setColumn(column);

    i2c_wr_a8_buf8(SSD1306_ADDR, SSD1306_DATA_CONTINUE, character, SYM_WIDTH);

    cursor++;
}

void SSD1306_puts(const char* str)
{
    while (*str != '\0')
        SSD1306_putchar(*str++);
}

void SSD1306_setXY(uint8_t x, uint8_t y)
{
    cursor = x + y * LINE_WIDTH;
}
