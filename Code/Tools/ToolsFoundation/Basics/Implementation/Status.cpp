#include <ToolsFoundation/PCH.h>
#include <ToolsFoundation/Basics/Status.h>
#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Logging/Log.h>

ezStatus::ezStatus(const ezFormatString& fmt)
  : m_Result(EZ_FAILURE)
{
  ezStringBuilder sMsg;
  m_sMessage = fmt.GetText(sMsg);
}

void ezStatus::LogFailure(ezLogInterface* pLog)
{
  if (Failed())
  {
    ezLogInterface* pInterface = pLog ? pLog : ezLog::GetDefaultLogSystem();
    ezLog::Error(pInterface, "%s", m_sMessage.GetData());
  }
}
