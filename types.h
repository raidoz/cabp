#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h> // NULL
#include <stdint.h>
#include <math.h>

// error_t values - not sure if they match up with TOS
#define SUCCESS 1
#define FAIL 0
#define ERETRY 2

// TOS doesn't have true and false
#define FALSE false
#define TRUE true

typedef int error_t;

#endif /*TYPES_H_*/
