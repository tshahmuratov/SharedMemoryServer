#pragma once
#include "..\COMServerGetSharedName\COMServerGetSharedName_i.h"

using namespace boost::interprocess;
using namespace std;
class ISharedMemoryNameService {
public:
	virtual string getName() = 0;
	virtual ~ISharedMemoryNameService() {};
};

class SharedMemoryNameService : public ISharedMemoryNameService {
public:
	SharedMemoryNameService();
	string getName();
protected:
	string sharedMemoryName;
};