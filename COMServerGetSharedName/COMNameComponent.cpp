// COMNameComponent.cpp : Implementation of CCOMNameComponent

#include "stdafx.h"
#include "COMNameComponent.h"


// CCOMNameComponent
wstring CCOMNameComponent::m_ssharedName(L"");

STDMETHODIMP CCOMNameComponent::GetSharedMemoryName(BSTR* SharedName)
{
	// TODO: Add your implementation code here
	*SharedName = SysAllocString( this->m_sharedName.c_str() );
	return S_OK;
}
