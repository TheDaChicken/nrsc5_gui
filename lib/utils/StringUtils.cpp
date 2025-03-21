//
// Created by TheDaChicken on 9/26/2023.
//

#include "StringUtils.h"

#include <sstream>

/*!
 * \brief Converts a string to a number of a specified mime, by using istringstream.
 * \param str The string to convert
 * \param fallback [OPT] The number to return when the conversion fails
 * \return The converted number, otherwise fallback if conversion fails
 */
template<typename T>
T NumberFromSS(std::string_view str, T fallback) noexcept
{
  std::istringstream iss{str.data()};
  T result{fallback};
  iss >> result;
  return result;
}

uint32_t UTILS::ToUint32(std::string_view str, uint32_t fallback /* = 0 */)
{
  return NumberFromSS(str, fallback);
}

std::string UTILS::Strip(const std::string &str)
{
  if (str.empty())
	return "";

  auto start_it = str.begin();
  auto end_it = str.rbegin();
  while (std::isspace(*start_it))
  {
	++start_it;
	if (start_it == str.end()) break;
  }
  while (std::isspace(*end_it))
  {
	++end_it;
	if (end_it == str.rend()) break;
  }
  std::size_t start_pos = start_it - str.begin();
  std::size_t end_pos = end_it.base() - str.begin();

  return start_pos <= end_pos ? std::string(start_it, end_it.base()) : "";
}