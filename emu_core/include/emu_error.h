#ifndef ERROR_H
#define ERROR_H

typedef enum error_code {
    STATUS_OK = 0,
    STATUS_BUS_ERROR,
    STATUS_SEG_FAULT,
    STATUS_EMPTY_CONTAINER,
} error_code_t;




#endif // ERROR_H