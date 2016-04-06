



//#include "../avr-uart/uart.h"

#include "roomba.h"

//#include "sensor_struct.h"

/*

#define LOW_BYTE(v)   ((unsigned char) (v))

#define HIGH_BYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))



#define DD_DDR DDRE

#define DD_PORT PORTE

#define DD_PIN PE4



void roomba_set_baud(void(*uart_send_byte_fn)(uint8_t), ROOMBA_BITRATE baudrate) {

  uart_send_byte_fn(ROOMBA_BAUD);

  uart_send_byte_fn(baudrate);

  _delay_ms(100);

}



void roomba_start(void(*uart_send_byte_fn)(uint8_t)) {

	uart_send_byte_fn(ROOMBA_SAFE);

	_delay_ms(20);

}



void roomba_init(void(*uart_send_byte_fn)(uint8_t)) {

	DD_DDR |= _BV(DD_PIN);

	DD_PORT &= ~_BV(DD_PIN);

	_delay_ms(500);

	DD_PORT |= _BV(DD_PIN);

	_delay_ms(2000);

	for (uint8_t i = 0; i < 6; i++) {

		DD_PORT ^= _BV(DD_PIN);

		_delay_ms(50);

	}

	DD_PORT &= ~_BV(DD_PIN);

	uart_send_byte_fn(ROOMBA_START);

	_delay_ms(20);

}

*/





int get_command_data_bytes (ROOMBA_OP_CODE command) {

  switch(command) {

    case ROOMBA_DRIVE: return 4;

    default: return -1;

  }

}



int is_valid_roomba_command (uint8_t command[], uint16_t size) {

  switch (command[0]) {

    case ROOMBA_RESET: return size==1;

    case ROOMBA_DRIVE:

      return (size==5) &&

      ((command[1]<<8|command[2]) >= -500 &&

       (command[1]<<8|command[2]) <= 500) &&

      (((command[3]<<8|command[4]) >= -2000 &&

        (command[3]<<8|command[4]) <= 2000)||

        (command[3]<<8|command[4])==ROOMBA_RADIUS_STRAIGHT_POSITIVE ||

        (command[3]<<8|command[4])==ROOMBA_RADIUS_STRAIGHT_NEGATIVE);

    default: return false;

  }

}
