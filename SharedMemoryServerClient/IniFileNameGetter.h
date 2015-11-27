#pragma once

using namespace std;

class IFileNameGetter {
public:
	virtual const vector<string>&  getFileList() = 0;
	virtual ~IFileNameGetter() {};
};

class IniNameGetter : public IFileNameGetter {
public:
	const vector<string>&  getFileList();
	IniNameGetter(string fname);
protected:
	vector<string> fileList;
};

