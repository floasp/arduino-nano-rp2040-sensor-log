
#include <Wire.h>
#include <WiFiNINA.h>
#include <SPI.h>
#include "wifi_secrets.h"
#include "bme280.h"
#include "CCS811.h"

// ---- WIFI ----

char ssid[] = WIFI_SSID; //  your network SSID (name)
char pass[] = WIFI_PASS; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;        // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = SERVER_NAME;
String sensor_id = "48";
String sensor_token = "56cf1f7ab75ee727";

WiFiClient client;

// ---- BME280 ----

bme280_data_t bme280_values;

uint16_t dig_T1 = 0;
uint16_t dig_T2 = 0;
uint16_t dig_T3 = 0;

uint16_t dig_P1 = 0;
uint16_t dig_P2 = 0;
uint16_t dig_P3 = 0;
uint16_t dig_P4 = 0;
uint16_t dig_P5 = 0;
uint16_t dig_P6 = 0;
uint16_t dig_P7 = 0;
uint16_t dig_P8 = 0;
uint16_t dig_P9 = 0;

uint8_t  dig_H1 = 0;
uint16_t dig_H2 = 0;
uint8_t  dig_H3 = 0;
uint16_t dig_H4 = 0;
uint16_t dig_H5 = 0;
uint8_t  dig_H6 = 0;

// ---- CCS811 ----

ccs811_data_t css811_values;


