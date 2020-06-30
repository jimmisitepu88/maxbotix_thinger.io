#define RXD2 17
#define TXD2 16

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM7000
//#define TINY_GSM_MODEM_SIM7600

// uncomment line for debug
// #define _DEBUG_

// Can be installed from Library Manager or https://github.com/vshymanskyy/TinyGSM
#include <TinyGsmClient.h>
#include <ThingerTinyGSM.h>

#define USERNAME "user_name"
#define DEVICE_ID "01"
#define DEVICE_CREDENTIAL "device_credential"

// use your own APN config
#define APN_NAME "Internet"
#define APN_USER ""
#define APN_PSWD ""

// set your cad pin (optional)
#define CARD_PIN ""

ThingerTinyGSM thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL, Serial2);

#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads; 

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60 // sleep for 60 seconds
unsigned long cur_time, old_time;

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(Serial2, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(Serial2);
#endif

TinyGsmClient client(modem);

void setup() {

  // uncomment line for debug
   Serial.begin(115200);
   Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
   delay(5000);
   Serial.println("wake up esp");
   //modem.init();
   
   thing.setAPN(APN_NAME, APN_USER, APN_PSWD);

  // set PIN (optional)
  // thing.setPIN(CARD_PIN);
  
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.begin();
  thing["maxbotix"] >> outputValue(ads.readADC_SingleEnded(3));
   
}

void loop() {
  cur_time = millis();
  old_time = cur_time;
  while(cur_time - old_time < 20000){
    cur_time = millis();
    thing.handle();
  }
  sleep_esp();
}

void sleep_esp(){
  modem.restart();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
}
