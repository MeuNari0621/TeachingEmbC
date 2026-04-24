#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stdint.h>

typedef uint8_t Std_ReturnType;
typedef uint8_t boolean;

#ifndef TRUE
#define TRUE 1u
#endif

#ifndef FALSE
#define FALSE 0u
#endif

#ifndef E_OK
#define E_OK 0u
#endif

#ifndef E_NOT_OK
#define E_NOT_OK 1u
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

#endif /* STD_TYPES_H */
