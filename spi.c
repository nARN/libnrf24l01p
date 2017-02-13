/*
   Based on the work by Neil MacMillan
   Functions for using the atmega328p as an SPI master.
*/

#include <avr/io.h>
#include "spi.h"
#include "pinout.h"
#include "nRF24L01+.h"

// wait for an SPI read/write operation to complete
//#define SPI_WAIT() loop_until_bit_is_set(SPSR, SPIF);
#define SPI_WAIT() while ((SPSR & _BV(SPIF)) == 0);

void spiInit() {
	// The DDR operations are pretty fragile and doing this less awkwardly breaks SPI.  I don't care enough
	// to figure out why right now.  Note that SPI_SS is the actual SPI port's SS pin, which is not necessarily
	// the SS pin that's used for a given slave.  Anything that uses this driver must handle its own slave selection.
	// It must set its SS pin direction to output, set the pin low before doing an SPI operation, and set it high
	// when the SPI operation is complete.
    SPI_DDR &= ~(_BV(SPI_MOSI)|_BV(SPI_MISO)|_BV(SPI_SS)|_BV(SPI_SCK));
    // Define the following pins as output
    SPI_DDR |= (_BV(SPI_MOSI) | _BV(SPI_SS) | _BV(SPI_SCK));

	/* SPCR format:
	 *
	 * bit 7
	 * SPE - SPI enable
	 * SPIE - SPI interrupt enable
	 * DORD - Data order (0 MSB first; 1 LSB first)
	 * MSTR - Master (0 slave; 1 master)
	 * CPOL - Clock polarity (should be 0)
	 * CPHA - Clock phase (should be 0)
	 * SPR1 | SPR0 - Determines SCK frequency along with the SPI2X bit in SPSR.  See Table 17-4 in hardware manual [p178].
	 * bit 0
	 */

    // Set the AT90's SS pin high during config (this disables the Flash RAM or something)
    SPI_PORT |= _BV(SPI_SS);
	SPCR = _BV(SPE) | _BV(MSTR);	// enable SPI, set as master, set prescaler to f(osc)/4
	SPSR = _BV(SPI2X);				// Double SCK to f(osc)/2 (4 MHz)
	SPI_PORT &= ~_BV(SPI_SS);
}

uint8_t spiReadWriteByte(uint8_t byte) {
    SPDR = byte;
    SPI_WAIT();
    return SPDR;
}

void spiWriteByte(uint8_t byte) {
    SPDR = byte;
    SPI_WAIT();
}

uint8_t spiReadByte(void) {
    return spiReadWriteByte(NOP);
}

void spiReadWriteBlock(uint8_t* data, uint8_t* buffer, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++) {
        buffer[i] = spiReadWriteByte(data[i]);
    }
}

void spiWriteBlock(uint8_t* data, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++) {
        spiWriteByte(data[i]);
    }
}

void spiReadBlock(uint8_t* buffer, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++) {
        buffer[i] = spiReadByte();
    }
}
