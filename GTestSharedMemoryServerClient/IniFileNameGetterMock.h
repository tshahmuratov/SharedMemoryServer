#include "IniFileNameGetter.h"

class IniFileNameGetterMock : public IFileNameGetter {
public:
	const vector<string>&  getFileList();
	IniFileNameGetterMock(vector<string> in_fileList);
private:
	vector<string> m_fileList;
};