// SharedMemoryServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LoggerAdapter.h"
#include "SharedMemoryServer.h"
#include "SharedMemoryClient.h"
#include "SharedMemoryNameService.h"
#include "IniFileNameGetter.h"

using namespace std;

void showUsage() {
	cout << "Usage: shm <client|server> <ini_file_name_for_client>";
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc<2) {
		showUsage();
		return 1;
	}
	if (_tcscmp(argv[1], _T("server")) == 0) {
		LoggerAdapter logger(SERVER_LOG_NAME);
		try {
			SharedMemoryNameService nameService;
			SharedMemoryServer server(nameService, logger);
			server.startMainLoop();
		} catch (exception& e)	{
			stringstream ss;
			ss << "Ошибка в сервере: " << e.what();
			logger.log(ss.str().c_str(), logging::trivial::error);
			return 1;
		}
	} else if (_tcscmp(argv[1], _T("client")) == 0) {
		LoggerAdapter logger(CLIENT_LOG_NAME);
		if (argc<3) {
			showUsage();
			return 1;
		}
		try {
			string iniName;
#ifndef UNICODE  
			iniName = argv[2]; 
#else
			typedef std::codecvt_utf8<wchar_t> convert_typeX;
			std::wstring_convert<convert_typeX, wchar_t> converterX;
			iniName = converterX.to_bytes(argv[2]);
#endif
			IniNameGetter iniGetter(iniName);
			SharedMemoryNameService nameService;
			SharedMemoryClient client(nameService, logger, iniGetter);
			client.sendFiles();
		} catch (exception& e)	{
			stringstream ss;
			ss << "Ошибка в сервере: " << e.what();
			logger.log(ss.str().c_str(), logging::trivial::error);
			return 1;
		}
	} else {
		showUsage();
		return 1;
	}
	return 0;
}

