//
// Created by TheDaChicken on 9/26/2023.
//

#ifndef NRSC5_GUI_STRINGUTILS_H
#define NRSC5_GUI_STRINGUTILS_H

#include <QString>
#include <filesystem>
#include <string_view>
#include <cstdint>

namespace StringUtils
{

uint32_t ToUint32(std::string_view str, uint32_t fallback = 0);
QString GetDisplayFrequency();
std::string Strip(const std::string& str);

inline QString PathToStr(const std::filesystem::path &path)
{
#ifdef _WIN32
  return QString::fromStdWString(path.native());
#else
  return QString::fromStdString(path.native());
#endif
}

inline std::filesystem::path StrToPath(const QString &path)
{
#ifdef _WIN32
  return {path.toStdWString()};
#else
  return {path.toStdString()};
#endif
}

};

#endif //NRSC5_GUI_STRINGUTILS_H