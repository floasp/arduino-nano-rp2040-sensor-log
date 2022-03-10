
#ifndef CCS811_GUARD
#define CCS811_GUARD 1

// ---- defines ----

#define CCS811_ADDR 0x5A

#define CCS811_REG_STATUS 0x00

#define CCS811_REG_MEAS_MODE      0x01
#define CCS811_VAL_DRIVE_MODE_0   0b00000000
#define CCS811_VAL_DRIVE_MODE_1   0b00010000
#define CCS811_VAL_DRIVE_MODE_2   0b00100000
#define CCS811_VAL_DRIVE_MODE_3   0b00110000
#define CCS811_VAL_DRIVE_MODE_4   0b01000000
#define CCS811_VAL_INT_DATARDY_EN 0b00001000
#define CCS811_VAL_INT_THRESH_EN  0b00000100

#define CCS811_REG_ALG_RESULT_DATA 0x02 // read 8 bytes for full data
// byte 0 and 1 -> eCO2 MSB, LSB
// byte 2 and 3 -> eTVOC MSB, LSB
// byte 4 status
// byte 5 Error ID
// byte 6 & 7 See RAW_DATA

#define CCS811_REG_RAW_DATA 0x03

#define CCS811_REG_ENV_DATA 0x05 // byte 0-1 humidity, byte 2-3 temperature

#define CCS811_REG_THRESHOLDS 0x10

#define CCS811_REG_BASELINE 0x11

#define CCS811_REG_HW_ID      0x20
#define CCS811_REG_HW_VERSION 0x21

#define CCS811_REG_FW_BOOT_VERSION 0x23
#define CCS811_REG_FW_APP_VERSION  0x24

#define CCS811_REG_ERROR_ID 0xE0

#define CCS811_REG_SW_RESET 0xFF

#define CCS811_REG_APP_ERASE  0xF1
#define CCS811_REG_APP_DATA   0xF2
#define CCS811_REG_APP_VERIFY 0xF3
#define CCS811_REG_APP_START  0xF4 // needs to be called when mode is changed

typedef struct {
  uint16_t eCO2;
  uint16_t eTVOC;
  uint8_t stat;
  uint8_t error_id;
  uint16_t raw_data;
} ccs811_data_t;

#endif