void setup() {
  //Initialize serial and wait for port to open:
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // ---- WIFI Setup ----

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("[WIFI] WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("[WIFI] Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("[WIFI] Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("[WIFI] Connected to wifi");
  printWifiStatus();

  // ---- BME280 Setup ----
  
  Serial.println("[BME280] Starting Setup.");
  delay(100);
  
  BME280_setupSensor();
  Serial.println("[BME280] Setup complete.");

  BME280_loadCalibration();
  Serial.println("[BME280] Calibration data loaded.");

  // ---- CCS811 Setup ----
  
  Serial.println("[CCS811] Starting Setup.");

  CCS811_setupSensor();
  Serial.println("[CCS811] Setup complete.");
  
  CCS811_startSensor();
  Serial.println("[CCS811] Sensor started.");

  delay(100);
}

void loop() {

  // ---- BME280 Readout ----

  BME280_readValues(&bme280_values);

  Serial.print("[BME280] Temperature: ");
  Serial.print(bme280_values.temperature);
  Serial.println(" °C");
  Serial.print("[BME280] Pressure:    ");
  Serial.print(bme280_values.pressure);
  Serial.println(" Pa");
  Serial.print("[BME280] Humidity:    ");
  Serial.print(bme280_values.humidity);
  Serial.println(" %");
  
  // ---- CCS811 Readout ----

  CCS811_readValues(&css811_values);
  
  Serial.print("[CCS811] eCO2: ");
  Serial.print(css811_values.eCO2);
  Serial.println(" ppm eCO2");
  Serial.print("[CCS811] eTVOC: ");
  Serial.print(css811_values.eTVOC);
  Serial.println(" ppb eTVOC");
  
  String values = String(bme280_values.temperature, 2) + ";"
                + String(bme280_values.pressure, 8) + ";"
                + String(bme280_values.humidity, 2) + ";"
                + String(css811_values.eCO2) + ";"
                + String(css811_values.eTVOC);
  String api_call = "/api/logValue.php?sensorID=" + sensor_id + "&token=" + sensor_token + "&values=" + values;
//
//  Serial.println(values);
//  Serial.println(api_call);

  // ---- WiFi ----

  // check if WiFi is still connected, if not -> try reconnect
  status = WiFi.status();
  while (status != WL_CONNECTED) {
    Serial.print("[WIFI] Disconnected. Attempting to re-connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    
    delay(5000);
  }
  
  Serial.println("[WiFi] Connect to Server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("[WiFi] Connected to server.");
    // Make a HTTP request:
    //1 5 2 6 7 

    
    client.println(String("GET " + api_call + " HTTP/1.1"));
    client.println("Host: asperger.home");
    client.println("Connection: close");
    client.println();
    
    delay(100);
    

    // wait for server response, max 5s
    for(int i = 0; i < 5; i++){
      if(client.available()){
        break;
      }
      else{
        delay(1000);
      }
    }

    // read available data
    bool complete = false;
    String resp_check = "200 OK";
    char resp[] = "      ";
    while (client.available()) {
      char c = client.read();
      //Serial.write(c);

      for(int i = 0; i < 5; i++){
        resp[i] = resp[i+1];
      }
      resp[5] = c;
      
      
      if(resp == resp_check){
        complete = true;
      }
    }
    if(complete){
      //Serial.println();
      Serial.println("[WiFi] Data uploaded.");
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    //while (true);
  }

  // wait 1 minute
  delay(10000);
  delay(10000);
  delay(10000);
  delay(10000);
  delay(10000);
  delay(10000);
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("[WIFI] SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("[WIFI] IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("[WIFI] signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

int readByteFrom(byte Sensor, byte Register) {
  int result = 0;
  Wire.beginTransmission(Sensor); // Begin transmission to the Sensor
  //Ask the particular registers for data
  Wire.write(Register);
  Wire.endTransmission();

  Wire.requestFrom(Sensor, 1); // Request the transmitted two bytes from the two registers

  while (Wire.available()) { //
    result = Wire.read(); // Reads the data from the register
  }
  return result;
}

int writeByteTo(byte Sensor, byte Register, byte Data) {
  Wire.beginTransmission(Sensor);
  Wire.write(Register);
  Wire.write(Data);
  return Wire.endTransmission();
}
int writeByte(byte Sensor, byte Data){
  Wire.beginTransmission(Sensor);
  Wire.write(Data);  
  return Wire.endTransmission();
}

int read20bitFrom(byte Sensor, byte Register) {
  int result = 0;
  Wire.beginTransmission(Sensor); // Begin transmission to the Sensor
  //Ask the particular registers for data
  Wire.write(Register);
  Wire.endTransmission();

  Wire.requestFrom(Sensor, 3); // Request the transmitted three bytes from the two registers

  if (Wire.available() == 3) {
    for (int i = 0; i < 3; i++) {
      byte temp = Wire.read();
      result += (int)(((int)temp & 0xFF) << (2 - i) * 8);
    }
  }
  Wire.endTransmission();
  return result >> 4;
}

int read16bitFrom(byte Sensor, byte Register) {
  int result = 0;
  Wire.beginTransmission(Sensor); // Begin transmission to the Sensor
  //Ask the particular registers for data
  Wire.write(Register);
  Wire.endTransmission();

  Wire.requestFrom(Sensor, 2); // Request the transmitted two bytes from the two registers

  if (Wire.available() == 2) {
    for (int i = 0; i < 2; i++) {
      byte temp = Wire.read();
      result += (int)(((int)temp & 0xFF) << (1 - i) * 8);
    }
  }
  Wire.endTransmission();
  return result;
}

uint64_t read8byteFrom(byte Sensor, byte Register){
  uint64_t result = 0;
  Wire.beginTransmission(Sensor); // Begin transmission to the Sensor 
  //Ask the particular registers for data
  Wire.write(Register);
  Wire.endTransmission();
  
  Wire.requestFrom(Sensor, 8); // Request the transmitted two bytes from the two registers
  
  if(Wire.available() == 8) {
    for(int i = 7; i >= 0; i--){
      byte temp = Wire.read();
      result |= (uint64_t)(((uint64_t)temp & 0xFF) << i*8);
    }
  }
  Wire.endTransmission();
  return result;
}

void BME280_setupSensor() {
  // Oversampling * 1 for humidity
  writeByteTo(BME280_ADDR, BME280_REG_CTRL_HUM, BME280_VAL_CTRL_HUM_OS1);
  //Serial.println("[BME280] Humidity setup: ");
  // normal mode, os * 1 for temp and pressure
  writeByteTo(BME280_ADDR, BME280_REG_CTRL_MEAS, BME280_VAL_CTRL_MEAS_MODE_NORMAL | BME280_VAL_CTRL_MEAS_TEMP_OS1 | BME280_VAL_CTRL_MEAS_PRES_OS1);
  //Serial.println("[BME280] Temp, pressure setup: ");
  // filter off, sample every 1s
  writeByteTo(BME280_ADDR, BME280_REG_CONFIG, BME280_VAL_CONFIG_FILTER_2 | BME280_VAL_CONFIG_TSB_1000);
}

// loading the calibration values from the sensors on-chip memory
void BME280_loadCalibration() {
  //temp
  dig_T1 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_T1_LSB);
  dig_T1 += readByteFrom(BME280_ADDR, BME280_REG_DIG_T1_MSB) << 8;
  dig_T2 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_T2_LSB);
  dig_T2 += readByteFrom(BME280_ADDR, BME280_REG_DIG_T2_MSB) << 8;
  dig_T3 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_T3_LSB);
  dig_T3 += readByteFrom(BME280_ADDR, BME280_REG_DIG_T3_MSB) << 8;

  dig_P1 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P1_LSB);
  dig_P1 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P1_MSB) << 8;
  dig_P2 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P2_LSB);
  dig_P2 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P2_MSB) << 8;
  dig_P3 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P3_LSB);
  dig_P3 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P3_MSB) << 8;
  dig_P4 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P4_LSB);
  dig_P4 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P4_MSB) << 8;
  dig_P5 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P5_LSB);
  dig_P5 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P5_MSB) << 8;
  dig_P6 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P6_LSB);
  dig_P6 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P6_MSB) << 8;
  dig_P7 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P7_LSB);
  dig_P7 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P7_MSB) << 8;
  dig_P8 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P8_LSB);
  dig_P8 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P8_MSB) << 8;
  dig_P9 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_P9_LSB);
  dig_P9 += readByteFrom(BME280_ADDR, BME280_REG_DIG_P9_MSB) << 8;

  dig_H1 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_H1);
  dig_H2 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_H2_LSB);
  dig_H2 += readByteFrom(BME280_ADDR, BME280_REG_DIG_H2_MSB) << 8;
  dig_H3 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_H3);
  dig_H4 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_H4_LSB) << 4;
  dig_H4 += readByteFrom(BME280_ADDR, BME280_REG_DIG_H4_H5) & 0b00001111;
  dig_H5 =  (readByteFrom(BME280_ADDR, BME280_REG_DIG_H4_H5) & 0b11110000) >> 4;
  dig_H5 += readByteFrom(BME280_ADDR, BME280_REG_DIG_H5_MSB) << 4;
  dig_H6 =  readByteFrom(BME280_ADDR, BME280_REG_DIG_H6);
}

