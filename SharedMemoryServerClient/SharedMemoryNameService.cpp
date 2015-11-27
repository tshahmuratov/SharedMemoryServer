#include "stdafx.h"
#include "SharedMemoryNameService.h"

SharedMemoryNameService::SharedMemoryNameService() {
	// Declare and HRESULT and a pointer to the Simple_ATL interface
	CoInitialize(NULL); 
	HRESULT	hRes;
	CComPtr<ICOMNameComponent> pINameComponent;
	// Now we will intilize COM
	hRes = pINameComponent.CoCreateInstance(L"SharedMemoryServer.NameComponent.1");
	if(FAILED(hRes)) {
		throw exception("Не удалось подключиться к COM компоненте");
	}
	CComBSTR name;
	pINameComponent->GetSharedMemoryName(&name);
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	sharedMemoryName = converterX.to_bytes(name);
}

string SharedMemoryNameService::getName() {
	return sharedMemoryName;
}