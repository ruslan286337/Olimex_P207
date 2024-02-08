#include "spi_interface.h"

volatile uint16_t spi_package[__MAX_COMMAND_LEN] = {0, 0, 0, 0};
volatile uint8_t spi_sent;
volatile uint16_t spi_len = 0;
volatile uint8_t spi_control_mod_enable = 0;
volatile uint8_t spi_control_mod_request = 0;


enum spi_states spi_state;


tQUEUE_SPI q_spi[__QUEUE_LEN];

volatile uint16_t queue_buff_pos = 0;

uint16_t queue_buff_check_pos = 0;
uint16_t q_data;
enum PACKAGE_TYPE q_p_type;


void spi_handler(void){
	switch(spi_state){
	case SPI_CHECK:
		if (q_spi[queue_buff_check_pos].not_used){
			spi_len = q_spi[queue_buff_check_pos].len;
			for (uint8_t i = 0; i < spi_len; i++){
				spi_package[i] = q_spi[queue_buff_check_pos].data[i];
			}
			q_spi[queue_buff_check_pos].not_used = 0;

			spi_state = SPI_START;

		} else {
			queue_buff_check_pos++;
			if (queue_buff_check_pos >= __QUEUE_LEN) queue_buff_check_pos = 0;
		}
		if(spi_control_mod_request){
			uint8_t check_queue = 0;
			for (uint8_t i = 0; i < __QUEUE_LEN; i++){
				check_queue += q_spi[i].not_used;
			}
			if (!check_queue){
				spi_control_mod_enable = 1;
				spi_control_mod_request = 0;
				spi_len = 26144;
				spi_state = SPI_START;
			}
		}
		break;
	case SPI_START:
		LCD_CS_ON();
		TIM2->SR = 0;
		TIM2->DIER |= TIM_DIER_UIE | TIM_DIER_CC1IE;
		TIM2->CR1 |= TIM_CR1_CEN;
		spi_state = SPI_END;
		break;
	case SPI_END:
		if(spi_sent){
			spi_state = SPI_CHECK;
			spi_sent = 0;
		}
		break;
	}
}
/*
void spi_add_to_queue(uint16_t * data, uint16_t len, uint8_t need_nop){

	for (uint8_t i = 0; i < len; i++){
		if (!i){
			q_spi[queue_buff_pos].data[i] = data[i];
		} else if ((i == (len-1)) & (need_nop)) {
			q_spi[queue_buff_pos].data[i] = data[i];
		} else {
			q_spi[queue_buff_pos].data[i] = (data[i] | 0x0100);

		}
	}
	q_spi[queue_buff_pos].len = len;
	q_spi[queue_buff_pos].not_used = 1;

	queue_buff_pos++;

	if (queue_buff_pos == __QUEUE_LEN) queue_buff_pos = 0;
}
*/
void spi_add_to_queue(uint8_t data_1, uint8_t data_2, uint8_t data_3, uint8_t data_4, uint8_t len, uint8_t need_nop){
	for (uint8_t i = 0; i < len; i++){
		switch(i){
		case 0:
			q_spi[queue_buff_pos].data[i] = data_1;
			break;
		case 1:
			q_spi[queue_buff_pos].data[i] = (data_2 | 0x0100);
			break;
		case 2:
			q_spi[queue_buff_pos].data[i] = (data_3 | 0x0100);
			break;
		case 3:
			if(need_nop){
				q_spi[queue_buff_pos].data[i] = data_4;
			}else{
				q_spi[queue_buff_pos].data[i] = (data_4 | 0x0100);
			}
			break;
		}
	}

	q_spi[queue_buff_pos].len = len;
	q_spi[queue_buff_pos].not_used = 1;

	queue_buff_pos++;

	if (queue_buff_pos == __QUEUE_LEN) queue_buff_pos = 0;
}
