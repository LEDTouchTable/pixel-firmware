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
 * @file color.h
 *
 * Header containing datatypes and functionality for dealing with colors
 */

#ifndef _LTT_COLOR_H_
#define _LTT_COLOR_H_

#include <inttypes.h>

/**
* Datatype holding RGB values of a color
*
* This is a simple container for RGB colors. It combines the three color
* components into a single struct and should be used throughout the project
* whenever dealing with RGB colors.
*/
typedef struct {

    /**
     * @brief Red component of the color
     */
    uint8_t red;

    /**
     * @brief Green component of the color
     */
    uint8_t green;

    /**
     * @brief Blue component of the color
     */
    uint8_t blue;

} color_rgb_t;

#endif /* _LTT_COLOR_H_ */
