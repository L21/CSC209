#include <string.h>
#include <stdio.h>

#include "data.h"

// based on code from:
// http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format?rq=1

const char *to_binary(int x) {
    static char bits[17];
    bits[0] = '\0';
    int z;
    for (z = 1 << 15; z > 0; z >>= 1) {
        strcat(bits, ((x & z) == z) ? "1" : "0");
    }
    return bits;
}

int get_most_significant_bit(unsigned short *reg, int i) {
	unsigned short a;
	int b;
	a = 1 << (i-1);
	if ((*reg | a) == *reg) {
		b = 1;
	} else {
		b = 0;
	}
	return b;
}



/*  
  Process one bit of the message.
  reg: register (modified by this function)
  key: CRC16 key being used
  next_bit: next bit of message
*/

int crc_bit(unsigned short *reg, unsigned int key, unsigned int next_bit) {
	unsigned short c;
	int z;
	z = get_most_significant_bit(reg, 16);
	*reg = *reg << 1;
	if (next_bit == 1) {
		c = 1 ;
		*reg = *reg | c;
	}
	if (z == 1) {
		*reg = *reg ^ key;
	}
	return *reg;

}
  
/*
  Process one byte of the message.
  reg: register (modified by this function)
  key: CRC16 key being used
  next_byte: next byte of message
*/

int crc_byte(unsigned short *reg, unsigned int key, unsigned int next_byte) {
	int i ;
	unsigned int j ;
	unsigned short temp = next_byte;
	unsigned short *next_byte1;
	next_byte1 = &temp;
	for (i = 0; i < 8; i++) {
		j = get_most_significant_bit(next_byte1, 8);
		
		*next_byte1 <<= 1;
		crc_bit(reg, key, j);
    }
	return *reg;
}

/*
  Process an entire message using CRC16 and return the CRC.
  key: CRC16 key being used
  message: message for which we are calculating the CRC.
  num_bytes: size of the message in bytes.
*/

unsigned short crc_message(unsigned int key,  unsigned char *message, int num_bytes) {
	unsigned short *reg;
	unsigned short temp = 0x0000;
	int i;
	int j;
	int k;
	reg = &temp;
	unsigned int a = (unsigned int) *message;
	for (i = 0; i < num_bytes; i++) {
		crc_byte(reg, key, a);
	}
	for (j = 0; j <16; j++) {
		k = get_most_significant_bit(reg, 16);
		*reg <<= 1;
		if (k == 1) {
			*reg = *reg ^ key;
		}
		
	}
	return *reg;
			
	
	
}


/*testing code*/

//int main(void) {
  //unsigned short crc16 = crc_message(XMODEM_KEY, "z", 1);
  //printf("%s\n", to_binary(crc16));
  //printf(crc_bit(0000000000000000, "aaa", 1));
  /*unsigned short *reg;
  unsigned short temp = 0x0000;
  reg = &temp;
  printf("%s\n",to_binary(crc_byte(reg, 1, 01111111)));
  printf("%s\n",to_binary(crc_bit(reg, 1, 0)));
  printf("%s\n",to_binary(crc_bit(reg, 1, 0)));
  printf("%s\n",to_binary(crc_bit(reg, 1, 0)));*/
  /*unsigned int b ;
  unsigned short *i;
  unsigned short a = 0b10101010;
  i = &a;
  b = get_most_significant_bit(i, 8);
  printf("%d\n",b);*/
  //return 0;
//}


