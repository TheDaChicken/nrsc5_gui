//
// Created by TheDaChicken on 9/26/2023.
//

#ifndef NRSC5_GUI_STRINGUTILS_H
#define NRSC5_GUI_STRINGUTILS_H

#include <QString>
#include <filesystem>
#include <string_view>
#include <cstdint>

namespace UTILS
{

uint32_t ToUint32(std::string_view str, uint32_t fallback = 0);
std::string Strip(const std::string& str);

inline QString PathToQStr(const std::filesystem::path &path)
{
#ifdef _WIN32
  return QString::fromStdWString(path.native());
#else
  return QString::fromStdString(path.native());
#endif
}

inline std::filesystem::path QStrToPath(const QString &path)
{
#ifdef _WIN32
  return {path.toStdWString()};
#else
  return {path.toStdString()};
#endif
}

} // namespace StringUtils

#endif //NRSC5_GUI_STRINGUTILS_H