//
// Created by TheDaChicken on 7/18/2025.
//

#include "AssetsManager.h"

#include "utils/Log.h"

#ifdef _WIN32
#define UNICODE
#include <libloaderapi.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

GUI::FileLoadResult GUI::AssetsManager::LoadImageAsset(const std::string &resource_name, Image &image, Image::Type type)
{
	std::filesystem::path filePath;
	FileLoadResult ret;

	if (resource_name.empty())
		return InvalidResourceName;

	ret = GetAsset(resource_name, filePath);
	if (ret != Success)
	{
		return ret;
	}

	ret = imageManager_.LoadImage(filePath.string(), image, type);
	if (ret != Success)
	{
		return ret;
	}

	return Success;
}

GUI::FileLoadResult GUI::AssetsManager::GetAsset(const std::string &resource_name, std::filesystem::path &assetPath)
{
	const std::filesystem::path assets = GetAssetsFolder();
	if (assets.empty())
		return FileNotFound;

	assetPath = assets / resource_name;
	if (!std::filesystem::exists(assetPath))
	{
		Logger::Log(err, "Asset not found: {}\n", assetPath.string());
		return FileNotFound;
	}

	return Success;
}

std::filesystem::path GUI::AssetsManager::GetAssetsFolder()
{
#ifdef __WIN32__
	return GetExecutableFolder() / "resources";
#endif

#ifdef __LINUX__
	std::filesystem::path path = GetExecutableFolder();
	if (!path.empty())
	{
		return path.parent_path() / "resources";
	}

	// If we can't determine the executable path, return a default assets folder
	return std::filesystem::path("/usr/share/") / "nrsc5_gui" / "resources";
#endif

#ifdef __APPLE__
	char path[1204];
	if (CFBundleCopyBundleURL(CFBundleGetMainBundle(), path, 1204) == nullptr)
		return {};

	return std::filesystem::path(path) / "resources";
#endif
	return {};
}

std::filesystem::path GUI::AssetsManager::GetExecutableFolder()
{
#ifdef __WIN32__
	std::wstring path(MAX_PATH, L'\0');
	while (true)
	{
		const DWORD len = GetModuleFileName(nullptr, path.data(), path.size());;
		if (len == 0)
			return {};

		if (len < path.size())
		{
			path.resize(len);
			break;
		}

		path.resize(2 * path.size());
	}

	return std::filesystem::path(path).parent_path();
#endif

#ifdef __LINUX__
	std::string path(1204, '\0');

	while (true)
	{
		const int length = readlink("/proc/self/exe", path.data(), path.size());
		if (length < 0)
			return {};

		if (length < static_cast<int>(path.size()))
		{
			path.resize(length);
			break;
		}

		path.resize(2 * path.size());
	}

	return std::filesystem::path(path).parent_path();
#endif
	return {};
}