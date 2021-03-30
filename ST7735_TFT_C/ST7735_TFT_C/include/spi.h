#ifndef _SPI_H_
#define _SPI_H_

#define eepromREAD	0x03		// Read data from memory array beginning at selected address
#define eepromWRITE	0x02		// Write data to memory array beginning at selected address
#define eepromWRDI	0x04		// Reset the write enable latch (disable write operations)
#define eepromWREN	0x06		// Set the write enable latch (enable write operations)
#define eepromRDRS	0x05		// Read status register
#define eepromWRSR	0x01		// Write status register
#define eepromNOPE	0x00		// No operation





#include<avr/io.h>

void spi_init(void);

void spi_write_eeprom(uint16_t address, uint8_t* pByte, uint8_t nBytes);
void spi_read_eeprom(uint16_t address, uint8_t* pByte, uint8_t nBytes);

static inline spi_transfer(uint8_t byte) { // m‰‰ritet‰‰n spi transfer funktio
	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}
//chip enable funktiota
static inline void spi_set_cs_tft(void) { //tft enable
	PORTB |= (1 << PB0);	// Mega D53
}

static inline void spi_unset_cs_tft(void) { //tft enable pois
	PORTB &= ~(1 << PB0);
}

static inline void spi_set_cs_eeprom(void) { // eeprom enable
	PORTB |= (1 << PB4);	
}

static inline void spi_unset_cs_eeprom(void) { // eeprom enable pois
	PORTB &= ~(1 << PB4);
}

#endif
