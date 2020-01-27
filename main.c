#include "button.h"
#include "nios2_control.h"
#include "timer.h"

#define SWITCHES ((volatile unsigned int*) 0x10000040)
#define NUM_WORDS 3

unsigned int done_flag = 0;
unsigned int msg_flag = 0; 
char* msg_ptr;

int init(void) {
    unsigned int timer_hi, timer_lo;

    // Set up button interrupts
    *BUTTON_EDGE = 0x2;
    *BUTTON_MASK = 0x2;

    // Read switch information
    unsigned int switch_state = *SWITCHES;
    switch_state = switch_state & 0x3;

    switch (switch_state) {
    case 0x3:
        // set to 0.25 sec
        timer_lo = 0xBC20;
        timer_hi = 0x00BE;
        break;
    case 0x2:
        // set to 0.5 sec
        timer_lo = 0x7840;
        timer_hi = 0x017D;
        break;
    case 0x1:
        // set to 1 sec
        timer_lo = 0xF080;
        timer_hi = 0x02FA;
        break;
    case 0x0:
        // set to 2 sec
        timer_lo = 0xE100;
        timer_hi = 0x05F5;
    default:
        // set to 1 sec
        timer_lo = 0xF080;
        timer_hi = 0x02FA;
        break;
    }

    *TIMER_STATUS = 0;
    *TIMER_START_HI = timer_hi;
    *TIMER_START_LO = timer_lo;
    *TIMER_CONTROL = 0x7;

    NIOS2_WRITE_IENABLE(0x3);
    NIOS2_WRITE_STATUS(1);
}

int main(void) {
    init();
    unsigned int idx = 0;
    char words[NUM_WORDS][10] = { "123", "456789", "123" };
    msg_ptr = words[idx];
    msg_flag = 1;

    PrintString(msg_ptr);
	PrintChar('\n');

    while (1) {
        if (done_flag == 1) {
            done_flag = 0;

            if (idx == NUM_WORDS) {
                idx = 0;
            } else {
                ++idx;
            }

            msg_ptr = words[idx];
            msg_flag = 1;
            PrintString(msg_ptr);
            PrintChar('\n');
        }
    }

    return 0;
}
