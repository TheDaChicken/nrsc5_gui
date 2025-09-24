//
// Created by TheDaChicken on 7/18/2025.
//

#include "AssetsManager.h"

#include "utils/Log.h"

#ifdef _WIN32
#define UNICODE
#include <libloaderapi.h>
#endif

#ifdef __unix__
#include <linux/limits.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

GUI::AssetsManager::AssetsManager()
{
	assetsFolder_ = TryAssetsFolder();

	if (assetsFolder_)
		Logger::Log(info, "Assets folder: {}", assetsFolder_->string());
}

bool GUI::AssetsManager::HasAssetsFolder()
{
	return assetsFolder_.has_value();
}

tl::expected<std::filesystem::path, GUI::FileLoadResult> GUI::AssetsManager::GetResource(
	const std::string_view resource_name) const
{
	if (!assetsFolder_.has_value())
		return tl::unexpected(FileNotFound);

	std::filesystem::path assetPath = assetsFolder_.value() / resource_name;
	if (!std::filesystem::exists(assetPath))
	{
		Logger::Log(err, "Asset not found: {}", assetPath.string());
		return tl::unexpected(FileNotFound);
	}

	return assetPath;
}

std::optional<std::filesystem::path> GUI::AssetsManager::TryAssetsFolder()
{
#ifdef _WIN32
	auto exeFolder = GetExecutableFolder();
	if (exeFolder.has_value())
	{
		return exeFolder.value() / "resources";
	}
#endif

#ifdef __unix__
	auto exeFolder = GetExecutableFolder();
	if (exeFolder.has_value())
	{
		std::filesystem::path localResources = exeFolder.value() / "resources";
		if (std::filesystem::exists(localResources))
			return localResources;

		// If we can't determine the executable path, try a default location
		return exeFolder.value().parent_path() / "share" / "nrsc5_gui" / "resources";
	}
#endif

#ifdef __APPLE__
	// CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	// CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	// const char *pathPtr = CFStringGetCStringPtr(macPath, kCFStringEncodingUTF8);
	//
	// return std::filesystem::path(pathPtr) / "resources";
#endif
	return {};
}

std::optional<std::filesystem::path> GUI::AssetsManager::GetExecutableFolder()
{
#ifdef _WIN32
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

#ifdef __unix__
	std::string path(PATH_MAX, '\0');

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
}