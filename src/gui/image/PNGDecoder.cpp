//
// Created by TheDaChicken on 7/13/2025.
//

#include "PNGDecoder.h"

#include <array>
#include <cassert>
#include <png.h>

#define PNG_SIG_SIZE 8

PNGDecoder::PNGDecoder()
{
	png_ptr_ = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		nullptr,
		nullptr,
		nullptr);

	if (!png_ptr_)
		return;

	info_ptr_ = png_create_info_struct(png_ptr_);

	if (!info_ptr_)
		return;
}

PNGDecoder::~PNGDecoder()
{
	png_destroy_read_struct(&png_ptr_, &info_ptr_, nullptr);
}

static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	auto from = static_cast<std::istream *>(png_get_io_ptr(png_ptr));
	assert(from);

	if (!from->read(reinterpret_cast<char *>(data), length))
	{
		png_error(png_ptr, "Error reading.");
	}
}

GUI::FileLoadResult PNGDecoder::Decode(std::istream &in, GUI::Image &image)
{
	png_set_read_fn(png_ptr_, &in, user_read_data);
	png_set_sig_bytes(png_ptr_, in.tellg());

	if (setjmp(png_jmpbuf(png_ptr_)))
		return GUI::FileLoadResult::DecodingError;

	png_read_info(png_ptr_, info_ptr_);

	image.width = static_cast<int>(png_get_image_width(png_ptr_, info_ptr_));
	image.height = static_cast<int>(png_get_image_height(png_ptr_, info_ptr_));

	int bit_depth = png_get_bit_depth(png_ptr_, info_ptr_);

	// if transparency, convert it to alpha
	if (png_get_valid(png_ptr_, info_ptr_, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr_);
	}

	// set least one byte per channel
	if (bit_depth < 8)
	{
		png_set_packing(png_ptr_);
		bit_depth = 8;
	}

	image.bit_depth = BitDepth(bit_depth);

	switch (png_get_color_type(png_ptr_, info_ptr_))
	{
		case PNG_COLOR_TYPE_GRAY:
			image.color_type = GUI::Image::ColorType::RGB;
			png_set_gray_to_rgb(png_ptr_);
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			image.color_type = GUI::Image::ColorType::RGBA;
			png_set_gray_to_rgb(png_ptr_);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			image.color_type = GUI::Image::ColorType::RGB;
			png_set_palette_to_rgb(png_ptr_);
			break;
		case PNG_COLOR_TYPE_RGB:
			image.color_type = GUI::Image::ColorType::RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			image.color_type = GUI::Image::ColorType::RGBA;
			break;
		default:
			image.color_type = GUI::Image::ColorType::Unknown;
			break;
	}

	if (image.color_type == GUI::Image::ColorType::Unknown)
	{
		printf("Unsupported PNG color type: %d\n", png_get_color_type(png_ptr_, info_ptr_));
		return GUI::UnsupportedFormat;
	}

	png_set_interlace_handling(png_ptr_);
	png_read_update_info(png_ptr_, info_ptr_);

	size_t row_bytes = png_get_rowbytes(png_ptr_, info_ptr_);

	// PNG rows must be a multiple of 4 bytes
	row_bytes += 3 - (row_bytes - 1) % 4;

	image.data.resize(
		static_cast<std::size_t>(image.height) * row_bytes
	);

	std::vector<png_bytep> rows(image.height);

	for (size_t i = 0; i < image.height; i++)
	{
		rows[i] = &image.data[i * row_bytes];
	}

	png_read_image(png_ptr_, rows.data());
	png_read_end(png_ptr_, nullptr);
	return GUI::FileLoadResult::Success;
}

bool PNGDecoder::Detect(std::istream &in)
{
	std::array<unsigned char, PNG_SIG_SIZE> header{};

	if (!in.read(reinterpret_cast<std::istream::char_type *>(header.data()), PNG_SIG_SIZE))
		return GUI::FileReadError;

	return png_sig_cmp(header.data(), 0, PNG_SIG_SIZE) == 0;
}

GUI::Image::ColorType PNGDecoder::ColorType(const png_byte color_type)
{
	switch (color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			return GUI::Image::ColorType::Grayscale;
		case PNG_COLOR_TYPE_RGB:
			return GUI::Image::ColorType::RGB;
		case PNG_COLOR_TYPE_RGBA:
			return GUI::Image::ColorType::RGBA;
		default:
			return GUI::Image::ColorType::Unknown;
	}
}

GUI::Image::BitDepth PNGDecoder::BitDepth(const png_byte bit_depth)
{
	switch (bit_depth)
	{
		case 2:
			return GUI::Image::BitDepth::Bit2;
		case 4:
			return GUI::Image::BitDepth::Bit4;
		case 8:
			return GUI::Image::BitDepth::Bit8;
		case 16:
			return GUI::Image::BitDepth::Bit16;
		default:
			return GUI::Image::BitDepth::Unknown;
	}
}
