#include "Link.h"

using namespace vgs::link;

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

constexpr int numCommands = 8;

constexpr char commandCodes[numCommands] = 
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

bool Link::commandToCode(Command command, unsigned int data, unsigned char& outCode)
{
  if(command == Command::UpdateTime)
  {
    outCode = (unsigned char)LINK_UPDATE_TIME | (unsigned char)data;
	return true;
  }

  for(int i=0; i<numCommands; i++)
  {
    if(command == commands[i])
    {
      outCode = (unsigned char)commandCodes[i] | ((unsigned char)data & 0x0F);
	  return true;
    }
  }
  
  return false;
}

bool Link::codeToCommand(unsigned char code, Command& outCommand, unsigned int& outData)
{
  if(code >= 0x80) // LINK_UPDATE_TIME 0x80 1xxxxxxx - x time in seconds (0000000 - 1111111, 0-127)
  {
    outData = code & 0x7F;
    outCommand = Command::UpdateTime;
	return true;
  }

  unsigned char command = code & 0xF0;
  unsigned char payload = code & 0x0F;

  for(int i=0; i<numCommands; i++)
  {
    if(command == commandCodes[i])
    {
      outData = payload;
	  outCommand = commands[i];
      return true;
    }
  }
  
  return false;
}
