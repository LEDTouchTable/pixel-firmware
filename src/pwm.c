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
 * @file pwm.c
 *
 * Implements the PWM functionality declared in pwm.h
 *
 * Each channel (red, green, blue) is controlled separately and is attached
 * to a different output compare pin of the microcontroller. As each timer has
 * only two output compare pins, two timers (1 and 2) are being used
 * (red = `OC1A`, green = `OC1B`, blue = `OC2A`). These timers are 16-bit wide,
 * allowing for an accurate brightness control.
 *
 * A {@link #pwm_table table} with precomputed PWM compare values is being used
 * as a basis for a seemingly linear brightness progression. There are exactly
 * 256 steps (= 8 bits), making it trivial to output any {@link color_rgb_t RGB
 * color}.
 *
 * @see pwm.h
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "color.h"
#include "pwm.h"

/**
 * Table containing precomputed values used to generate PWM signals
 *
 * This table contains 256 values, each representing a single step. The values
 * are computed in such a way that the human eye perceives the increments as
 * linear progression. This concept and the values itself are described at [1].
 *
 * [1]: https://www.mikrocontroller.net/articles/LED-Fading
 */
static const uint16_t PROGMEM pwm_table[256] = {

    0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7,
    7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15,
    15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    31, 32, 33, 35, 36, 38, 40, 41, 43, 45, 47, 49, 52, 54, 56, 59,
    61, 64, 67, 70, 73, 76, 79, 83, 87, 91, 95, 99, 103, 108, 112,
    117, 123, 128, 134, 140, 146, 152, 159, 166, 173, 181, 189, 197,
    206, 215, 225, 235, 245, 256, 267, 279, 292, 304, 318, 332, 347,
    362, 378, 395, 412, 431, 450, 470, 490, 512, 535, 558, 583, 609,
    636, 664, 693, 724, 756, 790, 825, 861, 899, 939, 981, 1024, 1069,
    1117, 1166, 1218, 1272, 1328, 1387, 1448, 1512, 1579, 1649, 1722,
    1798, 1878, 1961, 2048, 2139, 2233, 2332, 2435, 2543, 2656, 2773,
    2896, 3025, 3158, 3298, 3444, 3597, 3756, 3922, 4096, 4277, 4467,
    4664, 4871, 5087, 5312, 5547, 5793, 6049, 6317, 6596, 6889, 7194,
    7512, 7845, 8192, 8555, 8933, 9329, 9742, 10173, 10624, 11094,
    11585, 12098, 12634, 13193, 13777, 14387, 15024, 15689, 16384,
    17109, 17867, 18658, 19484, 20346, 21247, 22188, 23170, 24196,
    25267, 26386, 27554, 28774, 30048, 31378, 32768, 34218, 35733,
    37315, 38967, 40693, 42494, 44376, 46340, 48392, 50534, 52772,
    55108, 57548, 60096, 62757, 65535,

};

/**
 * Color currently being output
 *
 * This is used to keep track of the color that is currently being output. It
 * is explicitly initialized with zero values, as the appropriate compare
 * registers will be reset during {@link #pwm_init() initialization}.
 *
 * @see pwm_set_color_rgb()
 * @see pwm_get_color_rgb()
 */
color_rgb_t pwm_color_rgb = {0, 0, 0};

/**
 * Initializes the PWM module
 *
 * This sets up the involved timers, so that the PWM signals are generated. It
 * needs to be called once before the module and its functionality can be used.
 *
 * @note The phase-correct mode is used, so the resulting PWM signal will be
 * free of any glitches and extreme cases (0 and TOP) are handled correctly.
 *
 * @note No signals are actually being output until pwm_enable() is invoked.
 *
 * @note It is assumed that interrupts are disabled when this function is
 * invoked. Calling it with interrupts enabled may have unintended side
 * effects, as the involved timers no longer will run synchronously.
 */
void pwm_init()
{

    // Disable LEDs (active low)
    PORTA |= _BV(PA6) | _BV(PA5) | _BV(PA4);

    // Define PWM pins as output
    DDRA |= _BV(PA6) | _BV(PA5) | _BV(PA4);

    // Reset compare values
    OCR1A = 0;
    OCR1B = 0;
    OCR2A = 0;

    // Setup TOCC channels
    TOCPMSA0 = _BV(TOCC3S1);
    TOCPMSA1 = _BV(TOCC4S0) | _BV(TOCC5S0);

    // Enable TOCC channels
    TOCPMCOE = _BV(TOCC5OE) | _BV(TOCC4OE) | _BV(TOCC3OE);

    // Setup timers
    // Mode 10, Phase correct PWM, Top ICRn, prescaler 1

    ICR1 = UINT16_MAX;
    TCCR1A = _BV(WGM11);
    TCCR1B = _BV(WGM13) | _BV(CS10);

    ICR2 = UINT16_MAX;
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(WGM23) | _BV(CS20);

}

/**
 * Turns on the output of the PWM signals
 *
 * Once the module has been initialized, this can be used to enable the actual
 * output of the PWM signal to the appropriate pins.
 *
 * @note To make sure that the timers output their signals synchronously,
 * interrupts are shortly disabled while changing the timer settings.
 *
 * @see pwm_disable()
 */
void pwm_enable()
{

    // Save global interrupt flag and disable interrupts
    uint8_t tmp = SREG;
    cli();

    // Disable pins
    PORTA |= _BV(PA6) | _BV(PA5) | _BV(PA4);

    // Set OCnA/OCnB on Compare Match when up-counting,
    // clear OCnA/OCnB on Compare Match when downcounting
    TCCR1A |= _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0);
    TCCR2A |= _BV(COM2A1) | _BV(COM2A0);

    // Restore global interrupt flag
    SREG = tmp;

}

