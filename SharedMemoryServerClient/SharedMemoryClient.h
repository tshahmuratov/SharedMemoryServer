#pragma once
#include "SharedMemoryNameService.h"
#include "LoggerAdapter.h"
#include "IniFileNameGetter.h"
#include "Sync.h"

class SharedMemoryClient {
public:	
	void sendCommand(const Command &c);
	SharedMemoryClient(ISharedMemoryNameService&, ILoggerAdapter&, IFileNameGetter &);
    void sendFiles();
protected:
	ISharedMemoryNameService &nameService;
	ILoggerAdapter &logger;
	IFileNameGetter &fileNameGetter;

	string sharedGeneralName;
	string sharedFileIdQueueName;
	
	Sync * data;

	string serverLockFileName;

	unique_ptr<message_queue> mqueue;
	unique_ptr<mapped_region> region;
	unique_ptr<boost::interprocess::file_lock> serverFlock;

	void initSharedNames();
	void checkServerIsRunning();
	void initGeneralSharedMemory();
	void initFileIdSharedQueue();

	size_t bufferLength;
	boost::uuids::uuid serverUUID;

	void sendFile(const string &);
	void copyCommand(const Command &src, Command &dst);
	size_t getFileId();
};