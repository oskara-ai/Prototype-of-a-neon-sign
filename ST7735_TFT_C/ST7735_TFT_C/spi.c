#include "./include/spi.h"
#define F_CPU 16000000L
#include <util/delay.h>
//-----------------------------------------------------------------------
// Define macros for spi_functions
//-----------------------------------------------------------------------
#define MAXADDR 4095
#define ADDRSPLIT(Msb,Lsb,address)\
Msb=(uint8_t)(address>>8);\
Lsb=(uint8_t)(address & 0x00FF);
//-----------------------------------------------------------------------
void spi_init(void) {
	// Set MOSI and SCK, SS/CS, and set eeprom output, all others input, 
	DDRB = (1<<PB2) | (1<<PB1) | (1<<PB0) | (1<<PB4);
	SPCR = (1<<SPE) | (1<<MSTR);// Enable SPI, Master, set clock rate fck/4, mode 0
	PORTB |= ((1 << PB0) |(1<< PB4));// Set SS/CS high
}
//-----------------------------------------------------------------------
// function for spi_read
//-----------------------------------------------------------------------
void spi_read_eeprom(uint16_t address, uint8_t *pByte, uint8_t nBytes)
{
	uint8_t LsbAddress,MsbAddress;	// variables 
	uint8_t ByteCounter = 0;		// counter for bytes
	
	ADDRSPLIT(MsbAddress, LsbAddress, address);	//use ADDRSPLIT macro 
	
	spi_unset_cs_eeprom();			// Pull eeprom cs low
	spi_transfer(eepromREAD);		// enable spi_transfer 
	spi_transfer(MsbAddress);		// write data from MsbAddress
	spi_transfer(LsbAddress);		// write data from LsbAddress
	
	do								// last byte read or end of eeprom?
	{
		*pByte = spi_transfer(eepromNOPE);
		if (*pByte == '\r'){
			break;
		}
		pByte++;					// raise pByte by 1
		ByteCounter++				;// raise ByteCounter by 1
	} while (address + ByteCounter < 0xFFF && ByteCounter <= nBytes); // if last byte has been transfered 
	
	spi_set_cs_eeprom();			// pull eeprom cs high
}
//-----------------------------------------------------------------------
//function for spi_write
//-----------------------------------------------------------------------
void spi_write_eeprom(uint16_t address, uint8_t *pByte, uint8_t nBytes)
{
		uint8_t LsbAddress,MsbAddress;
				
		ADDRSPLIT(MsbAddress, LsbAddress, address);
		
		spi_unset_cs_eeprom();						// Pull eeprom cs low
		spi_transfer(eepromWREN);					//enable spi_transfer operations 
		spi_set_cs_eeprom();						// Pull eeprom cs high
		spi_unset_cs_eeprom();						// Pull eeprom cs low
		spi_transfer(eepromWRITE);					// write data to memory
		spi_transfer(MsbAddress);					// write data from MsbAddress
		spi_transfer(LsbAddress);					// write data from LsbAddress
		 
		while (nBytes && (address <= MAXADDR))		// last byte or end of eeprom?
		{
			spi_transfer(*pByte);					// write data to *pByte
			address++;								// raise address by 1
			if(address == 0x0000)break;				// if in the same page break
			pByte++;								// raise pByte by 1
			nBytes--;								// decrease nBytes by 1
			
			if (!(address & 0x001F))				// start writing to new page
			{
				ADDRSPLIT(MsbAddress,LsbAddress,address);
				spi_set_cs_eeprom();				// Pull eeprom cs high
				_delay_ms(5);						// new page needs 5ms delay
				spi_unset_cs_eeprom();				// Pull eeprom cs low
				spi_transfer(eepromWREN);			// enable spi_transfer operations
				spi_set_cs_eeprom();				// Pull eeprom cs high
				spi_unset_cs_eeprom();				// Pull eeprom cs low
				spi_transfer(eepromWRITE);			// write data to memory
				spi_transfer(MsbAddress);			// write data from MsbAddress
				spi_transfer(LsbAddress);			// write data from LsbAddress
			}
		}
			
		spi_set_cs_eeprom();						// Pull eeprom cs high
		_delay_ms(40);								// needs at least 40ms delay to work
}
//-----------------------------------------------------------------------