#include "UartLink.h"

// link codes (each 1-byte)
#define LINK_BUTTON_PRESSED 0x00 // 0000xxxx, x - player number(0000 - 1111, 16 players). V1: [0, 3], V2: [0, 15]
#define LINK_CLEAR 0x10 // 0001xxxx, no data. V1, V2
#define LINK_CORRECT_PRESS_SIGNAL 0x20 // 0010xxxx, x - player number(0000 - 1111, 16 players). V1: [4, 7], V2: [0, 15]
#define LINK_FALSTART_PRESS_SIGNAL 0x30 // 0011xxxx, x - player number(0000 - 1111, 16 players) V1: [4, 7], V2: [0, 15]
#define LINK_GAME_START_SIGNAL 0x40 // 0100xxxx, no data. V1, V2
#define LINK_DISPLAY_CORRECT_PRESS_SIGNAL 0x50 // 0101xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_DISPLAY_FALSTART_PRESS_SIGNAL 0x60 // 0110xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_PENDING_PRESS_SIGNAL 0x70 //0111xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_UPDATE_TIME 0x80 // 1xxxxxxx - x time in seconds (0000000 - 1111111, 0-127 seconds) V1, V2: [0, 127]

using namespace vgs::link;

constexpr int numCommands = 8;

constexpr char uartCommands[numCommands] = 
{
  LINK_BUTTON_PRESSED,
  LINK_CLEAR,
  LINK_CORRECT_PRESS_SIGNAL,
  LINK_DISPLAY_CORRECT_PRESS_SIGNAL,
  LINK_FALSTART_PRESS_SIGNAL,
  LINK_DISPLAY_FALSTART_PRESS_SIGNAL,
  LINK_PENDING_PRESS_SIGNAL,
  LINK_GAME_START_SIGNAL
};

constexpr Command commands[numCommands] = 
{
  Command::ButtonPressed,
  Command::Clear,
  Command::CorrectPressSignal,
  Command::DisplayCorrectPressSignal,
  Command::FalstartPressSignal, 
  Command::DisplayFalstartPressSignal,
  Command::PendingPressSignal,
  Command::GameStartSignal
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
  if(m_command == Command::CorrectPressSignal || m_command == Command::FalstartPressSignal)
  {
    m_data = m_data % 4;
  }
  
  if(m_command == Command::DisplayCorrectPressSignal || m_command == Command::DisplayFalstartPressSignal || m_command == Command::PendingPressSignal) // not supported in v1
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

  if(command == Command::DisplayCorrectPressSignal)
  {
    return; // not supported in v1;
  }

  if(command == Command::DisplayFalstartPressSignal)
  {
    return; // not supported in v1;
  }

  if(command == Command::CorrectPressSignal || command == Command::FalstartPressSignal)
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
