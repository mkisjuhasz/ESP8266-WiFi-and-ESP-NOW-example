#include <ESP8266WiFi.h>
#include <espnow.h>


//you should set these:
#define IS_AUTO_DETECT_ON 1             //Set 1 to automatically detect the WiFi channel, and 0 to set the channel manually
#define GATEWAY_AP_SSID "AP_esp8266"    //configure gateway access point SSID if IS_AUTO_DETECT_ON set to 1
#define STASSID "your router SSID"
#define STAPSK "your password"
static uint8_t PEER[] = {0x2C, 0xF4, 0x32, 0x51, 0xD1, 0x9F}; //MAC address of your node


const char *ssid = STASSID;
const char *password = STAPSK;


void handleReceivedMessage(uint8_t *mac, uint8_t *buf, uint8_t length) {
  Serial.println("Message from node: ");
  Serial.println((char *) buf);
}

#if IS_AUTO_DETECT_ON
  void configDeviceAP() {  
    String SSID = GATEWAY_AP_SSID;
    String Password = "123456789";
    bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), 2, 0);
    if (!result) {
      Serial.println("AP Config failed.");
    }
    else {
      Serial.println("AP Config Success");
    }
  }
#endif

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.persistent(false);   //DO NOT save WiFi settings into flash

  #if IS_AUTO_DETECT_ON
    WiFi.mode(WIFI_AP_STA);
    Serial.println("configured AP STA mode");
  #else 
    WiFi.mode(WIFI_STA);
  #endif
 
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(handleReceivedMessage);
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(PEER, ESP_NOW_ROLE_COMBO, 1, NULL, 0);  //the channel setting here doesn't matter

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println("Gateway init completed!");
  
  #if IS_AUTO_DETECT_ON
     configDeviceAP();
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
  char message[] = "Hello from gateway";
  esp_now_send(PEER, (uint8_t *)&message, sizeof(message));
  delay(5000);
}
