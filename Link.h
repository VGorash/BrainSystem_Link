#ifndef LINK_H
#define LINK_H

namespace vgs::link
{
  enum class Command
  {
    None = 0,
    ButtonPressed,
    CorrectPressSignal,
    DisplayCorrectPressSignal,  // can be used to communicate with external display, allows broadcasting
    FalstartPressSignal, 
    DisplayFalstartPressSignal, // can be used to communicate with external display, allows broadcasting
    PendingPressSignal,
    GameStartSignal,
    Clear,
    UpdateTime
  };

  class Link
  {
  public:
    // interface methods
    virtual void tick() = 0; // perform communication operations
    virtual Command getCommand() = 0;
    virtual unsigned int getData() = 0;
    virtual void send(Command command, unsigned int data = 0) = 0;
	
  protected:
    // conversion methods
    bool commandToCode(Command command, unsigned int data, unsigned char& outCode); // return true if command is not None, else false
	bool codeToCommand(unsigned char code, Command& outCommand, unsigned int& outData);  // return true if command is not None, else false

  public:
    static const int maxPlayers = 16;
  };

} //namespace vgs::link

#endif
