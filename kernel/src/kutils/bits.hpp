#pragma once

#define set_bit(var, pos)	((var) | 1 << pos)
#define clear_bit(var, pos) ((var) & ~(1 << pos))
#define test_bit(var, pos)	((var) & (1 << pos))