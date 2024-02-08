#include "lcd.h"
//#include "spi_interface.h"
//enum REQUEST_TYPE req_status;

//enum LCD_12_COLOR lcd_color;

volatile tControl_LCD control_lcd;

void send_start_bit(enum REQUEST_TYPE req_status){
	SPI_SCK_RESET();
	if (req_status){
		SPI_MOSI_SET();
	}else{
		SPI_MOSI_RESET();
	}
    for(uint8_t i = 5; i; i--);
    SPI_SCK_SET();
}

void dummy_cycle(void){
	SPI_SCK_RESET();
	for(uint8_t i = 5; i; i--);
	SPI_SCK_SET();
}

void transfer_data(uint8_t data, enum REQUEST_TYPE req_status){
	send_start_bit(req_status);

	for(uint16_t mask = 1UL << 7; mask; mask >>= 1){
		SPI_SCK_RESET();
		if(mask&data){
			SPI_MOSI_SET();
		}else{
			SPI_MOSI_RESET();
		}
	    for(uint8_t i = 5; i; i--);
	    SPI_SCK_SET();
	}
	SPI_MOSI_RESET();
	SPI_SCK_RESET();
}

uint8_t read_data(void){
	uint8_t data = 0;
	//dummy_cycle();

	for(uint16_t mask = 1UL << 7; mask; mask >>= 1){
		SPI_SCK_RESET();
		if(SPI_MISO_GET()){
			data |= mask;
		}
	    for(uint8_t i = 5; i; i--);
	    SPI_SCK_SET();
	}
	SPI_SCK_RESET();
	return data;
}

void transfer_data_block(uint8_t * buf, enum REQUEST_TYPE req_status, uint8_t size){
	for (uint8_t i = 0; i < size; i++){
		transfer_data(buf[i], req_status);
	}
}

void read_data_block(uint8_t * buf, uint8_t size){
	//dummy_cycle();
	for (uint8_t i = 0; i < size; i++){
		buf[i] = read_data();
	}
	SPI_SCK_RESET();
}

void read_cmd_lcd(uint8_t cmd_id, uint8_t * buf, uint8_t size){
	LCD_CS_ON();
	transfer_data(cmd_id, L_COMMAND);
	dummy_cycle();
	read_data_block(buf, size);
	LCD_CS_OFF();
}

void send_cmd_to_lcd(uint8_t cmd_id){
	LCD_CS_ON();
	transfer_data(cmd_id, L_COMMAND);
	LCD_CS_OFF();
}

void write_to_cmd_lcd(uint8_t cmd_id, uint8_t data){
	LCD_CS_ON();
	transfer_data(cmd_id, L_COMMAND);
	transfer_data(data, L_DATA);
	LCD_CS_OFF();
}

void send_data_to_lcd(uint8_t * buf, uint8_t size){
	LCD_CS_ON();
	transfer_data_block(buf, L_DATA, size);
	LCD_CS_OFF();
}

void lcd_delay(uint16_t t_ms){
	uint16_t time = ms_ticks + t_ms;

	while(time > ms_ticks);
}

void lcd_reset(void){
	LCD_RST_RESET();
	lcd_delay(1);
	LCD_RST_SET();
	lcd_delay(5);
}

void lcd_init(void){
	LCD_CS_OFF();
	SPI_MOSI_RESET();
	SPI_SCK_RESET();
	lcd_reset();

	__SPI_SEND_CMD(LCD_CMD_SLEEP_OUT);
	__SPI_SEND_CMD(LCD_CMD_INVON);
	__SPI_SEND_SETT_LEN_2(LCD_CMD_COLMOD, 0x03);
	__SPI_SEND_SETT_LEN_2(LCD_CMD_MADCTL, 0x08);
	__SPI_SEND_SETT_LEN_2(LCD_CMD_SETCON, 0x30);
	__SPI_SEND_CMD(LCD_CMD_DISPON);

	control_lcd.S.coordinates.cmd_СASET = LCD_CMD_CASET;
	control_lcd.S.coordinates.cmd_PASET = LCD_CMD_PASET;

	control_lcd.S.pixels.cmd_NOP = LCD_CMD_NOP;
	control_lcd.S.pixels.cmd_RAMWR = LCD_CMD_RAMWR;
}

void LCD_set_x_y(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end){

	__SPI_SEND_SETT_LEN_3(LCD_CMD_PASET, x_start, x_end);
	__SPI_SEND_SETT_LEN_3(LCD_CMD_CASET, y_start, y_end);

}
/*
void LCD_clear(void){
	uint16_t clear_color = RED;
	uint8_t color_buf[3];
	color_buf[0] = (uint8_t)((clear_color >> 4)&0xFF);
	color_buf[1] = (uint8_t)(((clear_color&0xF) << 4) | ((clear_color >> 8)&0xF));
	color_buf[2] = (uint8_t)(clear_color&0xFF);
	LCD_set_x_y(0, 0, 131, 131);

	send_cmd_to_lcd(LCD_CMD_RAMWR);

	for (uint16_t i = 0; i < ((132*132)/2); i++){
		send_data_to_lcd(color_buf, 3);
	}
	send_cmd_to_lcd(LCD_CMD_NOP);
}
*/
void LCD_clear(enum LCD_12_COLOR color){
	uint16_t step = 0;

	control_lcd.S.coordinates.x_start 	= (0 | 0x0100);
	control_lcd.S.coordinates.x_end 	= (131 | 0x0100);
	control_lcd.S.coordinates.y_start 	= (0 | 0x0100);
	control_lcd.S.coordinates.y_end 	= (131 | 0x0100);

	for (uint16_t i = 0; i < 8712; i++){
		control_lcd.S.pixels.buf[0 + step] = (((color >> 4)&0xFF) | 0x0100);
		control_lcd.S.pixels.buf[1 + step] = ((((color&0xF) << 4) | ((color >> 8)&0xF)) | 0x0100);
		control_lcd.S.pixels.buf[2 + step] = ((color&0xFF) | 0x0100);

		step = step + 3;
	}
	spi_control_mod_request = 1;
}

void LCD_set_pixel(uint8_t x, uint8_t y, enum LCD_12_COLOR color){
	LCD_set_x_y(x, x, y, y);

	__SPI_SEND_PIXEL(LCD_CMD_RAMWR, (uint8_t)(color >> 4), (uint8_t)(color << 8), LCD_CMD_NOP);
}



