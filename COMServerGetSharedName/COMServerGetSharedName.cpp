// COMServerGetSharedName.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "COMServerGetSharedName_i.h"

using namespace ATL;


class CCOMServerGetSharedNameModule : public ATL::CAtlExeModuleT< CCOMServerGetSharedNameModule >
{
private:
	CComPtr<ICOMNameComponent> pINameComponent;
public :
	DECLARE_LIBID(LIBID_COMServerGetSharedNameLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMSERVERGETSHAREDNAME, "{F778C5FE-E053-4035-BDF3-3C940C976069}")

	HRESULT PreMessageLoop(int nShow);

};

HRESULT CCOMServerGetSharedNameModule::PreMessageLoop(int nShow) {
	HRESULT hr = RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
	//CoCreateInstance(CLSID_COMNameComponent,
    hr = pINameComponent.CoCreateInstance(CLSID_COMNameComponent);
	
	if (hr == S_OK)
    {
        if (m_bDelayShutdown && !StartMonitor())
        {
            hr = E_FAIL;
        }
    }
    else
    {
        m_bDelayShutdown = false;
    }
    return hr;
}

CCOMServerGetSharedNameModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

