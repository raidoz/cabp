#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h> // NULL
#include <math.h>

// error_t values - not sure if they match up with TOS
#define SUCCESS 1
#define FAIL 0
#define ERETRY 2

// TOS doesn't have true and false
#define FALSE false
#define TRUE true

typedef int error_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

#endif /*TYPES_H_*/
