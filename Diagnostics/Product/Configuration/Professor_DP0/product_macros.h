#ifndef PRODUCT_MACROS_H
#define PRODUCT_MACROS_H

/* Task Runtime analysis will measure the time spent in each task. It hooks into the
*  FreeRTOS macros. Clearly enabling this will modifiy system timing.
*/
#define ANALYZE_TASK_RUNTIMES

#endif //PRODUCT_MACROS_H