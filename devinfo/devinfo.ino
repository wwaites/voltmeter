/*
 * Copyleft 2012 William Waites <wwaites@tardis.ed.ac.uk>
 * This program is Free Software. You may use it according
 * to the terms of the GNU GPL Version 3 or at your option
 * any later version.
 *
 */

#include <stdio.h>
#include "stdserial.h"

void setup() {
    tty_init();
}

void devinfo() {
    printf("\r\nArduino UNO\r\n\r\n");
    printf("sizeof(int) = %d\r\n", sizeof(int));
    printf("sizeof(long) = %d\r\n", sizeof(long));
    printf("sizeof(long long) = %d\r\n", sizeof(long long));
    printf("sizeof(float) = %d\r\n", sizeof(float));
    float pi = 3.1459;
    int ipi = (int)pi * 100 + (int)(pi * 100) % 100;
    printf("PI = %d.%d\r\n", ipi / 100, ipi % 100);
}

/* Main loop */
void loop() {
    devinfo();
    delay(5000);
}
