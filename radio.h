#include <stdint.h>
#include <stdbool.h>
#include "nRF24L01+.h"
#include "spi.h"

typedef uint8_t status_t;

void radioInit(void);

status_t radioSendCommandPutGet(uint8_t command, const uint8_t* data, uint8_t* buffer, uint8_t len);
status_t radioSendCommandPut(uint8_t command, const uint8_t* data, uint8_t len);
status_t radioSendCommandGet(uint8_t command, uint8_t* buffer, uint8_t len);
status_t radioSendCommand(uint8_t command);

status_t radioSetReg(uint8_t reg, const uint8_t* value, uint8_t len);
status_t radioGetReg(uint8_t reg, uint8_t* buffer, uint8_t len);
bool radioSetAndVerifyReg(uint8_t reg, const uint8_t* value, uint8_t len);

status_t radioGetStatus(void);

status_t radioSetPower(bool pwr_up);
#define radioPowerOn() radioSetPower(true);
#define radioPowerOff() radioSetPower(false);

status_t radioPrepareTX(uint8_t* addr, uint8_t len);
status_t radioTransmitPayload(uint8_t* payload, uint8_t len);
void radioPushTX(void);

status_t radioListen(void);

void radioChipDisable(void);

void radioDoCarrierTest(void);

