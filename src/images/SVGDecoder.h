//
// Created by TheDaChicken on 7/14/2025.
//

#ifndef SVGDECODER_H
#define SVGDECODER_H

#include <imgui.h>
#include <lunasvg.h>

#include "ImageBuffer.h"
#include "utils/Types.h"

class SVGDecoder
{
	public:
		GUI::FileLoadResult Decode(std::istream &in);
		GUI::FileLoadResult Decode(std::string_view in);
		void Render(GUI::ImageBuffer &image) const;

		int GetWidth()
		{
			return document ? document->width() : 0;
		}

		int GetHeight()
		{
			return document ? document->height() : 0;
		}

		std::unique_ptr<lunasvg::Document>& Document()
		{
			return document;
		}

		explicit operator bool() const
		{
			return document != nullptr;
		}

		static bool Detect(std::istream &in);
	private:
		std::unique_ptr<lunasvg::Document> document;
};

#endif //SVGDECODER_H
