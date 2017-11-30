#ifndef PRODUCT_MACROS_H
#define PRODUCT_MACROS_H

/* Task Runtime analysis will measure the time spent in each task. It hooks into the
*  FreeRTOS macros. Clearly enabling this will modifiy system timing.
*/
#define ANALYZE_TASK_RUNTIMES

/* The top level registers' I2C address for the Sii9437 can either be 0x60 or 0x62
 * based on the voltage given to GPIO0 on boot of the chip (0x60 Low / 0x62 High).
*/
#define SII9437_TOP_REGISTER_ADDR 0x62

#endif //PRODUCT_MACROS_H