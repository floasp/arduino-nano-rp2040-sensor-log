
#ifndef BME280_GUARD
#define BME280_GUARD 1

// ---- defines ----

#define BME280_ADDR 0x76

#define BME280_REG_ID 0xD0       // 8 bit, contains the chip ID (0x60 in this case)

#define BME280_REG_RESET 0xE0    // contains soft reset word, 8 bit, write 0xB6 to reset
#define BME280_VAL_RESET 0xB6    // soft reset word

#define BME280_REG_CTRL_HUM      0xF2 // sets humidity data aquisition options, only effective after write to "ctrl_meas", 3 bit
#define BME280_VAL_CTRL_HUM_SKIP 0b000 // skipped (output is 0x8000)
#define BME280_VAL_CTRL_HUM_OS1  0b001 // oversampling * 1
#define BME280_VAL_CTRL_HUM_OS2  0b010 // oversampling * 2
#define BME280_VAL_CTRL_HUM_OS4  0b011 // oversampling * 4
#define BME280_VAL_CTRL_HUM_OS8  0b100 // oversampling * 8
#define BME280_VAL_CTRL_HUM_OS16 0b101 // oversampling * 16

#define BME280_REG_STATUS 0xF3 // 2 status indicator bits (bit 3 measuring, bit 0 im_update)

#define BME280_REG_CTRL_MEAS             0xF4      // sets measurement data aquisition options, 3 bit
#define BME280_VAL_CTRL_MEAS_MODE_SLEEP  0b00000000 // sleep mode
#define BME280_VAL_CTRL_MEAS_MODE_FORCED 0b00000001 // forced mode
#define BME280_VAL_CTRL_MEAS_MODE_NORMAL 0b00000011 // normal mode
#define BME280_VAL_CTRL_MEAS_TEMP_SKIP   0b00000000 // skipped (output is 0x8000)
#define BME280_VAL_CTRL_MEAS_TEMP_OS1    0b00000100 // oversampling * 1
#define BME280_VAL_CTRL_MEAS_TEMP_OS2    0b00001000 // oversampling * 2
#define BME280_VAL_CTRL_MEAS_TEMP_OS4    0b00001100 // oversampling * 4
#define BME280_VAL_CTRL_MEAS_TEMP_OS8    0b00010000 // oversampling * 8
#define BME280_VAL_CTRL_MEAS_TEMP_OS16   0b00010100 // oversampling * 16
#define BME280_VAL_CTRL_MEAS_PRES_SKIP   0b00000000 // skipped (output is 0x8000)
#define BME280_VAL_CTRL_MEAS_PRES_OS1    0b00100000 // oversampling * 1
#define BME280_VAL_CTRL_MEAS_PRES_OS2    0b01000000 // oversampling * 2
#define BME280_VAL_CTRL_MEAS_PRES_OS4    0b01100000 // oversampling * 4
#define BME280_VAL_CTRL_MEAS_PRES_OS8    0b10000000 // oversampling * 8
#define BME280_VAL_CTRL_MEAS_PRES_OS16   0b10100000 // oversampling * 16

#define BME280_REG_CONFIG            0xF5 // rate filter and interface options
#define BME280_VAL_CONFIG_SPI3_EN    0b00000001 // enable SPI
#define BME280_VAL_CONFIG_FILTER_OFF 0b00000100 // filter coefficient 1
#define BME280_VAL_CONFIG_FILTER_2   0b00001000 // filter coefficient 2
#define BME280_VAL_CONFIG_FILTER_4   0b00001100 // filter coefficient 4
#define BME280_VAL_CONFIG_FILTER_8   0b00010000 // filter coefficient 8
#define BME280_VAL_CONFIG_FILTER_16  0b00010100 // filter coefficient 16
#define BME280_VAL_CONFIG_TSB_0_5    0b00000000 // standby time 0.5ms
#define BME280_VAL_CONFIG_TSB_62_5   0b00100000 // standby time 62.5ms
#define BME280_VAL_CONFIG_TSB_125    0b01000000 // standby time 125ms
#define BME280_VAL_CONFIG_TSB_250    0b01100000 // standby time 250ms
#define BME280_VAL_CONFIG_TSB_500    0b10000000 // standby time 500ms
#define BME280_VAL_CONFIG_TSB_1000   0b10100000 // standby time 1000ms
#define BME280_VAL_CONFIG_TSB_10     0b11000000 // standby time 10ms
#define BME280_VAL_CONFIG_TSB_20     0b11100000 // standby time 20ms

#define BME280_REG_PRES_MSB  0xF7 // raw pressure measurement MSB [19:12]
#define BME280_REG_PRES_LSB  0xF8 // raw pressure measurement LSB [11:4]
#define BME280_REG_PRES_XLSB 0xF9 // raw pressure measurement MSB [3:0], bits 7, 6, 5 and 4

#define BME280_REG_TEMP_MSB  0xFA // raw temperature measurement MSB [19:12]
#define BME280_REG_TEMP_LSB  0xFB // raw temperature measurement LSB [11:4]
#define BME280_REG_TEMP_XLSB 0xFC // raw temperature measurement MSB [3:0], bits 7, 6, 5 and 4

#define BME280_REG_HUM_MSB  0xFD // raw humidity measurement MSB [15:8]
#define BME280_REG_HUM_LSB  0xFE // raw humidity measurement LSB [7:0]

#define BME280_REG_DIG_T1_LSB 0x88
#define BME280_REG_DIG_T1_MSB 0x89
#define BME280_REG_DIG_T2_LSB 0x8A
#define BME280_REG_DIG_T2_MSB 0x8B
#define BME280_REG_DIG_T3_LSB 0x8C
#define BME280_REG_DIG_T3_MSB 0x8D

#define BME280_REG_DIG_P1_LSB 0x8E
#define BME280_REG_DIG_P1_MSB 0x8F
#define BME280_REG_DIG_P2_LSB 0x90
#define BME280_REG_DIG_P2_MSB 0x91
#define BME280_REG_DIG_P3_LSB 0x92
#define BME280_REG_DIG_P3_MSB 0x93
#define BME280_REG_DIG_P4_LSB 0x94
#define BME280_REG_DIG_P4_MSB 0x95
#define BME280_REG_DIG_P5_LSB 0x96
#define BME280_REG_DIG_P5_MSB 0x97
#define BME280_REG_DIG_P6_LSB 0x98
#define BME280_REG_DIG_P6_MSB 0x99
#define BME280_REG_DIG_P7_LSB 0x9A
#define BME280_REG_DIG_P7_MSB 0x9B
#define BME280_REG_DIG_P8_LSB 0x9C
#define BME280_REG_DIG_P8_MSB 0x9D
#define BME280_REG_DIG_P9_LSB 0x9E
#define BME280_REG_DIG_P9_MSB 0x9F

#define BME280_REG_DIG_H1     0xA1
#define BME280_REG_DIG_H2_LSB 0xE1
#define BME280_REG_DIG_H2_MSB 0xE2
#define BME280_REG_DIG_H3     0xE3
#define BME280_REG_DIG_H4_LSB 0xE4
#define BME280_REG_DIG_H4_H5  0xE5
#define BME280_REG_DIG_H5_MSB 0xE6
#define BME280_REG_DIG_H6     0xE7

// ---- types ----

typedef struct {
  float temperature;
  float pressure;
  float humidity;
} bme280_data_t;


// ---- functions ----

//void BME280_setupSensor();
//void BME280_printSetup();
//void BME280_readValues(int* values);

#endif
