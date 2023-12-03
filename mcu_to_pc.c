#include <lpc214x.h>

#define BAUD_RATE 2400
#define CRC16_POLY 0xA001
#define EEPROM_START_ADDRESS 0x0000

void initUART() {
    U0LCR = 0x03;
    U0LCR |= (1 << 7);
    U0DLL = (PCLK / (16 * BAUD_RATE)) % 256;
    U0DLM = (PCLK / (16 * BAUD_RATE)) / 256;
    U0LCR &= ~(1 << 7);
    U0FCR = 0x07;
}

void uartSend(char data) {
    while (!(U0LSR & (1 << 5)));
    U0THR = data;
}

char uartReceive() {
    while (!(U0LSR & 0x01));
    return U0RBR;
}

void eepromWrite(unsigned int address, char data) {
    IAP_CONSET = 0x01;
    IAP_ADDRH = (address >> 8) & 0xFF;
    IAP_ADDRL = address & 0xFF;
    IAP_CMD = 61;
    IAP_DATA = data;
    IAP_CONSET = 0x04;
    IAP_TRIG = 0x46;
    IAP_TRIG = 0xB9;
    while (IAP_CONSET & 0x04);
    IAP_CONCLR = 0x0C;
}

char eepromRead(unsigned int address) {
    IAP_CMD = 62;
    IAP_ADDRH = (address >> 8) & 0xFF;
    IAP_ADDRL = address & 0xFF;
    IAP_TRIG = 0x46;
    IAP_TRIG = 0xB9;
    while (IAP_TRIG & 0x01);
    return IAP_DATA;
}

uint16_t calculateCRC16(char *data, size_t length) {
    uint16_t crc = 0xFFFF;

    while (length--) {
        crc ^= *data++;

        for (int i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ CRC16_POLY;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

int main() {
    initUART();

    char buffer[256];
    size_t bufferIndex = 0;

    while (1) {
        char receivedByte = uartReceive();
        if (receivedByte == '\0') {
            break;
        }
        
        eepromWrite(EEPROM_START_ADDRESS + bufferIndex, receivedByte);
        bufferIndex++;

        if (bufferIndex == sizeof(buffer)) {
            break;
        }
    }

    for (size_t i = 0; i < bufferIndex; i++) {
        char eepromData = eepromRead(EEPROM_START_ADDRESS + i);
        uartSend(eepromData);
    }

    return 0;
}
