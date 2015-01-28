#ifndef UART_H
#define UART_H

#include <stdint.h>

//Returns a file descriptor which can be used with read()
int32_t uart_init();

#endif
