/*
 * roomba.c
 *
 *  Created on: 4-Feb-2009
 *      Author: nrqm
 */
 /*
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

//#include "../avr-uart/uart.h"
#include "roomba.h"
//#include "sensor_struct.h"

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
