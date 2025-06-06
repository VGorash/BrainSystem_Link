#include "UartLink.h"

// link codes (each 1-byte)
#define LINK_PLAYER_BUTTON 0x00 // 0000xxxx, x - player number(0000 - 1111, 16 players). V1: [0, 3], V2: [0, 15]
#define LINK_LEDS_OFF 0x10 // 0001xxxx, no data. V1, V2
#define LINK_PLAYER_LED_ON 0x20 // 0010xxxx, x - player number(0000 - 1111, 16 players). V1: [4, 7], V2: [0, 15]
#define LINK_PLAYER_LED_BLINK 0x30 // 0011xxxx, x - player number(0000 - 1111, 16 players) V1: [4, 7], V2: [0, 15]
#define LINK_SIGNAL_LED_ON 0x40 // 0100xxxx, no data. V1, V2
#define LINK_DISPLAY_PLAYER_LED_ON 0x50 // 0101xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_DISPLAY_PLAYER_LED_BLINK 0x60 // 0110xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
// 0x70 (0111xxxx) reserved for future use
#define LINK_UPDATE_TIME 0x80 // 1xxxxxxx - x time in seconds (0000000 - 1111111, 0-127 seconds) V1, V2: [0, 127]

using namespace vgs::link;

constexpr int numCommands = 7;

constexpr char uartCommands[numCommands] = 
{
  LINK_PLAYER_BUTTON, 
  LINK_LEDS_OFF, 
  LINK_PLAYER_LED_ON, 
  LINK_DISPLAY_PLAYER_LED_ON, 
  LINK_PLAYER_LED_BLINK, 
  LINK_DISPLAY_PLAYER_LED_BLINK, 
  LINK_SIGNAL_LED_ON
};

constexpr Command commands[numCommands] = 
{
  Command::PlayerButton,
  Command::LedsOff,
  Command::PlayerLedOn,
  Command::DisplayPlayerLedOn,
  Command::PlayerLedBlink, 
  Command::DisplayPlayerLedBlink,
  Command::SignalLedOn
};

UartLink::UartLink(UartLinkVersion version) : m_version(version)
{

}

UartLinkVersion UartLink::getVersion()
{
  return m_version;
}

Command UartLink::getCommand()
{
  return m_command;
}

unsigned int UartLink::getData()
{
  return m_data;
}

void UartLink::tick()
{
  switch(m_version)
  {
    case UartLinkVersion::V1:
    {
      tickV1();
    }
    break;

    case UartLinkVersion::V2:
    {
      tickV2();
    }
    break;
  }
}

void UartLink::send(Command command, unsigned int data)
{
  switch(m_version)
  {
    case UartLinkVersion::V1:
    {
      sendV1(command, data);
    }
    break;

    case UartLinkVersion::V2:
    {
      sendV2(command, data);
    }
    break;
  }
}

void UartLink::tickV1()
{
  tickV2();

  // process difference
  if(m_command == Command::PlayerLedOn || m_command == Command::PlayerLedBlink)
  {
    m_data = m_data % 4;
  }
  
  if(m_command == Command::DisplayPlayerLedOn || m_command == Command::DisplayPlayerLedBlink) // not supported in v1
  {
    m_data = 0;
    m_command == Command::None;
  }
}

void UartLink::tickV2()
{
  m_command = Command::None;
  m_data = 0;

  unsigned char raw;

  if(!readUartData(raw))
  {
    return;
  }

  if(raw >= 0x80) // LINK_UPDATE_TIME 0x80 1xxxxxxx - x time in seconds (0000000 - 1111111, 0-127)
  {
    m_data = raw & 0x7F;
    m_command = Command::UpdateTime;
    return;
  }

  unsigned char command = raw & 0xF0;
  unsigned char payload = raw & 0x0F;

  for(int i=0; i<numCommands; i++)
  {
    if(command == uartCommands[i])
    {
      m_command = commands[i];
      m_data = payload;
      break;
    }
  }
}

void UartLink::sendV1(Command command, unsigned int data)
{
  // process difference

  if(command == Command::DisplayPlayerLedOn)
  {
    return; // not supported in v1;
  }

  if(command == Command::DisplayPlayerLedBlink)
  {
    return; // not supported in v1;
  }

  if(command == Command::PlayerLedOn || command == Command::PlayerLedBlink)
  {
    data = 4 + (data % 4);
  }

  sendV2(command, data);
}

void UartLink::sendV2(Command command, unsigned int data)
{
  if(command == Command::None)
  {
    return;
  }

  if(command == Command::UpdateTime)
  {
    writeUartData((unsigned char)LINK_UPDATE_TIME | (unsigned char)data);
    return;
  }

  for(int i=0; i<numCommands; i++)
  {
    if(command == commands[i])
    {
      writeUartData((unsigned char)uartCommands[i] | ((unsigned char)data & 0x0F));
      break;
    }
  }
}
