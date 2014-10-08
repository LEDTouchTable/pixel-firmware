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
 * @file main.c
 *
 * Main entry point of the firmware
 *
 * This file glues together all of the other modules of the project and
 * represents the main entry point, where the execution will be started.
 */

#include "pwm.h"

/**
* @brief Main entry point to start execution at
*
* This is the main entry point where execution will start. It initializes the
* hardware and enters an infinite loop handling any upcoming events not yet
* covered.
*
* @note This function makes use of the attribute `OS_main`. For details refer
* to [1].
*
* [1]: http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
*
* @return Should never return anything
*/
__attribute__((OS_main)) int main(int argc, char* argv[])
{

    pwm_init();

    while(1);

}
