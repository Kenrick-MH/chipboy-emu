#ifndef EMULATOR_H
#define EMULATOR_H

#include <common.h>

/*  
    Defines the emulator context
*/
typedef struct emulator {   


    /**
     *  I/O Ports for emulator
     */

    /* 
        For drawing frame buffer, 
        this is linked out externally outside the
        emulator.
    */
    uint8_t *frame;
    unsigned frame_width;
    unsigned frame_height;

    /*
        Input buffer for reading input. 
    */ 
    /* !PLACEHOLDER POINTER HERE
        TODO: Replace this with an actual struct
                just for handling input.   
    */
    void *input_buf_ptr; 
    

} emulator_ctx_t


/*  
    Boots up emulator.
*/
void boot();



#endif // EMULATOR_H

