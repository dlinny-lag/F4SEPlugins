#include "MyTrace.h"
#include "winreg.h"
#include "f4se/GameAPI.h"
#include "common/IDebugLog.h"

LogSettings::MyLogLevel LogSettings::LogLevel = LogSettings::MyLogLevel::Warning;
bool LogSettings::TraceToConsole = false;

#define TRACE_TO_CONSOLE_KEY "TraceToConsole"
#define LOG_LEVEL_KEY "LogLevel"

std::string subKey;


std::string LevelToString(LogSettings::MyLogLevel l)
{
	switch (l)
	{
	case LogSettings::MyLogLevel::Error:
		return "ERROR:";
	case LogSettings::MyLogLevel::Warning:
		return "WARNING:";
	case LogSettings::MyLogLevel::Info:
		return "INFO:";
	case LogSettings::MyLogLevel::Debug:
		return "DEBUG:";
	default:
		return "";
	}
}


IDebugLog::LogLevel LevelToInternal(LogSettings::MyLogLevel l)
{
	switch (l)
	{
	case LogSettings::MyLogLevel::Error:
		return IDebugLog::LogLevel::kLevel_Error;
	case LogSettings::MyLogLevel::Warning:
		return IDebugLog::LogLevel::kLevel_Warning;
	case LogSettings::MyLogLevel::Info:
		return IDebugLog::LogLevel::kLevel_Message;
	case LogSettings::MyLogLevel::Debug:
		return IDebugLog::LogLevel::kLevel_DebugMessage;
	default:
		return IDebugLog::LogLevel::kLevel_DebugMessage;
	}
}


void LogSettings::Init(const char* appName)
{
	subKey = "Software\\Dlinny_Lag\\";
	subKey += appName;

	HKEY appKey;
	LSTATUS result;
	if ((result = RegOpenKeyExA(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &appKey)) == ERROR_SUCCESS)
	{
		DWORD keyType = REG_DWORD;
		DWORD value = 4; // Debug
		DWORD dataLen = sizeof(DWORD);
		if ((result = RegGetValueA(appKey, nullptr, LOG_LEVEL_KEY, RRF_RT_REG_DWORD, &keyType, &value, &dataLen)) == ERROR_SUCCESS)
		{
			if (value < MyLogLevel::None)
				value = MyLogLevel::None;
			if (value > MyLogLevel::Debug)
				value = MyLogLevel::Debug;
			LogLevel = (MyLogLevel)value;
		}
		else
		{
			_MESSAGE("Failed to read registry value for %s. ERR=%08X", LOG_LEVEL_KEY, result);
		}
		value = 0;
		if ((result = RegGetValueA(appKey, nullptr, TRACE_TO_CONSOLE_KEY, RRF_RT_REG_DWORD, &keyType, &value, &dataLen)) == ERROR_SUCCESS)
		{
			if (value < 0)
				value = 0;
			if (value > 1)
				value = 1;
			TraceToConsole = (bool)value;
		}
		else
		{
			_MESSAGE("Failed to read registry value for %s. ERR=%08X", TRACE_TO_CONSOLE_KEY, result);
		}
		RegCloseKey(appKey);
	}
	else
	{
		_MESSAGE("Failed to open registry key %s. ERR=%08X", subKey.c_str(), result);
	}
}

void SaveRegValue(const char* name, DWORD value)
{
	HKEY appKey;
	DWORD disposition;
	LSTATUS result;
	if (result = RegCreateKeyExA(HKEY_CURRENT_USER, subKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &appKey, &disposition) == ERROR_SUCCESS)
	{
		if (result = RegSetValueExA(appKey, name, 0, REG_DWORD, (BYTE*)(&value), sizeof(value)) != ERROR_SUCCESS)
		{
			_MESSAGE("Failed to set value for %s. ERR=%08X", name, result);
		}
		RegCloseKey(appKey);
	}
	else
	{
		_MESSAGE("Failed to create/open registry key %s. ERR=%08X", subKey.c_str(), result);
	}
}

void LogSettings::SetLogLevel(MyLogLevel level)
{
	LogLevel = level;
	SaveRegValue(LOG_LEVEL_KEY, (DWORD)level);
	
}
void LogSettings::SetTraceToConsole(bool toConsole)
{
	TraceToConsole = toConsole;
	SaveRegValue(TRACE_TO_CONSOLE_KEY, toConsole);
}


void ConsolePrintV(const char* fmt, va_list args)
{
	ConsoleManager* mgr = *g_console;
	if (mgr)
	{
#if _F4SE_DECLARATIVE_LOAD // function name was changed along with other changes
		CALL_MEMBER_FN(mgr, PrintVArgs)(fmt, args);
#else
		CALL_MEMBER_FN(mgr, VPrint)(fmt, args);
#endif
	}
}

void Dmp(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	gLog.Log(IDebugLog::LogLevel::kLevel_FatalError, fmt, args);

	if (LogSettings::TraceToConsole)
	{
		ConsolePrintV(fmt, args);
	}

	va_end(args);
}


#define WriteLog(level, fmt)\
	{\
		va_list args;\
		va_start(args, fmt);\
		if (level > LogSettings::LogLevel) \
			return; \
		std::string format = LevelToString(level) + fmt;\
		gLog.Log(LevelToInternal(level), format.c_str(), args);\
		if (LogSettings::TraceToConsole)\
		{\
			ConsolePrintV(format.c_str(), args);\
		}\
		va_end(args);\
	}\

#define WriteLogRet(level, fmt, retVal)\
	{\
		va_list args;\
		va_start(args, fmt);\
		if (level > LogSettings::LogLevel) \
			return retVal; \
		std::string format = LevelToString(level) + fmt;\
		gLog.Log(LevelToInternal(level), format.c_str(), args);\
		if (LogSettings::TraceToConsole)\
		{\
			ConsolePrintV(format.c_str(), args);\
		}\
		va_end(args);\
	}\

// log/debug
void D(const char* fmt, ...)
{
	WriteLog(LogSettings::MyLogLevel::Debug, fmt);
}
// log/info
void I(const char* fmt, ...)
{
	WriteLog(LogSettings::MyLogLevel::Info, fmt);
}
// log/warning
void W(const char* fmt, ...)
{
	WriteLog(LogSettings::MyLogLevel::Warning, fmt);
}
// log/error
void E(const char* fmt, ...)
{
	WriteLog(LogSettings::MyLogLevel::Error, fmt);
}
// false/error
bool FE(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Error, fmt, false);
	return false;
}
// false/warning
bool FW(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Warning, fmt, false);
	return false;
}
// false/debug
bool FD(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Debug, fmt, false);
	return false;
}
// false/info
bool FI(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Info, fmt, false);
	return false;
}
// true/warning
bool TW(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Warning, fmt, true);
	return true;
}
// true/error
bool TE(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Error, fmt, true);
	return true;
}
bool TD(const char* fmt, ...)
{
	WriteLogRet(LogSettings::MyLogLevel::Debug, fmt, true);
	return true;
}
const char* S(bool val)
{
	return val ? "true" : "false";
}