/**
 * Turns off the output of the PWM signals
 *
 * This can be used to disable the actual output of the PWM signal. Furthermore
 * the pins are set to a defined level.
 *
 * @note Note, however, that the timers will continue to run.
 *
 * @note To make sure that the timers output their signals synchronously,
 * interrupts are shortly disabled while changing the timer settings.
 *
 * @see pwm_enable()
 */
void pwm_disable()
{

    // Save global interrupt flag and disable interrupts
    uint8_t tmp = SREG;
    cli();

    // Normal port operation, OCnA/OCnB disconnected
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
    TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));

    // Disable pins
    PORTA |= _BV(PA6) | _BV(PA5) | _BV(PA4);

    // Restore global interrupt flag
    SREG = tmp;

}

/**
 * Applies the given compare values to the timers
 *
 * This applies the given values directly to the timers by writing them to the
 * appropriate output compare registers.
 *
 * @note To make sure that the timers output their signals synchronously,
 * interrupts are shortly disabled while changing the timer settings.
 *
 * @param red Compare value for the red channel
 * @param green Compare value for the green channel
 * @param blue Compare value for the blue channel
 */
static void pwm_set_compare_values(uint16_t red, uint16_t green, uint16_t blue)
{

    // Save global interrupt flag and disable interrupts
    uint8_t tmp = SREG;
    cli();

    // Assign compare values
    OCR1A = red;
    OCR1B = green;
    OCR2A = blue;

    // Restore global interrupt flag
    SREG = tmp;

}

/**
 * Sets up the timers to output a signal corresponding to the given RGB color
 *
 * This function reads in the compare values for the given arguments from the
 * precomputed {@link #pwm_table table} and applies them to the involved
 * timers. As the components of a {@link color_rgb_t RGB colors} are expected
 * to be eight bits each, and there are 256 values in the table, no further
 * transformations are needed.
 *
 * @param color Color that PWM signal should be output for
 *
 * @see pwm_table
 * @see pwm_color_rgb
 * @see pwm_set_compare_values()
 */
void pwm_set_color_rgb(color_rgb_t* color)
{

    // Save color
    pwm_color_rgb = *color;

    // Get PWM compare values for each channel separately
    uint16_t red_value = pgm_read_word(&(pwm_table[color->red]));
    uint16_t green_value = pgm_read_word(&(pwm_table[color->green]));
    uint16_t blue_value = pgm_read_word(&(pwm_table[color->blue]));

    // Apply PWM compare values
    pwm_set_compare_values(red_value, green_value, blue_value);

}

/**
 * Returns the color currently being output
 *
 * @return Pointer to {@link color_rgb_t color} currently being output
 *
 * @see pwm_color_rgb
 */
const color_rgb_t* pwm_get_color_rgb()
{

    return &pwm_color_rgb;

}
