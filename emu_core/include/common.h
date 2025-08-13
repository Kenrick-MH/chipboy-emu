#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

// uint_t aliases
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// int_t aliases
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// For now, address types are defined here

// GameBoy uses 16-bit addres space.
typedef uint16_t addr_t; 

/* Timing units */

/* 
    Machine cycles (M-cycles) denote the amount of CPU cycles
    elapsed. One M-cycle is 4 T-cycle.
*/
typedef uint64_t m_cycle_t;

/**
 *  Timing cycles (T-cycles) denote the actual cycles 
 *  of the clock (from one rising edge to the 
 *  next rising edge).
 */
typedef uint64_t t_cycle_t;

/*
    Common macro defs here
*/
#define MAX_DEVICE_NUMBER 20





