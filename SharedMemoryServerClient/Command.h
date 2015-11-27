#pragma once

using namespace std;
struct Command {
	enum { 
		BufferLengthConst = SHARED_BUFFER_SIZE 
	};
	enum { 
		SEND_FILENAME, SEND_FILENAME_END, SEND_FILECONTENT, SEND_FILECONTENT_END, KILL_SERVER
	} operationType;
	
	char buffer[BufferLengthConst];
	int bufferLength;
	size_t fileDescId;
	boost::uuids::uuid serverUUID;
};