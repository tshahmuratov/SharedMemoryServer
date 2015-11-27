#include "stdafx.h"
#include "SharedMemoryClient.h"

SharedMemoryClient::SharedMemoryClient
			(ISharedMemoryNameService& _nameService, ILoggerAdapter& _logger, IFileNameGetter& _fileNameGetter) 
			: nameService(_nameService), logger(_logger), fileNameGetter(_fileNameGetter) {
	initSharedNames();
	initGeneralSharedMemory();
	initFileIdSharedQueue();
	checkServerIsRunning();
}

//Получение имен shared memory
void SharedMemoryClient::initSharedNames() {
	string sharedNameBase = nameService.getName();
	sharedGeneralName = sharedNameBase + "General";
	sharedFileIdQueueName = sharedNameBase + "Queue";
}

//Инициализация общей sharedMemory
void SharedMemoryClient::initGeneralSharedMemory() {
	shared_memory_object shm(open_only, sharedGeneralName.c_str(), read_write);
	region.reset(new mapped_region(shm, read_write));
	void * addr       = region->get_address();
	data = static_cast<Sync*>(addr);
	serverUUID = data->serverUUID;
	bufferLength = data->command.bufferLength;
}

//инициализация shared memory для очереди
void SharedMemoryClient::initFileIdSharedQueue() {
	scoped_lock<interprocess_mutex> lock(data->queueMutex);
	mqueue.reset(new message_queue(open_only, sharedFileIdQueueName.c_str()));
}

void SharedMemoryClient::checkServerIsRunning() {
	serverFlock.reset(new boost::interprocess::file_lock(data->serverLockFileName));
	if (serverFlock->try_lock()) {
		throw exception("сервер не запущен");
	}
}

void SharedMemoryClient::sendFiles() {
	auto files = this->fileNameGetter.getFileList();
	for (const auto &file: files) {
		sendFile(file);
	}
}

void SharedMemoryClient::sendFile(const string &fullFileName) {
	ifstream fs;
	fs.open(fullFileName, std::ios::binary | std::ios::in);
	if (!fs.is_open()) {
		stringstream ss;
		ss << "Не удалось открыть файл " << fullFileName;
		throw exception(ss.str().c_str());
	}
	boost::filesystem::path p(fullFileName.c_str());
	string fileName = p.filename().string();
	Command localCommand;
	localCommand.serverUUID = serverUUID;
	localCommand.bufferLength = bufferLength;
	std::streamsize readCount;
	localCommand.fileDescId = getFileId();
	size_t offset, step = localCommand.BufferLengthConst;
	for (size_t i = 0; i < fileName.size(); i += step) {
		localCommand.operationType = localCommand.SEND_FILENAME;
		offset = i * step;
		localCommand.bufferLength = (offset +  step > fileName.size() ? fileName.size() - offset : step);
		strncpy_s(localCommand.buffer, fileName.c_str(), localCommand.bufferLength);
		sendCommand(localCommand);
	}
	localCommand.operationType = localCommand.SEND_FILENAME_END;
	sendCommand(localCommand);
	while ( fs.read(localCommand.buffer, localCommand.BufferLengthConst)) {
		localCommand.operationType = localCommand.SEND_FILECONTENT;
		localCommand.bufferLength = localCommand.bufferLength;
		sendCommand(localCommand);
	}
	readCount = fs.gcount();
	if (readCount > 0) {
		localCommand.operationType = localCommand.SEND_FILECONTENT;
		localCommand.bufferLength = readCount;
		sendCommand(localCommand);
	}
	localCommand.operationType = localCommand.SEND_FILECONTENT_END;
	sendCommand(localCommand);
}

size_t SharedMemoryClient::getFileId() {
	scoped_lock<interprocess_mutex> e_lock(data->queueMutex);
	unsigned int priority;
	message_queue::size_type recvd_size;
	size_t fileId;
	mqueue->receive(&fileId, sizeof(fileId), recvd_size, priority);
	if(fileId < 0 || recvd_size != sizeof(fileId)) {
		throw exception("Получен не корректный id файла");
	}
	return fileId;
}

void SharedMemoryClient::sendCommand(const Command &c) {
	scoped_lock<interprocess_mutex> lock(data->dataMutex);
    if(data->message_in) {
		data->condFull.wait(lock);
    }
	copyCommand(c, data->command);
    data->condEmpty.notify_one();
    data->message_in = true;
}

void SharedMemoryClient::copyCommand(const Command &src, Command &dst) {
	dst.operationType = src.operationType;
	if (src.operationType == src.SEND_FILENAME || src.operationType == src.SEND_FILECONTENT) {
		strncpy_s(dst.buffer, src.buffer, src.bufferLength);
		dst.bufferLength = src.bufferLength;
	}
	dst.serverUUID = src.serverUUID;
	dst.fileDescId = src.fileDescId;
}
	