/*
 * Copyright (C) 2014 Karol Babioch <karol@babioch.de>
 *
 * This file is part of LEDTouchTable.
 *
 * LEDTouchTable is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LEDTouchTable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LEDTouchTable. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file pwm.h
 *
 * Functionality for manipulating the PWM signal in control of the color
 *
 * This module provides means to modify the PWM signal output and hence the
 * color of the attached LED.
 *
 * @see pwm.c
 */

#ifndef _LTT_PWM_H_
#define _LTT_PWM_H_

#include "color.h"

void pwm_init();

void pwm_enable();
void pwm_disable();

void pwm_set_color_rgb(color_rgb_t* color);
const color_rgb_t* pwm_get_color_rgb();

#endif /* _LTT_PWM_H_ */
