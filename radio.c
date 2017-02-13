#include <util/delay.h>
#include "pinout.h"
#include "radio.h"

// Definitions for selecting and enabling the radio
#define CSN_HIGH()  CSN_PORT |=  _BV(CSN_PIN);
#define CSN_LOW()   CSN_PORT &= ~_BV(CSN_PIN);
#define CE_HIGH()   CE_PORT |=  _BV(CE_PIN);
#define CE_LOW()    CE_PORT &= ~_BV(CE_PIN);

void radioInit(void) {
    // Disable the chip
    CE_LOW();

    // Set control pins as outputs...
    CE_DDR |= _BV(CE_PIN);
    CSN_DDR |= _BV(CSN_PIN);

    // Deselect and enable the chip
    CSN_HIGH();

    // Wait for power on (if we've just started)
    _delay_ms(15);

    spiInit();

}

status_t radioSendCommandPutGet(uint8_t command, const uint8_t* data, uint8_t* buffer, uint8_t len) {
    status_t status;

    CSN_LOW();
    status = spiReadWriteByte(command);
    spiReadWriteBlock(data, buffer, len);
    CSN_HIGH();

    return status;
}

status_t radioSendCommandPut(uint8_t command, const uint8_t* data, uint8_t len) {
    status_t status;

    CSN_LOW();
    status = spiReadWriteByte(command);
    spiWriteBlock(data, len);
    CSN_HIGH();

    return status;
}

status_t radioSendCommandGet(uint8_t command, uint8_t* buffer, uint8_t len) {
    status_t status;

    CSN_LOW();
    status = spiReadWriteByte(command);
    spiReadBlock(buffer, len);
    CSN_HIGH();

    return status;
}

status_t radioSendCommand(const uint8_t command) {
    status_t status;

    CSN_LOW();
    status = spiReadWriteByte(command);
    CSN_HIGH();

    return status;
}

status_t radioSetReg(uint8_t reg, const uint8_t* value, uint8_t len) {
    return radioSendCommandPut(W_REGISTER | (REGISTER_MASK & reg), value, len);
}

status_t radioGetReg(uint8_t reg, uint8_t* buffer, uint8_t len) {
    return radioSendCommandGet(R_REGISTER | (REGISTER_MASK & reg), buffer, len);
}

bool radioSetAndVerifyReg(uint8_t reg, const uint8_t* value, uint8_t len) {
    uint8_t* new_value;
    uint8_t i;
    bool success;

    new_value = malloc(len);
    radioSetReg(reg, value, len);
    radioGetReg(reg, new_value, len);
    success = true;
    for (i = 0; i < len; i++)
        if (value[i] != new_value[i]) {
            success = false;
            break;
        }
    free(new_value);

    return success;
}

status_t radioGetStatus(void) {
    return radioSendCommand(NOP);
}

status_t radioSetPower(bool pwr_status) {
    uint8_t config;
    radioGetReg(CONFIG, &config, 1);
    if (pwr_status)
        config |= _BV(PWR_UP);
    else
        config &= ~_BV(PWR_UP);

    return radioSetReg(CONFIG, &config, 1);
}

status_t radioPrepareTX(uint8_t* addr, uint8_t len) {

    radioSetReg(TX_ADDR, addr, len);
    return radioSetReg(RX_ADDR_P0, addr, len);
}

status_t radioTransmitPayload(uint8_t* payload, uint8_t len) {
    uint8_t config;
    status_t status;

    radioGetReg(CONFIG, &config, 1);
    config &= ~_BV(PRIM_RX);
    radioSetReg(CONFIG, &config, 1);

    status = radioSendCommandPut(W_TX_PAYLOAD, payload, len);
    radioPushTX();

    return status;
}

void radioPushTX(void) {
    CE_HIGH();
    _delay_us(20);
    CE_LOW();
}

status_t radioListen(void) {
    uint8_t config, status;

    radioGetReg(CONFIG, &config, 1);
    config |= _BV(PRIM_RX);
    status =radioSetReg(CONFIG, &config, 1);

    CE_HIGH();
    _delay_us(140);

    return status;
}

void radioChipDisable(void) {
    CE_LOW();
}

void radioDoCarrierTest(void) {
    uint8_t config, rf_setup;

    radioGetReg(CONFIG, &config, 1); 
    config &= ~_BV(PRIM_RX);
    config |= _BV(PWR_UP);
    radioSetReg(CONFIG, &config, 1);

    _delay_ms(2);

    radioGetReg(RF_SETUP, &rf_setup, 1);
    rf_setup |= _BV(CONT_WAVE);
    rf_setup |= _BV(PLL_LOCK);
	CE_HIGH();
	_delay_ms(10000);
	CE_LOW();
}
