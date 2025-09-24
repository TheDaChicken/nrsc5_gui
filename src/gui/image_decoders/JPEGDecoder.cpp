//
// Created by TheDaChicken on 7/22/2025.
//

#include "JPEGDecoder.h"

#include <csetjmp>
#include <cstdint>
#include <istream>

#include "utils/Log.h"

struct jpegErrorManager
{
	/* "public" fields */
	struct jpeg_error_mgr pub;
	/* for return to caller */
	jmp_buf setjmp_buffer;
};

char jpegLastErrorMsg[JMSG_LENGTH_MAX];

void jpegErrorExit(j_common_ptr cinfo)
{
	auto myerr = reinterpret_cast<jpegErrorManager *>(cinfo->err);
	(*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);
	longjmp(myerr->setjmp_buffer, 1);
}

struct JpegStream
{
	jpeg_source_mgr pub;
	std::istream *stream;
	uint8_t buffer[4096];
};

void init_source(j_decompress_ptr cinfo)
{
	const auto src = reinterpret_cast<JpegStream *>(cinfo->src);
	src->stream->seekg(0);
}

boolean fill_input_buffer(const j_decompress_ptr cinfo)
{
	auto src = reinterpret_cast<JpegStream *>(cinfo->src);
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = src->stream->readsome(reinterpret_cast<char *>(src->buffer), sizeof(src->buffer));
	if (src->pub.bytes_in_buffer <= 0)
	{
		src->buffer[0] = static_cast<JOCTET>(0xFF);
		src->buffer[1] = static_cast<JOCTET>(JPEG_EOI);
		src->pub.bytes_in_buffer = 2;
		return true;
	}

	return true;
}

void skip(j_decompress_ptr cinfo, long count)
{
	const auto src = reinterpret_cast<JpegStream *>(cinfo->src);

	// Request more if we are skipping more than the current buffer size
	while (count > src->pub.bytes_in_buffer)
	{
		count -= static_cast<long>(src->pub.bytes_in_buffer);
		fill_input_buffer(cinfo);
	}
	src->pub.next_input_byte += count;
	src->pub.bytes_in_buffer -= count;
}

void term_source(j_decompress_ptr cinfo)
{
	// Close the stream, can be nop
}

JPEGDecoder::JPEGDecoder()
{
}

JPEGDecoder::~JPEGDecoder()
{
	jpeg_destroy_decompress(&cinfo);
}

GUI::FileLoadResult JPEGDecoder::Decode(std::istream &in, GUI::ImageBuffer &image)
{
	jpegErrorManager jerr;
	/* Allocate and initialize JPEG decompression object. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpegErrorExit;

	/* Establish the setjmp return context for JpegLoadErrorExit to use. */
	if (setjmp(jerr.setjmp_buffer))
		return GUI::FileLoadResult::DecodingError;

	jpeg_create_decompress(&cinfo);

	/* first time for this JPEG object? */
	cinfo.src = static_cast<jpeg_source_mgr *>((*cinfo.mem->alloc_small)(
		reinterpret_cast<j_common_ptr>(&cinfo),
		JPOOL_PERMANENT,
		sizeof(JpegStream)));

	auto *src = reinterpret_cast<JpegStream *>(cinfo.src);
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip;
	src->pub.resync_to_restart = jpeg_resync_to_restart;
	src->pub.term_source = term_source;
	src->pub.bytes_in_buffer = 0;
	src->pub.next_input_byte = nullptr;
	src->stream = &in;

	jpeg_read_header(&cinfo, TRUE);

	// Set the output color space to RGBA
	cinfo.out_color_space = JCS_EXT_RGBA;

	jpeg_start_decompress(&cinfo);

	// JPEG is always 8-bit per channel
	image.color_type = GUI::ImageBuffer::ColorType::RGBA;
	image.type = GUI::FileType::JPEG;
	image.width = cinfo.output_width;
	image.height = cinfo.output_height;
	image.data.resize(image.width * image.height * cinfo.output_components); // RGBA

	uint8_t *row_pointer = image.data.data();
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &row_pointer, 1);
		row_pointer += image.width * cinfo.output_components; // Move to next row
	}
	jpeg_finish_decompress(&cinfo);
	return GUI::FileLoadResult::Success;
}
