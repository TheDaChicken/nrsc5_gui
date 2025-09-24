#include "Log.h"
#include "spdlog/async.h"

#include <iostream>
#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async_logger.h>

const std::string LogPattern = "[%Y-%m-%d %T.%e] [%^%l%$] %v";

Logger::Logger()
	: m_sinks(
		  std::make_shared<spdlog::sinks::stdout_color_sink_st>()
	  ),
	  m_defaultLogger(CreateLogger("default"))
{
	// register the default logger with spdlog
	spdlog::set_default_logger(m_defaultLogger);

	// set the formatting pattern globally
	spdlog::set_pattern(LogPattern);

	// flush on debug logs
	spdlog::flush_on(spdlog::level::debug);

	// set the log level
	spdlog::set_level(spdlog::level::debug);
}

std::shared_ptr<spdlog::logger> Logger::CreateLogger(const std::string &loggerName)
{
	spdlog::init_thread_pool(8192, 1);

	// create the logger
	auto logger = std::make_shared<spdlog::async_logger>(loggerName,
														 m_sinks,
														 spdlog::thread_pool(),
														 spdlog::async_overflow_policy::block);

	// initialize the logger
	spdlog::initialize_logger(logger);

	return logger;
}

Logger::~Logger()
{
	// flush all loggers
	spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger)
	{
		logger->flush();
	});
	spdlog::drop("default");
}

Logger &Logger::GetInstance()
{
	static Logger instance;
	return instance;
}

spdlog::level::level_enum Logger::MapLogLevel(int level)
{
	switch (level)
	{
		case LOGTRACE: return spdlog::level::trace;
		case LOGDEBUG: return spdlog::level::debug;
		case LOGINFO: return spdlog::level::info;
		case LOGWARNING: return spdlog::level::warn;
		case LOGERROR: return spdlog::level::err;
		case LOGFATAL: return spdlog::level::critical;
		case LOGNONE: return spdlog::level::off;
		default: break;
	}

	return spdlog::level::info;
}

