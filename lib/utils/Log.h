//
// Created by TheDaChicken on 7/10/2023.
//

#ifndef NRSC5_GUI_LOG_H
#define NRSC5_GUI_LOG_H

#include <iostream>
#include <string>

#include <vector>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/dist_sink.h>

static constexpr int LOGTRACE = -1;
static constexpr int LOGDEBUG = 0;
static constexpr int LOGINFO = 1;
static constexpr int LOGWARNING = 2;
static constexpr int LOGERROR = 3;
static constexpr int LOGFATAL = 4;
static constexpr int LOGNONE = 6;

enum LevelEnum : int
{
	trace = LOGTRACE,
	debug = LOGDEBUG,
	info = LOGINFO,
	warn = LOGWARNING,
	err = LOGERROR,
	fatal = LOGFATAL,
	off = LOGNONE,
};

class Logger
{
	public:
		Logger();
		~Logger();

		template<typename... Args>
		static void Log(LevelEnum level,
		                spdlog::format_string_t<Args...> fmt,
		                Args &&... args)
		{
			GetInstance().LogInternal(level, fmt, std::forward<Args>(args)...);
		}

	private:
		static Logger &GetInstance();
		static spdlog::level::level_enum MapLogLevel(int level);

		template<typename... Args>
		void LogInternal(const LevelEnum level,
		                 spdlog::format_string_t<Args...> fmt,
		                 Args &&... args)
		{
			m_defaultLogger->log(MapLogLevel(level), fmt, std::forward<Args>(args)...);
		}

		std::shared_ptr<spdlog::logger> CreateLogger(const std::string &loggerName);

		std::shared_ptr<spdlog::sinks::sink> m_sinks;
		std::shared_ptr<spdlog::logger> m_defaultLogger;
};

// Formatter for QString to be used with fmt::format
#ifdef QT_CORE_LIB
#include <QString>

template<>
struct fmt::formatter<QString> : formatter<const char *>
{
	template<typename FormatContext>
	auto format(const QString &value, FormatContext &ctx) const -> decltype(ctx.out())
	{
		return formatter<const char *>::format(value.toUtf8().constData(), ctx);
	}
};
#endif

#endif //NRSC5_GUI_LOG_H
