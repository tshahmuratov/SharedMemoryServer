#include "LoggerAdapter.h"

class LoggerAdapterMock : public ILoggerAdapter {
public:
	void log(string message, logging::v2s_mt_nt5::trivial::severity_level lvl);
};