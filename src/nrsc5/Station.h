//
// Created by TheDaChicken on 6/18/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_NRSC5_STATION_H_
#define NRSC5_GUI_SRC_LIB_NRSC5_STATION_H_

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <filesystem>
#include <optional>
#include <unordered_map>

#include "utils/Types.h"

extern "C" {
#include <nrsc5.h>
}

#include <nlohmann/json.hpp>

namespace NRSC5
{
struct Ber
{
	void Add(float cber);

	float cber_{0};
	float ber{0};
	float min{1};
	float max{0};

	private:
		float sum{0};
		float count{0};
};

struct ID3
{
	ID3() = default;
	explicit ID3(const nrsc5_event_t *event);

	std::string title;
	std::string artist;
	std::string album;
	std::string genre;

	/**
	 * @brief Trigger for rendering media content to display in the receiver
	 * @details Custom ID3 frame for the ID3 standard
	 * @sa US: https://patents.google.com/patent/US8451868
	 * @sa Internal: https://patents.google.com/patent/WO2011044349A1/en
	 */
	struct XHDR
	{
		enum PARAM : int
		{
			EMPTY = -1,
			PROVIDED = 0,
			/** Image should be immediately removed from the display (to default image) */
			DEFAULT = 1,
			/**
			 * The receivers image memory should be flushed; removing all previously stored images except the default image.
			 * The current image may be immediately removed from the display and the default image should be displayed */
			FLUSH = 2,
		} param = EMPTY;

		uint32_t mime = 0;
		int lot = -1;

		void Clear()
		{
			param = EMPTY;
			mime = NRSC5_MIME_PRIMARY_IMAGE;
			lot = -1;
		}

		[[nodiscard]] std::string_view ParamName() const;

		[[nodiscard]] bool Empty() const
		{
			return param == EMPTY;
		}

		bool operator ==(const XHDR &rhs) const
		{
			return param == rhs.param && mime == rhs.mime && lot == rhs.lot;
		}
	} xhdr;

	void Clear()
	{
		title.clear();
		artist.clear();
		album.clear();
		genre.clear();
		xhdr.Clear();
	}

	[[nodiscard]] bool Empty() const
	{
		return title.empty() && artist.empty()
				&& album.empty() && genre.empty() && xhdr.Empty();
	}

	bool operator==(const ID3 &rhs) const
	{
		return title == rhs.title && artist == rhs.artist && album == rhs.album
				&& genre == rhs.genre && xhdr == rhs.xhdr;
	}

	bool operator!=(const ID3 &rhs) const
	{
		return !(rhs == *this);
	}
};

struct Program
{
	unsigned int type = NRSC5_PROGRAM_TYPE_UNDEFINED;

	std::string name;
};

struct DataService
{
	DataService(const nrsc5_sig_service_t *sig_service,
	            const nrsc5_sig_component_t *component);
	DataService() = default;

	uint8_t type{0};
	uint32_t mime{0};
	uint16_t port{100};

	unsigned int channel;
	std::optional<unsigned int> programId;
};

struct StationInfo
{
	unsigned int id{0};
	std::string country_code;
	std::string name;

	/**
	* @brief Chosen program for the station (0 = MPS) 2-8 are HD Radio programs
	*/
	unsigned int current_program{0};

	void Reset()
	{
		name.clear();
		country_code.clear();
		current_program = 0;
	}

	/**
	 * @brief Check if the station is the similar to another station
	 * The station can be the same but with reduced information (e.g. no programs)
	 * The nature of the NRSC5 protocol and the way data is updated slowly over time
	 * @param station The station to compare
	 * @return true if the stations are similar
	 */
	bool operator==(const StationInfo &station) const
	{
		return name == station.name && current_program == station.current_program;
	}
	bool operator!=(const StationInfo &station_info) const
	{
		return !(station_info == *this);
	}

	private:
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(StationInfo, id, country_code, name, current_program)
};

struct StationDetails
{
	std::string message;
	std::string slogan;

	// program ID -> program
	std::map<unsigned int, Program> programs;

	void Reset()
	{
		message.clear();
		slogan.clear();
		programs.clear();
	}
};

/**
 * \brief Large Object Transfer
 */
struct Lot
{
	explicit Lot(const nrsc5_event_t *evt);
	Lot() = default;

	unsigned int id{0};
	uint32_t mime{0};

	std::string name;
	std::filesystem::path path;
	vector_uint8_t data;
	tm discard_utc{};

	std::chrono::system_clock::time_point expire_point;

	DataService component;

	[[nodiscard]] bool isExpired() const
	{
		return expire_point < std::chrono::system_clock::now();
	}
};

std::string_view DescribeMime(uint32_t mime);
inline unsigned int FriendlyProgramId(const unsigned int program)
{
	// Program ID is 0-based. Friendly would be to display it as 1-based
	return program + 1;
}

} // namespace NRSC5

#endif //NRSC5_GUI_SRC_LIB_NRSC5_STATION_H_
