#include "nios2_control.h"

#define TIMER_STATUS	((volatile unsigned int *) 0x10002000)
#define TIMER_CONTROL	((volatile unsigned int *) 0x10002004)
#define TIMER_START_LO	((volatile unsigned int *) 0x10002008)
#define TIMER_START_HI	((volatile unsigned int *) 0x1000200C)

#define JTAG_UART_DATA ((volatile unsigned int *) 0x10001000)
#define JTAG_UART_STATUS ((volatile unsigned int *) 0x10001004)
#define DATA_MASK 0xFFFF0000

#define BUTTON_DATA ((volatile unsigned int*) 0x10000050)
#define BUTTON_MASK ((volatile unsigned int*) 0x10000058)
#define BUTTON_EDGE ((volatile unsigned int*) 0x1000005C)

#define SWITCHES ((volatile unsigned int*) 0x10000040)
#define HEX_LED ((volatile unsigned int*) 0x10000020)

unsigned int scrolling_active = 0;
unsigned int current_idx = 0;
unsigned int pause_flag = 0;
unsigned int msg_flag = 0;
unsigned int done_flag = 0;
unsigned int length = 0;
unsigned int output_buff[256];
char* msg_ptr;	 

unsigned int ascii_hex_table[] = {
    0x3F,  0x06,  0x5B,  0x4F,
    0x66,  0x6D,  0x7D,  0x07,
    0x7F,  0x6F,  0x00,  0x00,
    0x00,  0x00,  0x00,  0x00
};

// waits till availible and prits a character 
void PrintChar(unsigned int c) {
	unsigned int status;

	// loop until status shows its free 
	while (!((*JTAG_UART_STATUS) & DATA_MASK ));
	*JTAG_UART_DATA = c; // write the character
	return;
}

// calls print char to print a string until \n
void PrintString(char *c) {
	int idx =0;
	while (c[idx] != '\0') {
		PrintChar(c[idx]);
		idx++;
	}
	return;
}

// Prints Hex Char
void PrintHexDigit(unsigned int offset) {
	if (offset > 10) {
		PrintChar('A' + (offset - 10));
	} else {
		PrintChar('0' + offset);
	}
}

void init_scrolling(void){
	int j = 0;
	int i = 0;
	output_buff[j++] = '0';
	output_buff[j++] = '0';
	output_buff[j++] = '0';
	output_buff[j++] = '0';
	while (msg_ptr[i] != '\0')
		output_buff[j++] = msg_ptr[i++];
	
	output_buff[j++] = '0';
	output_buff[j++] = '0';
	output_buff[j++] = '0';
	output_buff[j++] = '0';
	length = j;
	current_idx = 0;
	scrolling_active = 1;
	update_Hex(current_idx);
}

void cont_scrolling(void){
	current_idx++;
	if (current_idx < length-3)
		update_Hex(current_idx);
	else{
		scrolling_active = 0;
		done_flag = 1;
	}
}

void toggle_Pause(void){
	pause_flag = pause_flag ^ 1;
	*HEX_LED = *HEX_LED ^ 0x80808080;
}

void update_Hex(unsigned int index) {	
	unsigned int data_to_display = ascii_hex_table[output_buff[index] - '0'] << 24
        | ascii_hex_table[output_buff[index + 1] - '0'] << 16
        | ascii_hex_table[output_buff[index + 2] - '0'] << 8
        | ascii_hex_table[output_buff[index + 3] - '0'];
    
    *HEX_LED = data_to_display;
}  


void interrupt_handler(void){
	int ipending = NIOS2_READ_IPENDING();
	if (ipending & 0x1){
		*TIMER_STATUS = 0;
		if (scrolling_active == 0 && msg_flag == 1){
			init_scrolling();
			msg_flag = 0;
		} else if (scrolling_active == 1  && pause_flag == 0){
			cont_scrolling();
		}
	}
	if (ipending & 0x2){
		*BUTTON_EDGE = 0x2;
		if (scrolling_active == 1)
			toggle_Pause();
	}
}

void init(void){
	int cycles, speed;
	*BUTTON_EDGE = 0x2;
	*BUTTON_MASK = 2;
	*TIMER_STATUS = 0;
	
	speed = *SWITCHES & 0x3;
	if (speed == 0){
		cycles = 0.25*50*10*10*10*10*10*10;
	}else if (speed == 1){
		cycles = 0.5*50*10*10*10*10*10*10;
	}else if (speed == 2){
		cycles = 1*50*10*10*10*10*10*10;
	}else {
		cycles = 2*50*10*10*10*10*10*10;
	}
	*TIMER_START_HI = cycles >> 16;
	*TIMER_START_LO = cycles;
	*TIMER_CONTROL = 7;
	NIOS2_WRITE_IENABLE(0x3);
	NIOS2_WRITE_STATUS(0x1);
}

int main(void) {
	init();
    unsigned int idx = 0;
    char words[3][4] = { {"123\0"}, {"456\0"}, {"789\0"}};
	msg_flag = 1;
    msg_ptr = words[idx];
	PrintString(msg_ptr);
	PrintChar('\n');

    while (1) {
        if (done_flag == 1) {
            done_flag = 0;
            msg_ptr = words[++idx];
            msg_flag = 1;
            PrintString(msg_ptr);
            PrintChar('\n');
        }
    }

    return 0;
}
