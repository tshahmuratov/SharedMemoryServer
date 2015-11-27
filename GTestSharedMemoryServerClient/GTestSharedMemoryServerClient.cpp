// GTestSharedMemoryServerClient.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include "gtest/gtest.h"
#include "SharedMemoryNameService.h"
#include "IniFileNameGetter.h"
#include "LoggerAdapter.h"
#include "consts.h"
#include "IniFileNameGetterMock.h"
#include "LoggerAdapterMock.h"
#include "SharedMemoryNameServiceMock.h"
#include "SharedMemoryServer.h"
#include "SharedMemoryClient.h"
#include <thread>
#include <condition_variable>

TEST(SharedMemoryNameServiceTest, TestGetSharedName)
{
	SharedMemoryNameService service, service2;
	string result = service.getName();
	EXPECT_TRUE(result.size()>0);
	string result2 = service2.getName();
	EXPECT_EQ(result, result2);
}

TEST(IniFileReaderTest, TestIniNameGetter)
{
	boost::filesystem::path fullPath(boost::filesystem::current_path());
	stringstream ss;
	string fullPathString(fullPath.string());
	ss << fullPathString << "/" << INI_FILE_NAME;
	std::ofstream out(ss.str());
	out << "[fileList]" << std::endl << "list=file1.txt,file2.txt,русское имя с пробелом.txt" << std::endl;
	out.close();
	IniNameGetter iniGetter(ss.str());
	auto fileList = iniGetter.getFileList();
	EXPECT_EQ((size_t)3, fileList.size());
	EXPECT_EQ(string("file1.txt"), fileList[0]);
	EXPECT_EQ(string("file2.txt"), fileList[1]);
	EXPECT_EQ(string("русское имя с пробелом.txt"), fileList[2]);
}

TEST(LoggerAdapterTest, TestLog)
{
	boost::filesystem::path fullPath(boost::filesystem::current_path());
	stringstream ss;
	string fullPathString(fullPath.string());
	ss << fullPathString << "/test_0.log";
	if (boost::filesystem::exists(ss.str().c_str())) {
		boost::filesystem::remove(ss.str().c_str());
	}
	LoggerAdapter logger("test");
	logger.log("test error", logging::trivial::error);
	EXPECT_TRUE(boost::filesystem::exists(ss.str().c_str()));
}

TEST(SharedMemoryClientServerTest, TestCreateServer)
{
	vector<string> v;
	condition_variable cv;
	mutex mtx;
	unique_lock<mutex> lck(mtx);
	thread serverThread([&]() {
		LoggerAdapterMock logger;
		SharedMemoryNameServiceMock nameService;
		SharedMemoryServer s1(nameService, logger);
		cv.notify_one();
		s1.startMainLoop();
	});
	cv.wait(lck);
	LoggerAdapterMock logger;
	SharedMemoryNameServiceMock nameService;
	IniFileNameGetterMock iniGetter(v);
	SharedMemoryClient client(nameService, logger, iniGetter);
	Command c;
	c.operationType = c.KILL_SERVER;
	client.sendCommand(c);
	serverThread.join();
}

TEST(SharedMemoryClientServerTest, TestMultipleServers)
{
	vector<string> v;
	IniFileNameGetterMock iniGetter(v);
	condition_variable cv;
	mutex mtx;
	unique_lock<mutex> lck(mtx);
	thread serverThread([&]() {
		LoggerAdapterMock logger;
		SharedMemoryNameServiceMock nameService;
		SharedMemoryServer s1(nameService, logger);
		cv.notify_one();
		s1.startMainLoop();
	});
	cv.wait(lck);
	auto func = [this] {
		LoggerAdapterMock logger;
		SharedMemoryNameServiceMock nameService;
		SharedMemoryServer s2(nameService, logger);
		s2.startMainLoop();
	};
	EXPECT_THROW(func(), exception);
	LoggerAdapterMock logger;
	SharedMemoryNameServiceMock nameService;
	SharedMemoryClient client(nameService, logger, iniGetter);
	Command c;
	c.operationType = c.KILL_SERVER;
	client.sendCommand(c);
	serverThread.join();
}

TEST(SharedMemoryClientServerTest, TestSendFiles)
{
	LoggerAdapterMock logger;
	SharedMemoryNameServiceMock nameService;
	vector<string> v;
	boost::filesystem::create_directory("server");

	v.emplace_back(boost::filesystem::system_complete("test1.txt").string());
	v.emplace_back(boost::filesystem::system_complete("test2.txt").string());

	boost::filesystem::current_path(boost::filesystem::system_complete("server"));
	for (auto &file : v) {
		std::ofstream out(file);
		out << file;
		out.close();
	}
	condition_variable cv;
	mutex mtx;
	unique_lock<mutex> lck(mtx);
	IniFileNameGetterMock iniGetter(v);
	thread serverThread([&]() {
		LoggerAdapterMock logger2;
		SharedMemoryServer s1(nameService, logger2);
		cv.notify_one();
		s1.startMainLoop();
	});
	cv.wait(lck);
	SharedMemoryClient client(nameService, logger, iniGetter);
	client.sendFiles();
	Command c;
	c.operationType = c.KILL_SERVER;
	client.sendCommand(c);
	for (auto &clientFile : v) {
		boost::filesystem::path p(clientFile.c_str());
		string baseName = p.filename().string();
		string serverFile = boost::filesystem::system_complete(baseName.c_str()).string();
		//check file transfered
		EXPECT_TRUE(boost::filesystem::exists(serverFile.c_str()));
		ifstream lFile(serverFile);
		ifstream rFile(clientFile);
		char lBuffer[SHARED_BUFFER_SIZE];
		char rBuffer[SHARED_BUFFER_SIZE];
		size_t numberOfRead;
		bool isEqual = true;
		do {
			lFile.read(lBuffer, SHARED_BUFFER_SIZE);
			rFile.read(rBuffer, SHARED_BUFFER_SIZE);
			numberOfRead = lFile.gcount();//I check the files with the same size

			if (std::memcmp(lBuffer, rBuffer, numberOfRead) != 0)
			{
				memset(lBuffer, 0, numberOfRead);
				memset(rBuffer, 0, numberOfRead);
				isEqual = false;
				break;
			}
		} while (lFile.good() || rFile.good());
		EXPECT_TRUE(isEqual);
	}
	serverThread.join();
}