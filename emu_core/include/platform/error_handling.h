#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdnoreturn.h>
#include <emu_error.h>

void emu_die(error_code_t code, const char *msg);

#endif
