# ESP8266-WiFi-and-ESP-NOW-example

ESP8266: Using ESP-NOW and WiFi connection together
The goal of this example is to show how can you configure an ESP8266 as a gateway between an ESP-NOW network and the internet. I only tested it on ESP8266 with one node, but I wouldn’t be surprised if it would work like quite a similar way on ESP32. 
 

The key is the channel setting.  Your whole system should use the channel which your router is using. The ESP can only communicate in one channel, so when you connet to an Access Point with the ESP, the channel is automatically set to the one used by the Access Point. To communicate between the ESPs, you should set the channel used by the node similar to gateway channel (Channel 2 on the illustration).

This example is capable of automatically detect the channel of your gateway and router connection.
```c
#define IS_AUTO_DETECT_ON 1
```  
