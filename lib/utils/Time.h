//
// Created by TheDaChicken on 8/21/2024.
//

#ifndef NRSC5_GUI_LIB_UTILS_TIME_H_
#define NRSC5_GUI_LIB_UTILS_TIME_H_

#include <fmt/chrono.h>

template<typename Rep, typename Period>
struct fmt::formatter<std::chrono::duration<Rep, Period>>
{
  constexpr auto parse(fmt::format_parse_context &ctx)
  {
	return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const std::chrono::duration<Rep, Period> &duration, FormatContext &ctx) const
  {
	auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	int days = total_seconds / (24 * 3600);
	total_seconds %= (24 * 3600);
	int hours = total_seconds / 3600;
	total_seconds %= 3600;
	int minutes = total_seconds / 60;
	int seconds = total_seconds % 60;

	std::string result;
	if (days > 0) result += fmt::format("{}D ", days);
	if (hours > 0 || !result.empty()) result += fmt::format("{}H ", hours);
	if (minutes > 0 || !result.empty()) result += fmt::format("{}M ", minutes);
	result += fmt::format("{}S", seconds);

	return fmt::format_to(ctx.out(), "{}", result);
  }
};

namespace TimeUtils
{

static time_t timegm(tm &e)
{
#if defined(WIN32)
  return ::_mkgmtime(&e);
#else
  return ::timegm(&e);
#endif
}

} // Time

#endif //NRSC5_GUI_LIB_UTILS_TIME_H_
