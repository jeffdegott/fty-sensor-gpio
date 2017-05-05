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

/*
@header
    libgpio - General Purpose Input/Output lib
@discuss
@end
*/

#include "fty_sensor_gpio_classes.h"

//  Structure of our class

struct _libgpio_t {
    int filler;     //  Declare class properties here
};

//  Private functions forward declarations

static int libgpio_export(int pin);
static int libgpio_unexport(int pin);
static int libgpio_set_direction(int pin, int dir);


//  --------------------------------------------------------------------------
//  Create a new libgpio

libgpio_t *
libgpio_new (void)
{
    libgpio_t *self = (libgpio_t *) zmalloc (sizeof (libgpio_t));
    assert (self);
    //  Initialize class properties here
    return self;
}

//  --------------------------------------------------------------------------
//  Read a GPI or GPO status
int
libgpio_read (libgpio_t **self_p, int pin)
{
    char path[GPIO_VALUE_MAX];
    char value_str[3];
    int fd;
    int direction = GPIO_DIRECTION_IN; // FIXME: autodetection for GPIO_DIRECTION_OUT

    // GPI pin has -1 offset, i.e. GPI 1 is pin 0
    pin--;

    // Enable the desired GPIO
    if (libgpio_export(pin) == -1)
        return -1;

    // Set its direction
    if (libgpio_set_direction(pin, direction) == -1)
        return -1;

    snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin + GPIO_BASE_INDEX);
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        return -1;
    }

    if (read(fd, value_str, 3) == -1) {
        fprintf(stderr, "Failed to read value!\n");
        close(fd);
        return -1;
    }

    close(fd);

    if (libgpio_unexport(pin) == -1) {
        return -1;
    }

    return(atoi(value_str));
}
//  --------------------------------------------------------------------------
//  Write a GPO (to enable or disable it)
int
libgpio_write (libgpio_t **self_p, int pin, int value)
{
    static const char s_values_str[] = "01";
    char path[GPIO_VALUE_MAX];
    int fd;
    int retval = 0;

    snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin + GPIO_BASE_INDEX);
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        return(-1);
    }

    if (write(fd, &s_values_str[GPIO_STATUS_CLOSED == value ? 0 : 1], 1) != 1) {
        fprintf(stderr, "Failed to write value!\n");
        retval = -1;
    }

    close(fd);
    return retval;
}


//  --------------------------------------------------------------------------
//  Get the textual name for a status
string
libgpio_get_status_string (libgpio_t **self_p, int value)
{
    string status_str;

    switch (value) {
        case GPIO_STATUS_CLOSED:
            status_str = "closed";
            break;
        case GPIO_STATUS_OPENED:
            status_str = "opened";
            break;
        case GPIO_STATUS_UNKNOWN:
        default:
            status_str = "";
    }
    return status_str;
}

//  --------------------------------------------------------------------------
//  Destroy the libgpio

void
libgpio_destroy (libgpio_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        libgpio_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
libgpio_test (bool verbose)
{
    printf (" * libgpio: ");

    //  @selftest
    //  Simple create/destroy test

    // Note: If your selftest reads SCMed fixture data, please keep it in
    // src/selftest-ro; if your test creates filesystem objects, please
    // do so under src/selftest-rw. They are defined below along with a
    // usecase for the variables (assert) to make compilers happy.
    const char *SELFTEST_DIR_RO = "src/selftest-ro";
    const char *SELFTEST_DIR_RW = "src/selftest-rw";
    assert (SELFTEST_DIR_RO);
    assert (SELFTEST_DIR_RW);
    // Uncomment these to use C++ strings in C++ selftest code:
    //std::string str_SELFTEST_DIR_RO = std::string(SELFTEST_DIR_RO);
    //std::string str_SELFTEST_DIR_RW = std::string(SELFTEST_DIR_RW);
    //assert ( (str_SELFTEST_DIR_RO != "") );
    //assert ( (str_SELFTEST_DIR_RW != "") );
    // NOTE that for "char*" context you need (str_SELFTEST_DIR_RO + "/myfilename").c_str()

    libgpio_t *self = libgpio_new ();
    assert (self);
    libgpio_destroy (&self);
    //  @end
    printf ("OK\n");
}

//  --------------------------------------------------------------------------
//  Private functions

//  --------------------------------------------------------------------------
//  Set the current GPIO pin to act on

int libgpio_export(int pin)
{
    char buffer[GPIO_BUFFER_MAX];
    ssize_t bytes_written;
    int fd;
    int retval = 0;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return -1;
    }

    bytes_written = snprintf(buffer, GPIO_BUFFER_MAX, "%d", pin + GPIO_BASE_INDEX);
    if (write(fd, buffer, bytes_written) < bytes_written) {
        retval = -1;
    }

    close(fd);
    return retval;
}

//  --------------------------------------------------------------------------
//  Unset the current GPIO pin to act on

int libgpio_unexport(int pin)
{
    char buffer[GPIO_BUFFER_MAX];
    ssize_t bytes_written;
    int fd;
    int retval = 0;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
      fprintf(stderr, "Failed to open unexport for writing!\n");
      return -1;
    }

    bytes_written = snprintf(buffer, GPIO_BUFFER_MAX, "%d", pin + GPIO_BASE_INDEX);
    if (write(fd, buffer, bytes_written) < bytes_written) {
        retval = -1;
    }

    close(fd);
    return retval;
}


//  --------------------------------------------------------------------------
//  Set the current GPIO direction to 'in' (read) or 'out' (write)

int libgpio_set_direction(int pin, int direction)
{
    static const char s_directions_str[]  = "in\0out";
    int retval = 0;

    char path[GPIO_DIRECTION_MAX];
    int fd;

    snprintf(path, GPIO_DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin + GPIO_BASE_INDEX);
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return -1;
    }

    if (write(fd, &s_directions_str[GPIO_DIRECTION_IN == direction ? 0 : 3],
      GPIO_DIRECTION_IN == direction ? 2 : 3) == -1) {
        fprintf(stderr, "Failed to set direction!\n");
        retval = -1;
    }

    close(fd);
    return retval;
}
