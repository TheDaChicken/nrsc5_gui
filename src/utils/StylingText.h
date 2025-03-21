//
// Created by TheDaChicken on 8/21/2024.
//

#ifndef NRSC5_GUI_SRC_STYLINGTEXT_H_
#define NRSC5_GUI_SRC_STYLINGTEXT_H_

#include <QTextCharFormat>
#include <utils/Log.h>

#include "RadioChannel.h"

/**
 * @brief Class to style text for display
 */
class StylingText
{
	public:
		enum Direction
		{
			SHORT = 0,
			LONG = 1
		};

		static QString GetDisplayChannel(const Channel& channel);
		static void DisplayStation(QTextDocument *document, const ActiveChannel &channel, const Direction &direction);
		static void GenerateChannelList(QTextDocument *document,
		                                const std::map<unsigned int, NRSC5::Program> &channels,
		                                unsigned int current_program);
		static void DisplayID3(QTextDocument *document, const NRSC5::ID3 &id3);

		static void DisplaySignalStrength(QTextDocument *document, double level);

	private:
		static QColor SignalColor(double level);

		static QFont SmallFont(const QFont &original);
		static QFont MediumFont(const QFont &original);
		static QFont BaseFont(const QFont &original);
		static QFont LargeFont(const QFont &original);
		static QFont GetFont(const QFont &defaultFont,
		                     const QString &fontSizeVar,
		                     const QString &defaultValue = "20px");

		static QTextCharFormat HDSelectedFormat(const QTextCharFormat &original);

		static QFont ParseFontSize(const QFont &font, const QString &fontSize)
		{
			QFont newFont = font;

			if (fontSize.endsWith("px"))
				newFont.setPixelSize(fontSize.left(fontSize.length() - 2).toInt());
			if (fontSize.endsWith("pt"))
				newFont.setPointSize(fontSize.left(fontSize.length() - 2).toInt());

			return newFont;
		}
};

#endif //NRSC5_GUI_SRC_STYLINGTEXT_H_
