/*
   Based on the work by Neil MacMillan
   Functions for using the atmega328p as an SPI master.
*/

#ifndef SPI_H_
#define SPI_H_

#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>

/**
 * Initialize the SPI port as a master.
 * This function must be called once before using the SPI interface.
 */
void spiInit();

/**
 * Write a block of data to the slave, and read the data returned from the slave into a buffer.
 * The data and buffer memory blocks must be the same length.
 * \param data A pointer to the contiguous memory block to write to the slave.
 * \param buffer A pointer to the contiguous memory block to which the SPI data are to be written.
 * \param len The length of the memory blocks, in bytes.
 */
void spiReadWriteBlock (uint8_t * data, uint8_t * buffer, uint8_t len);

/**
 * Write a byte to the slave, and get the return byte from the slave.
 */
uint8_t spiReadWriteByte(uint8_t byte);

// More confenience functions
void spiWriteByte(uint8_t byte);
uint8_t spiReadByte(void);

void spiWriteBlock(uint8_t* data, uint8_t len);
void spiReadBlock(uint8_t* buffer, uint8_t len);

#endif /* SPI_H_ */
