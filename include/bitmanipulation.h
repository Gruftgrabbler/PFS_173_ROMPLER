#ifndef EASYPDKTEST_BITMANIPULATION_H
#define EASYPDKTEST_BITMANIPULATION_H

// Bit manipulation macros
#define set_bit(var, bit) ((var) |= (1 << (bit)))
#define clear_bit(var, bit) ((var) &= (uint8_t)~(1 << (bit)))
#define toggle_bit(var,bit) ((var) ^= (1 << (bit)))

#define bit_is_set(var, bit) ((var) & (1 << (bit)))
#define bit_is_clear(var, bit) !bit_is_set(var, bit)

#endif //EASYPDKTEST_BITMANIPULATION_H
