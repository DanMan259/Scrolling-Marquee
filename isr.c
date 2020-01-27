#include "button.h"
#include "nios2_control.h"
#include "timer.h"

#define HEX_DATA ((volatile unsigned int*) 0x10000020)

extern unsigned int done_flag;
extern unsigned int msg_flag ; 
extern char* msg_ptr;

unsigned int current_index = 0;
unsigned int length = 0;
unsigned int pause_flag = 0;
unsigned int scrolling_active = 0;
char output_buf[256];

extern unsigned int ascii_hex_table[96];
// unsigned int ascii_hex_table[] = {
//     0x3F,  0x06,  0x5B,  0x4F,
//     0x66,  0x6D,  0x7D,  0x07,
//     0x7F,  0x6F,  0x00,  0x00,
//     0x00,  0x00,  0x00,  0x00
// };

void update_hex(unsigned int index) {
    unsigned int data_to_display = ascii_hex_table[output_buf[index] - ' '] << 24
        | ascii_hex_table[output_buf[index + 1] - ' '] << 16
        | ascii_hex_table[output_buf[index + 2] - ' '] << 8
        | ascii_hex_table[output_buf[index + 3] - ' '];
    
    *HEX_DATA = data_to_display;
}   

void initiateScrolling(void) {
    unsigned int i;
    unsigned int j;
    for (i = 0; i < 4; ++i) {
        output_buf[i] = ' ';
    }

    i = 0;
    j = 4;
    while (msg_ptr[i] != '\0') {
        output_buf[j] = msg_ptr[i]; // Copy original message
    }

    for (i = j; i < j + 4; ++i) {
        output_buf[i] = ' ';
    }

    length = j + 4;
    current_index = 0;
    scrolling_active = 1;
    update_hex(current_index);
}

void continueScrolling(void) {
    ++current_index;
    if (current_index < length - 3) {
        update_hex(current_index);
    } else {
        // No characters to display
        scrolling_active = 0;
        done_flag = 1;
    }
}

void togglePause(void) {
    pause_flag = pause_flag ^ 1;
    *HEX_DATA = *HEX_DATA ^ 0x80808080;
}

void interrupt_handler(void) {
    unsigned int ipending = NIOS2_READ_IPENDING();

    if (ipending & 0x1) {
        *TIMER_STATUS = 0;  // Clear the interrupt
        
        if (scrolling_active == 0 && msg_flag == 1) {
            initiateScrolling();
            msg_flag = 0;
        } else if (scrolling_active == 1 && pause_flag == 0) {
            continueScrolling();
        }
    }

    if (ipending & 0x2) {
        *BUTTON_EDGE = 0x2;  // Clear the interrupt
        if (scrolling_active == 1)
            togglePause();
    }
}
