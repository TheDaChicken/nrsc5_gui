//
// Created by TheDaChicken on 6/18/2024.
//

#include "Station.h"
#include "utils/Time.h"

#include <cassert>
#include <nrsc5.h>

std::string_view NRSC5::ID3::XHDR::ParamName() const
{
	switch (param)
	{
		case EMPTY: return "EMPTY";
		case DEFAULT: return "DEFAULT";
		case PROVIDED: return "PROVIDED";
		case FLUSH: return "FLUSH";
		default: return "Unknown";
	}
}

NRSC5::DataService::DataService(
	const nrsc5_sig_service_t *sig_service,
	const nrsc5_sig_component_t *component)
{
	assert(sig_service);
	assert(component);

	channel = sig_service->number;
	if (sig_service->audio_component)
		programId = sig_service->audio_component->audio.port;

	port = component->data.port;
	type = component->data.type;
	mime = component->data.mime;
}

void NRSC5::Ber::Add(float cber)
{
	cber_ = cber;
	sum += cber;
	count += 1;

	ber = sum / count;

	if (cber < min) min = cber;
	if (cber > max) max = cber;
}

std::string_view NRSC5::DescribeMime(const uint32_t mime)
{
	switch (mime)
	{
		case NRSC5_MIME_JPEG: return "JPEG";
		case NRSC5_MIME_PNG: return "PNG";
		case NRSC5_MIME_TEXT: return "Text";
		/* types of images */
		case NRSC5_MIME_PRIMARY_IMAGE: return "Primary Image";
		case NRSC5_MIME_STATION_LOGO: return "Station Logo";
		/* here's navteq */
		case NRSC5_MIME_NAVTEQ: return "NAVTEQ";
		case NRSC5_MIME_HERE_IMAGE: return "HERE IMAGE";
		case NRSC5_MIME_HERE_TPEG: return "HERE TPEG";
		case NRSC5_MIME_HD_TMC: return "HD TMC";
		/* TTN HDRadio */
		case NRSC5_MIME_TTN_TPEG_1: return "TTN TPEG 1";
		case NRSC5_MIME_TTN_TPEG_2: return "TTN TPEG 2";
		case NRSC5_MIME_TTN_TPEG_3: return "TTN TPEG 3";
		case NRSC5_MIME_TTN_STM_TRAFFIC: return "TTN STM Traffic";
		case NRSC5_MIME_TTN_STM_WEATHER: return "TTN STM Weather";
		default: return "Unknown";
	}
}
