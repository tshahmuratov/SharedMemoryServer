#pragma once

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include "Command.h"

using namespace std;
struct Sync {
	Sync() : message_in(false) {
	
	}

	//Conditions
	boost::interprocess::interprocess_condition  condEmpty, condFull;
	//Mutexes
	boost::interprocess::interprocess_mutex      dataMutex, queueMutex;
	
	char serverLockFileName[MAX_PATH];
	
	//Is there any message
	bool message_in;
	boost::uuids::uuid serverUUID;
	Command command;
};