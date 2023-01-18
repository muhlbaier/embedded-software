#include "system.h"
#include "eeprom.h"

#ifdef USE_MODULE_SUBSYS
void EEPROM_Callback(int argc, char *argv[]);
void EEPROM_Print(char c);
uint8_t sys_id;
#endif

void EEPROM_Init(void) {
    static uint8_t initialized = 0;
    if(initialized == 0) {
        // Initialize real or virtual EEPROM
        hal_EEPROM_Init();

    #ifdef USE_MODULE_SUBSYS
        sys_id = Subsystem_Init("EE", (version_t)0x01020001UL, EEPROM_Callback);
    #endif
        initialized = 1;
    }
}

uint16_t EEPROM_Read(uint16_t address) {
    return hal_EEPROM_Read(address);
}

void EEPROM_Write(uint16_t address, uint16_t data) {
    hal_EEPROM_Write(address, data);
}

#ifdef USE_MODULE_SUBSYS
void EEPROM_Callback(int argc, char *argv[]) {
    static uint16_t address, data;
    static uint16_t write = 0;
    if(argc == 0) { LogMsg(sys_id, "too few args"); return; }
    switch (*argv[0]) {
        case 'r':
        case 'R':
            address = ArgToU16(argv[1]);
            data = EEPROM_Read(address);
            LogMsg(sys_id, "Address: %d, Read: %d", address, data);
            break;
        case 'w':
            if (write == 0) {
                if(argc < 3) { LogMsg(sys_id, "too few args"); return; }
                address = ArgToU16(argv[1]);
                data = ArgToU16(argv[2]);
                if (address <= EEPROM_SIZE) {
                    write = 1;
                    LogMsg(sys_id, "Type $EE w again to save.");
                } else LogMsg(sys_id, "Invalid address");
            } else {
                EEPROM_Write(address, data);
                LogMsg(sys_id, "Address: %d, Wrote: %d", address, data);
                write = 0;
            }
            break;
        case 'p':
            UART_RegisterReceiver(SUBSYS_UART ,EEPROM_Print);
            EEPROM_Print(0);
            break;
        default:
            LogMsg(sys_id, "unknown cmd");
            write = 0;
            break;
    }
}

void EEPROM_Print(char c) {
    static uint16_t address = 0;
    uint16_t end_address;
    if (c == 0) address = 0;
    else if (c != ' ') {
        address = 0;
        UART_UnregisterReceiver(SUBSYS_UART, EEPROM_Print);
        return;
    }
    end_address = address + 10;
    LogMsg(sys_id, "EEPROM Contents (address, data):");
    while (address < end_address) {
        if (address > EEPROM_SIZE) {
            address = 0;
            UART_UnregisterReceiver(SUBSYS_UART, EEPROM_Print);
            return;
        }
        LogStr("%d, %d\r\n", address, EEPROM_Read(address));
        address++;
    }
    LogStr("Press space for the next 10\r\n");
}
#endif // USE_MODULE_SUBSYS
