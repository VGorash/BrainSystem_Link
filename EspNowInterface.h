#ifdef USE_ESP_NOW_LINK

#ifndef ESP_NOW_INTERFACE_H
#define ESP_NOW_INTERFACE_H

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

// wireless codes (1 byte header + 1 byte link command)
#define LINK_WIRELESS_HEADER_COMMAND_V2 0x02 // V2 link command

// wireless codes (1 byte header + 1 byte service command)
#define LINK_WIRELESS_HEADER_PING_REQUEST 0x80 // payload - any byte
#define LINK_WIRELESS_HEADER_PING_RESPONSE 0x81 // payload - byte from ping request
#define LINK_WIRELESS_HEADER_PAIRING_REQUEST 0x82 // payload - device code (see below)
#define LINK_WIRELESS_HEADER_PAIRING_RESPONSE 0x83 // payload - device number in server list
#define LINK_WIRELESS_HEADER_BATTERY_INFO 0x84 // payload - battery percentage (0-100)

// wireless device types (1 byte)
#define LINK_WIRELESS_DEVICE_MASTER 0x00
#define LINK_WIRELESS_DEVICE_BUTTON 0x01
#define LINK_WIRELESS_DEVICE_DISPLAY 0x02

namespace vgs::link
{

class EspNowHandler
{
protected:
  virtual void onCommandV2(const uint8_t* address, uint8_t data){}
  virtual void onPingRequest(const uint8_t* address, uint8_t data){}
  virtual void onPingResponse(const uint8_t* address, uint8_t data){}
  virtual void onPairingRequest(const uint8_t* address, uint8_t data){}
  virtual void onPairingResponse(const uint8_t* address, uint8_t data){}
  virtual void onBatteryInfo(const uint8_t* address, uint8_t data){}
  
public:
  void handleEspNowMessage(const uint8_t* address, uint8_t header, uint8_t data)
  {
    switch(header)
    {
      case LINK_WIRELESS_HEADER_COMMAND_V2:
        onCommandV2(address, data);
        break;
      case LINK_WIRELESS_HEADER_PING_REQUEST:
        onPingRequest(address, data);
        break; 
      case LINK_WIRELESS_HEADER_PING_RESPONSE:
        onPingResponse(address, data);
        break; 
      case LINK_WIRELESS_HEADER_PAIRING_REQUEST:
        onPairingRequest(address, data);
        break; 
      case LINK_WIRELESS_HEADER_PAIRING_RESPONSE:
        onPairingResponse(address, data);
        break; 
      case LINK_WIRELESS_HEADER_BATTERY_INFO:
        onBatteryInfo(address, data);
        break;
	}
  }
};

class EspNowInterface
{

public:
  static EspNowInterface* getInstance()
  {
    static EspNowInterface* instance = new EspNowInterface();
    static bool initialized = false;
    if(!initialized)
    {
      esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len){instance->onDataRecv(info, data, len);});
      initialized = true;
    }

    return instance;
  }
  
  void setHandler(EspNowHandler* handler)
  {
    m_handler = handler;
  }
  
  void send(const uint8_t* address, uint8_t header, uint8_t data)
  {
    if(!esp_now_is_peer_exist(address))
    {
      addPeer(address);
    }
	
    uint8_t sendData[2];
    sendData[0] = header;
    sendData[1] = data;
    esp_now_send(address, sendData, 2);
  }
  
private:  
  EspNowInterface()
  {
    WiFi.mode(WIFI_STA);
    WiFi.setChannel(1);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B);
    esp_now_init();
  };
  
  void addPeer(const uint8_t* address)
  {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, address, 6);
    peer.channel = 1;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    esp_now_rate_config_t config = {};
    config.phymode = WIFI_PHY_MODE_LR;
    config.rate = WIFI_PHY_RATE_LORA_250K;
    config.ersu = false;  
    config.dcm = false;
    esp_now_set_peer_rate_config(address, &config);
  }
  
  void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
  {
    if(len == 2 && m_handler)  // all correct packages have size 2 (header + data)
    {
      m_handler->handleEspNowMessage(info->src_addr, data[0], data[1]);
    }
  }
  
private:  
  EspNowHandler* m_handler = nullptr;
};

} // namespace vgs::link

#endif
#endif