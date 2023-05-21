#pragma once

namespace LogSettings
{

	enum MyLogLevel
	{
		None = 0,
		Error = 1,
		Warning = 2,
		Info = 3,
		Debug = 4,
	};

	extern MyLogLevel LogLevel;
	extern bool TraceToConsole;
	void Init(const char* appName);
	void SetLogLevel(MyLogLevel level);
	void SetTraceToConsole(bool toConsole);
}

void Dmp(const char* fmt, ...);

// log/debug
void D(const char* fmt, ...);

// log/info
void I(const char* fmt, ...);
// log/warning
void W(const char* fmt, ...);
// log/error
void E(const char* fmt, ...);
// false/error
bool FE(const char* fmt, ...);
// false/warning
bool FW(const char* fmt, ...);
// false/debug
bool FD(const char* fmt, ...);
// false/info
bool FI(const char* fmt, ...);
// true/warning
bool TW(const char* fmt, ...);
// true/error
bool TE(const char* fmt, ...);
// true/debug
bool TD(const char* fmt, ...);

const char* S(bool val);
