#include <platform/error_handling.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>

noreturn void emu_die(error_code_t code, const char *msg)
{
    printf(
        "A fatal error encountered!\n"
        "Error Code: %d\n"
        "Code Description: %s\n"
        "Message: %s\n",
        (int) code, "", msg);
    exit(code);
}