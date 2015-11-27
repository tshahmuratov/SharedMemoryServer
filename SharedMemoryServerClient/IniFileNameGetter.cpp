#include "stdafx.h"
#include "IniFileNameGetter.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/typeof/typeof.hpp>

const vector<string>&  IniNameGetter::getFileList() {
	return fileList;
}

IniNameGetter::IniNameGetter(string fname) {
	using boost::property_tree::ptree;
    ptree pt;
	try{
		string iniName = boost::filesystem::system_complete(fname.c_str()).string();
		read_ini(iniName, pt);
		boost::split(this->fileList, pt.get<std::string>("fileList.list"), boost::is_any_of(","), boost::token_compress_on);
	} catch(boost::property_tree::ini_parser_error &e) {
		stringstream ss;
		ss << "Ошибка при обработке " << e.filename() << ", на строке " << e.line() << ": "<< e.message(); 
		throw exception(ss.str().c_str());
	}
}