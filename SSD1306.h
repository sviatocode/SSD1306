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

// Here you must include HAL I2C headers
#include "i2c.h"

#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  64

typedef enum Color_enum {BLACK = 0, WHITE = 1, INV = 2} color_t;

void SSD1306_init(I2C_HandleTypeDef* hi2c);
void SSD1306_clear();
void SSD1306_update(I2C_HandleTypeDef* hi2c);
void SSD1306_setPixel(size_t x, size_t y, color_t color);

#endif // __SSD1306__
