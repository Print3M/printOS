/*
    Universal PS/2 driver.
    
    https://wiki.osdev.org/PS/2
    https://wiki.osdev.org/PS/2_Keyboard
    
*/


#pragma once
#include <libc/stdint.hpp>


namespace ps2 {



}; // namespace ps2

namespace ps2::controller {

u8 test();

}

namespace ps2::port_1 {

u8 test();
void disable();
void enable();

} // namespace ps2::port_1

namespace ps2::port_2 {

u8 test();
void disable();
void enable();

} // namespace ps2::port_2