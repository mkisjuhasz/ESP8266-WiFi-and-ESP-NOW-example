#include <ESP8266WiFi.h>
#include <espnow.h>
#include <user_interface.h>

#define IS_AUTO_DETECT_ON 1             //Set 1 to automatically detect the WiFi channel, and 0 to set the channel manually
#define GATEWAY_AP_SSID "AP_esp8266"    //configure gateway access point SSID if IS_AUTO_DETECT_ON set to 1
#define DEFAULT_CHANNEL 7     

uint8_t selectedChannel = DEFAULT_CHANNEL;
static uint8_t PEER[]{0x2C, 0xF4, 0x32, 0x51, 0xD1, 0xA7};  //set this to gateway MAC if IS_AUTO_DETECT_ON is set to 0


#if IS_AUTO_DETECT_ON
   void setPeerMAC(uint8_t pos) {
    uint8_t *newMAC;
    newMAC = WiFi.BSSID(pos);
    memcpy(PEER, newMAC, 6);
    Serial.printf("Setting peer MAC to: ");
    for (uint8_t i = 0; i < 6; i++) {
      Serial.printf("0x%02x ", PEER[i]);
    }
    Serial.println("");
  }
  
  void setChannel(uint8_t pos) {
    selectedChannel = WiFi.channel(pos);
    Serial.printf("\n>> SWITCHING CHANNEL TO %d\n", selectedChannel);
    wifi_set_channel(selectedChannel);
  }
  
  void checkAndUpdateAPConnection() {
    int n = WiFi.scanNetworks();
    for (uint8_t i = 0; i < n; i++) {
      if ((WiFi.SSID(i).compareTo(GATEWAY_AP_SSID) == 0) && (WiFi.channel(i) != selectedChannel)) {
        setChannel(i);
        setPeerMAC(i);
      }
    }
  }
#endif


void handleReceivedMessage(uint8_t *mac, uint8_t *buf, uint8_t length) {
  Serial.println("Message from gateway: ");
  Serial.println((char *) buf);
}


void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.persistent(false);   //DO NOT save WiFi settings into flash
  WiFi.mode(WIFI_STA);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  wifi_set_channel(selectedChannel);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(handleReceivedMessage);
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(PEER, ESP_NOW_ROLE_CONTROLLER, 1, NULL, 0);  //the channel setting here doesn't matter
  Serial.println("Node init completed!");
}

void loop() {
  char message[] = "Hello from node";
  esp_now_send(PEER, (uint8_t *)&message, sizeof(message));
  
  delay(5000);

  #if IS_AUTO_DETECT_ON
    checkAndUpdateAPConnection();
  #endif
}
