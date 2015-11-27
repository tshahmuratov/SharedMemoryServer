#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/record_ostream.hpp>

using namespace std;

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

class ILoggerAdapter {
public:
	virtual void log(string message, logging::v2s_mt_nt5::trivial::severity_level lvl) = 0;
	virtual ~ILoggerAdapter() {};
};

class LoggerAdapter : public ILoggerAdapter {
public:
	void log(string message, logging::v2s_mt_nt5::trivial::severity_level lvl);
	LoggerAdapter(string filename);
protected:
	src::severity_logger< logging::trivial::severity_level > lg;
};

