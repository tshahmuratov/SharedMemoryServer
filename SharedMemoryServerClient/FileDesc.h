#pragma once
#include <fstream>
using namespace std;

struct FileDesc {
	ofstream fs;
	string fname;
};