/*
 *******************************************************************************
 * Copyright © 2016 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
#ifndef __SSD1306__
#define __SSD1306__

#include <stdint.h>
#include <stdlib.h>

#define SSD1306_WIDTH   64
#define SSD1306_HEIGHT  32

void SSD1306_init();
void SSD1306_clear();
void SSD1306_chess();
void SSD1306_putchar(char c);
void SSD1306_puts(const char* str);
void SSD1306_setXY(uint8_t x, uint8_t y);

#endif // __SSD1306__
