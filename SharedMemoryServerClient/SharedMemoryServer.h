#pragma once
#include "SharedMemoryNameService.h"
#include "LoggerAdapter.h"
#include "Sync.h"
#include "FileDesc.h"

//using namespace boost::interprocess;
//https://svn.boost.org/trac/boost/ticket/11426
#pragma warning(disable:4996)
#include <boost/uuid/uuid_generators.hpp> // generators


class SharedMemoryServer {
public:	
	SharedMemoryServer(ISharedMemoryNameService&, ILoggerAdapter&);
    void startMainLoop();
protected:
	ISharedMemoryNameService &nameService;
	ILoggerAdapter &logger;
	Sync * data;

	unique_ptr<mapped_region> region;
	
	std::vector<FileDesc> fdPool;
	queue<size_t> localEmptyFDPoolElements;
	unique_ptr<message_queue> mqueue;

	string sharedGeneralName;
	string sharedFileIdQueueName;

	string serverLockFileName;

	unique_ptr<boost::interprocess::file_lock> serverFlock;

	boost::uuids::uuid serverUUID;

	void initSharedNames();
	void initSyncFileNames();
	void initFileLocks();
	void initGeneralSharedMemory();
	void initFileIdSharedQueue();

	void checkQueueNeedToBeFilled();
	void fillQueueWithEmptyFileIds();
	void moveLocalEmptyFdToShared();
	void processCommand(Command &);
	
	
	void getFileName(Command &);
	void getFileNameEnd(Command &);
	void getFileContent(Command &);
	void getFileContentEnd(Command &);
	FileDesc* getPoolById(size_t);
};