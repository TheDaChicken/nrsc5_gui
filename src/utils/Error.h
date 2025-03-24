//
// Created by TheDaChicken on 9/2/2024.
//

#ifndef NRSC5_GUI_LIB_UTILS_ERROR_H_
#define NRSC5_GUI_LIB_UTILS_ERROR_H_

namespace UTILS
{
enum class StatusCodes
{
	Empty,
	Ok, // No error

	NoDevice, // No device found
	NoStream, // No stream found

	SQLBusy, // SQL busy

	NoPermission, // No permission

	TunerError, // Generic tuner error
	DatabaseError, // Generic Database error
	UnknownError, // Generic error
};

} // namespace UTILS

#endif //NRSC5_GUI_LIB_UTILS_ERROR_H_
