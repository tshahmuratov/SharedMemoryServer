#include "stdafx.h"
#include "LoggerAdapter.h"

void LoggerAdapter::log(string message, logging::v2s_mt_nt5::trivial::severity_level lvl) {
	BOOST_LOG_SEV(lg, lvl) << message;
}

LoggerAdapter::LoggerAdapter(string filename) {
	stringstream ss;
	ss << filename << "_%N.log";
	logging::add_file_log (
		keywords::file_name = ss.str().c_str(),                                        /*< file name pattern >*/
        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
        keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
    );

    logging::core::get()->set_filter (
        logging::trivial::severity >= logging::trivial::info
    );

	logging::add_common_attributes();
}