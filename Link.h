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
    virtual void tick() = 0; // perform communication operations
    virtual Command getCommand() = 0;
    virtual unsigned int getData() = 0;
    virtual void send(Command command, unsigned int data = 0) = 0;
    
    static const int maxPlayers = 16;
  };

} //namespace vgs::link

#endif
