#include "stdafx.h"
#include "SharedMemoryNameServiceMock.h"

string SharedMemoryNameServiceMock::getName() {
	return "testStringSharedName";
}