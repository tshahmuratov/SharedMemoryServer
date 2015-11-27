#include "stdafx.h"
#include "IniFileNameGetterMock.h"

IniFileNameGetterMock::IniFileNameGetterMock(vector<string> in_fileList) : m_fileList(in_fileList) {

}

const vector<string>&  IniFileNameGetterMock::getFileList() {
	return m_fileList;
}