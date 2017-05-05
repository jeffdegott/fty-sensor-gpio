/*  =========================================================================
    libgpio - General Purpose Input/Output lib

    Copyright (C) 2014 - 2017 Eaton                                        
                                                                           
    This program is free software; you can redistribute it and/or modify   
    it under the terms of the GNU General Public License as published by   
    the Free Software Foundation; either version 2 of the License, or      
    (at your option) any later version.                                    
                                                                           
    This program is distributed in the hope that it will be useful,        
    but WITHOUT ANY WARRANTY; without even the implied warranty of         
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
    GNU General Public License for more details.                           
                                                                           
    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.            
    =========================================================================
*/

#ifndef LIBGPIO_H_INCLUDED
#define LIBGPIO_H_INCLUDED

// Target at gpiochip488
#define GPIO_BASE_INDEX    488

// Directions
#define GPIO_DIRECTION_IN    0
#define GPIO_DIRECTION_OUT   1

// GPIO Status value
#define GPIO_STATUS_UNKNOWN -1
#define GPIO_STATUS_CLOSED   0
#define GPIO_STATUS_OPENED   1

// Defines
#define GPIO_BUFFER_MAX      4
#define GPIO_DIRECTION_MAX  35
#define GPIO_VALUE_MAX      30
#define GPI_MAX_NUM         10


#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new libgpio
FTY_SENSOR_GPIO_EXPORT libgpio_t *
    libgpio_new (void);

//  @interface
//  Read a GPI or GPO status
FTY_SENSOR_GPIO_EXPORT int
    libgpio_read (libgpio_t **self_p, int pin);

//  @interface
//  Write a GPO (to enable or disable it)
FTY_SENSOR_GPIO_EXPORT int
    libgpio_write (libgpio_t **self_p, int pin, int value);

//  @interface
//  Get the textual name for a status
FTY_SENSOR_GPIO_EXPORT string
    libgpio_get_status_string (libgpio_t **self_p, int value);

//  Destroy the libgpio
FTY_SENSOR_GPIO_EXPORT void
    libgpio_destroy (libgpio_t **self_p);

//  Self test of this class
FTY_SENSOR_GPIO_EXPORT void
    libgpio_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif