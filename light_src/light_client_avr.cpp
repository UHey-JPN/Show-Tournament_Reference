/*
 * dmx_led.cpp
 *
 * Created: 2017/02/25 7:48:34
 * Author : Yuhei MATSUMURA
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SW_HIGH (PINC & 0b00000001)

#define REG_CH1 OCR1A
#define REG_CH2 OCR1B

#define DELAY_TIME 2

volatile uint8_t dmx_data[4] = {0};
volatile int16_t dmx_cnt = 0;

volatile uint8_t send_flag = 0;

volatile uint8_t off_set = 0;

void init_port(void){
	// bit : 76543210
	DDRB = 0b00000110;
	DDRC = 0b00000000;
	DDRD = 0b00000010;
	
	// bit  : 76543210
	PORTB = 0b00000000;
	PORTC = 0b00000000;
	PORTD = 0b00000000;
	
}

void init_uart(uint32_t baud){
	UCSR0A = 0b00000010; // 倍速許可モード
	UCSR0B = 0b10011000; // 受信完了割り込み、送受信許可、データ長8bit
	UCSR0C = 0b00001110; // 非同期動作、パリティなし、停止ビット長2bit、データ長8bit
	UBRR0  = F_CPU/baud/8 - 1;
}

void uart_send(uint8_t data){
	while( !(UCSR0A & 0b00100000 ) );
	UDR0 = data;
}

uint32_t conv_lut(uint8_t data){
	uint32_t output = 0;
	if( data < 3 ){
		output = (uint32_t)data * 1666 / 1000;
		
	}else if( data < 10 ){
		output = (uint32_t)data * 8857 / 1000 - 21;
		
	}else if( data < 45 ){
		output = (uint32_t)data * 4463 / 100 - 379;
		
	}else if( data < 100 ){
		output = (uint32_t)data * 132 - 4312;
		
	}else if( data < 185 ){
		output = (uint32_t)data * 282 - 19311;
		
	}else{
		output = (uint32_t)data * 467 - 53484;
		
	}
	if( 65535 < output ) output = 65535;
	
	return output;

}


ISR(USART_RX_vect){
	uint8_t data = UDR0;
	if( (dmx_cnt == 0) && (data == 131) ){
		dmx_cnt += 1;
	}else if( (1 <= dmx_cnt) && (dmx_cnt <= 4) ){
		dmx_data[dmx_cnt-1] = data;
		dmx_cnt += 1;
	}else{
		dmx_cnt = 0;
		REG_CH1 = conv_lut( dmx_data[off_set+0] );
		REG_CH2 = conv_lut( dmx_data[off_set+1] );
	}
}




int main(void){

	init_port();
	init_uart(9600);
	
	// set offset
	if(SW_HIGH) off_set = 2;
	
	TCCR1A = 0b11110010;
	TCCR1B = 0b00010001;
	ICR1 = 0xFFFF;

	if(off_set == 0){
		for(uint16_t i = 0; i < 256; i++){
			REG_CH1 = conv_lut(i);
			_delay_ms(DELAY_TIME);
		}
		for(uint16_t i = 0; i < 256; i++){
			REG_CH2 = conv_lut(i);
			_delay_ms(DELAY_TIME);
		}
		for(uint16_t i = 0; i < 512; i++) _delay_ms(DELAY_TIME);
	}else{
		for(uint16_t i = 0; i < 512; i++) _delay_ms(DELAY_TIME);
		for(uint16_t i = 0; i < 256; i++){
			REG_CH1 = conv_lut(i);
			_delay_ms(DELAY_TIME);
		}
		for(uint16_t i = 0; i < 256; i++){
			REG_CH2 = conv_lut(i);
			_delay_ms(DELAY_TIME);
		}
	}
	
	REG_CH1 = conv_lut(50);
	REG_CH2 = conv_lut(50);
	
	sei();
	
	while(1);
	
	return 0;
}


