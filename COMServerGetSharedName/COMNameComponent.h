// COMNameComponent.h : Declaration of the CCOMNameComponent

#pragma once
#include "resource.h"       // main symbols

#include "COMServerGetSharedName_i.h"
#include "randomStringGenerator.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;
using namespace std;


// CCOMNameComponent

class ATL_NO_VTABLE CCOMNameComponent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMNameComponent, &CLSID_COMNameComponent>,
	public IDispatchImpl<ICOMNameComponent, &IID_ICOMNameComponent, &LIBID_COMServerGetSharedNameLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CCOMNameComponent()
	{
		if (!m_ssharedName.size()) {
			RandomStringGenerator::generate(m_ssharedName);
		}
		m_sharedName = m_ssharedName;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMNAMECOMPONENT)


BEGIN_COM_MAP(CCOMNameComponent)
	COM_INTERFACE_ENTRY(ICOMNameComponent)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	STDMETHOD(GetSharedMemoryName)(BSTR* SharedName);
protected:
	static wstring m_ssharedName;
	wstring m_sharedName;
};

OBJECT_ENTRY_AUTO(__uuidof(COMNameComponent), CCOMNameComponent)
