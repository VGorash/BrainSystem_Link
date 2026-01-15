#include "UartLink.h"

using namespace vgs::link;

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

  unsigned char code;

  if(!readUartData(code))
  {
    return;
  }

  codeToCommand(code, m_command, m_data);
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
  unsigned char code;
  
  if(commandToCode(command, data, code))
  {
    writeUartData(code);
  }
}