void BME280_printSetup() {
  Serial.print("\n");
  Serial.print("[BME280] Setup: ");
  int value = readByteFrom(BME280_ADDR, BME280_REG_CTRL_HUM);
  Serial.print(value, BIN);
  Serial.print(" ");
  value = readByteFrom(BME280_ADDR, BME280_REG_CTRL_MEAS);
  Serial.print(value, BIN);
  Serial.print(" ");
  value = readByteFrom(BME280_ADDR, BME280_REG_CONFIG);
  Serial.print(value, BIN);
  Serial.print("\n");
  Serial.print("\n");
}

void BME280_readValues(bme280_data_t* values) {
  int raw_pressure = read20bitFrom(BME280_ADDR, BME280_REG_PRES_MSB);
  int raw_temperature = read20bitFrom(BME280_ADDR, BME280_REG_TEMP_MSB);
  int raw_humidity = read16bitFrom(BME280_ADDR, BME280_REG_HUM_MSB);
  values->temperature = (float)(BME280_compensate_T_int32(raw_temperature) / (float)100);
  values->pressure = (float)(BME280_compensate_P_int64(raw_pressure) / (float)256);
  values->humidity = (float)(BME280_compensate_H_int32(raw_humidity) / (float)1024);
}

// compensation formulas, as given in the datasheet

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int t_fine;
int32_t BME280_compensate_T_int32(int32_t adc_T) {
  int32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t BME280_compensate_P_int64(int32_t adc_P) {
  int64_t var1, var2, p;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
  var2 = var2 + (((int64_t)dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
  if (var1 == 0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
  return (uint32_t)p;
}
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
uint32_t BME280_compensate_P_int32(int32_t adc_P) {
  int32_t var1, var2;
  uint32_t p;
  var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11 ) * ((int32_t)dig_P6);
  var2 = var2 + ((var1 * ((int32_t)dig_P5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)dig_P4) << 16);
  var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((int32_t)dig_P1)) >> 15);
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if (p < 0x80000000) {
    p = (p << 1) / ((uint32_t)var1);
  }
  else {
    p = (p / (uint32_t)var1) * 2;
  }
  var1 = (((int32_t)dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
  var2 = (((int32_t)(p >> 2)) * ((int32_t)dig_P8)) >> 13;
  p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
  return p;
}
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
uint32_t BME280_compensate_H_int32(int32_t adc_H) {
  int32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((int32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                 ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r *
                     ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                     ((int32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (uint32_t)(v_x1_u32r >> 12);
}

int CCS811_readByteAndPrint(byte Register, String text){
  int result = readByteFrom(CCS811_ADDR, Register);
  Serial.print(text);
  Serial.println(result, HEX);
  return result;
}

void CCS811_swReset(){
  // measure every 10s
  writeByte(CCS811_ADDR, CCS811_REG_SW_RESET);
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(CCS811_REG_SW_RESET);
  Wire.write(0x11);
  Wire.write(0xE5);
  Wire.write(0x72);
  Wire.write(0x8A);
  Wire.endTransmission();
}

void CCS811_setupSensor(){
  CCS811_wake_up();
  CCS811_swReset();

  delay(100);
  
  CCS811_readByteAndPrint(CCS811_REG_HW_ID, "[CCS811] HW ID (should be 0x81): ");
  CCS811_readByteAndPrint(CCS811_REG_HW_VERSION, "[CCS811] HW Version (should be 0x1X): ");
  CCS811_readByteAndPrint(CCS811_REG_STATUS, "[CCS811] Status (should be 0x10): ");
  CCS811_readByteAndPrint(CCS811_REG_FW_APP_VERSION, "[CCS811] FW App Version: ");
  
  // start app
  writeByte(CCS811_ADDR, CCS811_REG_APP_START);

  int result = CCS811_readByteAndPrint(CCS811_REG_STATUS, "[CCS811] Status (should be 0x90): ");
  CCS811_wake_down();

  if(result != 0x90){
    Serial.println("[CCS811] Wrong Status. Error while setting up CCS811.");
    while(true);
  }
}

void CCS811_startSensor(){
  CCS811_wake_up();
  writeByteTo(CCS811_ADDR, CCS811_REG_MEAS_MODE, CCS811_VAL_DRIVE_MODE_2);
  CCS811_wake_down();
}

void CCS811_wake_up(){
  digitalWrite(2, LOW);
}
void CCS811_wake_down(){
  digitalWrite(2, HIGH);
}

void CCS811_readValues(ccs811_data_t* values){
  CCS811_wake_up();
  uint64_t data = read8byteFrom(CCS811_ADDR, CCS811_REG_ALG_RESULT_DATA);
  CCS811_wake_down();

  uint16_t eCO2 = (uint16_t)(data >> 48);
  uint16_t eTVOC = (uint16_t)(data >> 32);
  uint8_t stat = (uint8_t)(data >> 24);
  uint8_t error_id = (uint8_t)(data >> 16);
  uint16_t raw_data = (uint16_t)(data);

  values->eCO2 = eCO2;
  values->eTVOC = eTVOC;
  values->stat = stat;
  values->error_id = error_id;
  values->raw_data = raw_data;
}
