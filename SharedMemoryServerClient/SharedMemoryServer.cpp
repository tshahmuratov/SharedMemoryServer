#include "stdafx.h"
#include "SharedMemoryServer.h"
#include "Command.h"



using namespace logging::trivial;

SharedMemoryServer::SharedMemoryServer(ISharedMemoryNameService &_nameService, ILoggerAdapter &_logger) : nameService(_nameService), logger(_logger) {
	initSyncFileNames();
	initFileLocks();
	initSharedNames();
	initGeneralSharedMemory();
	initFileIdSharedQueue();
	fillQueueWithEmptyFileIds();
	
}

//Получение имен shared memory
void SharedMemoryServer::initSharedNames() {
	string sharedNameBase = nameService.getName();
	sharedGeneralName = sharedNameBase + "General";
	sharedFileIdQueueName = sharedNameBase + "Queue";
}

//Подготовка файлов для синхронизации.
void SharedMemoryServer::initSyncFileNames() {
	boost::filesystem::path fullPath( boost::filesystem::current_path() );
	stringstream ss;
	string fullPathString(fullPath.string());
	ss << fullPathString << "/" << SERVER_LOCK_FILE_NAME;
	serverLockFileName = ss.str();
}

//инициализация файловых локов
void SharedMemoryServer::initFileLocks() {
	//file lock сервера
	ofstream file(serverLockFileName.c_str());
	serverFlock.reset(new boost::interprocess::file_lock(serverLockFileName.c_str()));
	if (!serverFlock->try_lock()) {
		throw exception("сервер уже запущен");
	}
}

//Инициализация общей sharedMemory
void SharedMemoryServer::initGeneralSharedMemory() {
	shared_memory_object::remove(sharedGeneralName.c_str());
	shared_memory_object shm(create_only, sharedGeneralName.c_str(), read_write);
	shm.truncate(sizeof(Sync));
	region.reset(new mapped_region(shm, read_write));
	void * addr       = region->get_address();
	data = new (addr) Sync;
	serverUUID = boost::uuids::random_generator()();
	data->serverUUID = serverUUID;
	strcpy_s(data->serverLockFileName, serverLockFileName.c_str());
}

//инициализация shared memory для очереди
void SharedMemoryServer::initFileIdSharedQueue() {
	scoped_lock<interprocess_mutex> lock(data->queueMutex);
	message_queue::remove(sharedFileIdQueueName.c_str());
	mqueue.reset(new message_queue(create_only, sharedFileIdQueueName.c_str(), SHARED_QUEUE_MAX_SIZE, sizeof(int)));
}

//Наполнение очереди
void SharedMemoryServer::fillQueueWithEmptyFileIds() {
	scoped_lock<interprocess_mutex> e_lock(data->queueMutex);
	//localEmptyFDPoolElements -> sharedQueue
	moveLocalEmptyFdToShared();
	if (mqueue->get_num_msg() < SHARED_QUEUE_MIN_FOR_ENLARGE) {
		size_t last = fdPool.size();
		fdPool.resize(fdPool.size() + LOCAL_QUEUE_RESERVE_STEP);
		for (size_t i = last; i < fdPool.size(); i++) {
			localEmptyFDPoolElements.push(i);
		}
	}
	moveLocalEmptyFdToShared();
}

//boost interprocess message_queue
void SharedMemoryServer::moveLocalEmptyFdToShared() {
	while(localEmptyFDPoolElements.size()) {
		mqueue->send(&localEmptyFDPoolElements.front(), sizeof(size_t), 0);
		localEmptyFDPoolElements.pop();
	}
}

void SharedMemoryServer::checkQueueNeedToBeFilled() {
	if (mqueue->get_num_msg() < SHARED_QUEUE_MIN_FOR_ENLARGE) 
		fillQueueWithEmptyFileIds();
}

//Monitor synchronization pattern
void SharedMemoryServer::startMainLoop() {
	while(true) {
		scoped_lock<interprocess_mutex> lock(data->dataMutex);
		if(!data->message_in){
			data->condEmpty.wait(lock);
		}
		processCommand(data->command);
		checkQueueNeedToBeFilled();
		data->message_in = false;
		data->condFull.notify_one();
		if (data->command.operationType == data->command.KILL_SERVER) break;
	}
}

void SharedMemoryServer::processCommand(Command &c) {
	//TODO: организовать thread pool для приема сообщений
	try {
		if (c.serverUUID != serverUUID) {
			stringstream ss;
			ss << "Найдены сообщения от другого сервера (uuid1-" << c.serverUUID.data << ";uuid2-" << serverUUID.data << ")";
			throw exception(ss.str().c_str());
		}
		switch(c.operationType) {
		case c.SEND_FILENAME:
			getFileName(c);
			break;
		case c.SEND_FILENAME_END:
			getFileNameEnd(c);
			break;
		case c.SEND_FILECONTENT:
			getFileContent(c);
			break;
		case c.SEND_FILECONTENT_END:
			getFileContentEnd(c);
			break;
		}	
	} catch(exception &e) {
		logger.log(e.what(), logging::trivial::error);
	}
}

void SharedMemoryServer::getFileName(Command &c) {
	string s;
	s.reserve(200);
	s.append("Получена часть имени файла:");
	s.append(c.buffer, c.bufferLength > 100 ? 100 : c.bufferLength);
	logger.log(s, logging::trivial::info);
	auto poolElem = getPoolById(c.fileDescId);
	poolElem->fname.append(c.buffer, c.bufferLength);
}

void SharedMemoryServer::getFileNameEnd(Command &c) {
	string s("Получена сигнал о завершении имени файла");
	logger.log(s, logging::trivial::info);
	auto poolElem = getPoolById(c.fileDescId);
	poolElem->fs.open(poolElem->fname, std::ios::binary | std::ios::trunc | std::ios::out);
	if (poolElem->fs.fail()) {
		stringstream ss;
		//не описанное в стандартах поведение установка errno, но в MSVC 2012 работает
		char buff[1001];
		strerror_s(buff, 1000, NULL);
		ss << "Ошибка открытия файла " << poolElem->fname << ": " << buff;
		throw std::exception(ss.str().c_str());
	}
	
}

void SharedMemoryServer::getFileContent(Command &c) {
	string s;
	s.reserve(200);
	s.append("Получена часть содержимого файла:");
	s.append(c.buffer, c.bufferLength > 100 ? 100 : c.bufferLength);
	logger.log(s, logging::trivial::info);
	auto poolElem = getPoolById(c.fileDescId);
	if (!poolElem->fs.is_open()) {
		stringstream ss;
		ss << "Файл для записи " << poolElem->fname << " не открыт";
		throw std::exception(ss.str().c_str());
	}
	string contentPart(c.buffer, c.bufferLength);
	if (!(poolElem->fs << contentPart)) {
		stringstream ss;
		char buff[1001];
		strerror_s(buff, 1000, NULL);
		ss << "Ошибка записи в файл " << poolElem->fname << ": " << buff;
		throw std::exception(ss.str().c_str());
	}
}

void SharedMemoryServer::getFileContentEnd(Command &c) {
	string s("Получена сигнал о завершении содержимого файла");
	logger.log(s, logging::trivial::info);
	auto poolElem = getPoolById(c.fileDescId);
	poolElem->fs.close();
	this->localEmptyFDPoolElements.push(c.fileDescId);
}


FileDesc* SharedMemoryServer::getPoolById(size_t id) {
	if (id >= fdPool.size() || id < 0) {
		stringstream ss;
		ss << "не удалось получить FieldDesc id " << id;
		throw std::exception(ss.str().c_str());
	} else {
		return &fdPool[id];
	}
}