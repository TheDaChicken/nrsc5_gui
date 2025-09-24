//
// Created by TheDaChicken on 7/13/2025.
//

#include "PNGDecoder.h"

#include <array>
#include <cassert>
#include <png.h>

#include "utils/Log.h"
#include "utils/Types.h"

#define PNG_BYTES_CHECK 4

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

static void io_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	auto is = static_cast<std::istream *>(png_get_io_ptr(png_ptr));
	assert(is);

	if (!is || !is->good())
	{
		png_error(png_ptr, "cannot read from stream");
		return;
	}

	is->read(reinterpret_cast<char *>(data), length);
}

GUI::FileLoadResult PNGDecoder::Decode(std::istream &in, GUI::ImageBuffer &image)
{
	if (!in.good())
		return GUI::InvalidResourceName;

	png_set_read_fn(png_ptr_, &in, io_read_data);
	png_set_sig_bytes(png_ptr_, in.tellg());
	return TryDecode(image);
}

GUI::FileLoadResult PNGDecoder::TryDecode(GUI::ImageBuffer &image)
{
	if (setjmp(png_jmpbuf(png_ptr_)))
	{
		return GUI::FileLoadResult::DecodingError;
	}

	png_read_info(png_ptr_, info_ptr_);

	image.type = GUI::FileType::PNG;
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

	if (png_get_bit_depth(png_ptr_, info_ptr_) == 16)
		png_set_strip_16(png_ptr_);

	switch (png_get_color_type(png_ptr_, info_ptr_))
	{
		case PNG_COLOR_TYPE_GRAY:
			image.color_type = GUI::ImageBuffer::ColorType::RGB;
			png_set_gray_to_rgb(png_ptr_);
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			image.color_type = GUI::ImageBuffer::ColorType::RGBA;
			png_set_gray_to_rgb(png_ptr_);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			image.color_type = GUI::ImageBuffer::ColorType::RGB;
			png_set_palette_to_rgb(png_ptr_);
			break;
		case PNG_COLOR_TYPE_RGB:
			image.color_type = GUI::ImageBuffer::ColorType::RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			image.color_type = GUI::ImageBuffer::ColorType::RGBA;
			break;
		default:
			image.color_type = GUI::ImageBuffer::ColorType::Unknown;
			break;
	}

	if (image.color_type == GUI::ImageBuffer::ColorType::RGB)
	{
		png_set_add_alpha(png_ptr_, 0xFF, PNG_FILLER_AFTER);
		image.color_type = GUI::ImageBuffer::ColorType::RGBA;
	}

	if (image.color_type == GUI::ImageBuffer::ColorType::Unknown)
	{
		printf("Unsupported PNG color type: %d\n", png_get_color_type(png_ptr_, info_ptr_));
		return GUI::UnsupportedFormat;
	}

	// Ensure all images have an alpha channel
	if (!(png_get_color_type(png_ptr_, info_ptr_) & PNG_COLOR_MASK_ALPHA))
		png_set_add_alpha(png_ptr_, 0xFF, PNG_FILLER_AFTER);

	png_set_interlace_handling(png_ptr_);
	png_read_update_info(png_ptr_, info_ptr_);

	const size_t row_bytes = png_get_rowbytes(png_ptr_, info_ptr_);

	image.data.resize(
		static_cast<std::size_t>(image.height) * row_bytes
	);

	std::vector<png_bytep> png_rows(image.height);
	for (size_t i = 0; i < image.height; i++)
	{
		png_rows[i] = &image.data[i * row_bytes];
	}

	png_read_image(png_ptr_, png_rows.data());
	png_read_end(png_ptr_, nullptr);
	return GUI::FileLoadResult::Success;
}

bool PNGDecoder::Detect(std::istream &in)
{
	std::array<unsigned char, PNG_BYTES_CHECK> header{};

	if (!in.read(reinterpret_cast<std::istream::char_type *>(header.data()), PNG_BYTES_CHECK))
		return GUI::FileReadError;

	return png_sig_cmp(header.data(), 0, PNG_BYTES_CHECK) == 0;
}

bool PNGDecoder::Detect(const std::string_view in)
{
	return png_sig_cmp(reinterpret_cast<png_const_bytep>(in.data()), 0, PNG_BYTES_CHECK) == 0;
}
