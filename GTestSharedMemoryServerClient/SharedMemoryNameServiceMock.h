#include "SharedMemoryNameService.h"

class SharedMemoryNameServiceMock : public ISharedMemoryNameService {
public:
	string getName();
};