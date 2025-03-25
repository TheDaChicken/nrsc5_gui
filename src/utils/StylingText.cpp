//
// Created by TheDaChicken on 8/21/2024.
//

#include "StylingText.h"

#include <Application.h>
#include <QTextCharFormat>
#include <QTextCursor>

static constexpr auto HDColor = QColor(255, 165, 0);

[[nodiscard]] QString StylingText::GetDisplayChannel(const Channel& channel)
{
	const auto& tuner_opts = channel.tuner_opts;
	const auto& station = channel.station_info;

	QString text = QString::number(tuner_opts.GetFrequencyInShort(), 'g', 5);

	if (station.current_program > NRSC5_MPS_PROGRAM)
	{
		text += QString(" HD%1").arg(NRSC5::FriendlyProgramId(station.current_program));
	}

	return text;
}

/**
 * @brief Generates the String styled list of channels.
 * @param document The QTextDocument to insert the text into.
 * @param channels The map of channels to display.
 * @param current_program The current program to highlight.
 * @return
 */
void StylingText::GenerateChannelList(QTextDocument *document,
                                      const std::map<unsigned int, NRSC5::Program> &channels,
                                      const unsigned int current_program)
{
	document->clear();

	if (channels.size() < 2)
	{
		return;
	}

	QTextCursor cursor(document);

	QTextBlockFormat block_format(cursor.blockFormat());

	block_format.setTopMargin(0);
	block_format.setBottomMargin(0);
	block_format.setAlignment(Qt::AlignHCenter);

	QTextCharFormat smallFormat(cursor.charFormat());
	smallFormat.setFont(SmallFont(smallFormat.font()));

	QTextCharFormat selectedFormat(HDSelectedFormat(smallFormat));;

	cursor.setBlockFormat(block_format);
	cursor.setCharFormat(smallFormat);

	// Set the default font to the small font.
	// This is necessary to ensure that the text is scaled properly with no text.
	document->setDefaultFont(smallFormat.font());

	const unsigned int lastProgram = std::prev(channels.end())->first;

	for (auto &[id, program] : channels)
	{
		const unsigned int channel = NRSC5::FriendlyProgramId(id);

		if (current_program == id)
			cursor.insertText(QString::number(channel), selectedFormat);
		else
			cursor.insertText(QString::number(channel), smallFormat);

		// Add a space between the channel numbers
		if (id != lastProgram)
			cursor.insertText("  ");
	}
}

void StylingText::DisplayStation(QTextDocument *document, const ActiveChannel &channel, const Direction &direction)
{
	const NRSC5::StationInfo &station = channel.station_info;
	const QString channelText = GetDisplayChannel(channel);

	document->clear();

	QTextCursor cursor(document);

	const QTextCharFormat original(cursor.charFormat());

	QTextCharFormat smallFormat(cursor.charFormat());
	QTextCharFormat medium_format_bold(cursor.charFormat());

	smallFormat.setFont(SmallFont(original.font()));

	medium_format_bold.setFont(MediumFont(original.font()));
	medium_format_bold.setFontWeight(QFont::Bold);

	switch (direction)
	{
		case SHORT:
		{
			cursor.insertText(channelText, medium_format_bold);

			if (!station.name.empty())
			{
				cursor.insertText(" • " + QString::fromStdString(station.name));
			}

			cursor.insertBlock();
			break;
		}
		case LONG:
		{
			QTextBlockFormat blockFormat(cursor.blockFormat());
			blockFormat.setTopMargin(0);
			blockFormat.setBottomMargin(2);
			cursor.setBlockFormat(blockFormat);

			cursor.insertText(channelText, medium_format_bold);
			cursor.insertBlock();
			cursor.setCharFormat(smallFormat);

			// Display Station Name if available
			if (!station.name.empty())
			{
				// TODO: use station name without "HD" if signal dies
				cursor.insertText(QString::fromStdString(station.name));
				cursor.insertText("-HD");

				// Display Channel Number if there are multiple programs
				if (channel.hd_details.programs.size() > 1)
				{
					cursor.insertText(QString::number(NRSC5::FriendlyProgramId(station.current_program)));
				}

				cursor.insertBlock();
			}

			if (const auto iter = channel.hd_details.programs.find(station.current_program);
				iter != channel.hd_details.programs.end())
			{
				NRSC5::Program program = iter->second;

				// Display Program Type if available
				if (program.type != NRSC5_PROGRAM_TYPE_UNDEFINED)
				{
					const QString kProgramTypeName = QString::fromLatin1(
						NRSC5::Decoder::ProgramTypeName(program.type));
					cursor.insertText(kProgramTypeName);
					cursor.insertBlock();
				}
			}
			break;
		}
	}
}

void StylingText::DisplayID3(QTextDocument *document, const NRSC5::ID3 &id3)
{
	document->clear();

	QTextCursor cursor(document);
	QTextCharFormat char_format(cursor.charFormat());

	QFont baseFont = BaseFont(char_format.font());
	QFont largeBoldFont = LargeFont(char_format.font());

	largeBoldFont.setWeight(QFont::Bold);

	QTextBlockFormat blockFormat(cursor.blockFormat());
	blockFormat.setTopMargin(0);
	blockFormat.setBottomMargin(15);

	cursor.setBlockFormat(blockFormat);

	auto insertTextBlock = [&](const QString &text, const QFont &font)
	{
		char_format.setFont(font);

		cursor.insertText(text, char_format);
		cursor.insertBlock();
	};

	if (!id3.title.empty())
	{
		insertTextBlock(QString::fromUtf8(id3.title), largeBoldFont);
	}
	if (!id3.artist.empty())
	{
		insertTextBlock(QString::fromUtf8(id3.artist), baseFont);
	}
	if (!id3.album.empty())
	{
		insertTextBlock(QString::fromUtf8(id3.album), baseFont);
	}
	if (!id3.genre.empty())
	{
		insertTextBlock(QString::fromUtf8(id3.genre), baseFont);
	}
}

void StylingText::DisplaySignalStrength(QTextDocument *document, const double level)
{
	document->clear();
	document->setDefaultStyleSheet("background: transparent; border:0;");

	QTextCursor cursor(document);
	QTextBlockFormat blockFormat(cursor.blockFormat());

	blockFormat.setTopMargin(0);
	blockFormat.setBottomMargin(0);
	blockFormat.setAlignment(Qt::AlignHCenter);
	blockFormat.setBackground(SignalColor(level));

	cursor.setBlockFormat(blockFormat);

	QTextCharFormat format(cursor.charFormat());

	cursor.insertText(QString("%1 dB").arg(level, 3, 'f', 1), format);
}

QColor StylingText::SignalColor(const double level)
{
	if (level > 15)
		return {0, 186, 0};
	if (level > 9)
		return {0, 128, 0};
	if (level > 5)
		return {255, 165, 0};
	return {220, 20, 60};
}

QFont StylingText::GetFont(const QFont &defaultFont,
                           const QString &fontSizeVar,
                           const QString &defaultValue)
{
	const QString fontSize = getApp()->GetThemeManager().GetValue(fontSizeVar, defaultValue).toString();
	QFont font(ParseFontSize(defaultFont, fontSize));

	// This stupid shit is required for the text to not look ugly
	font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);
	return font;
}

QFont StylingText::BaseFont(const QFont &original)
{
	return GetFont(original, QStringLiteral("font_base"), "16px");
}

QFont StylingText::SmallFont(const QFont &original)
{
	return GetFont(original, QStringLiteral("font_small"), "12px");
}

QFont StylingText::MediumFont(const QFont &original)
{
	return GetFont(original, QStringLiteral("font_medium"), "20px");
}

QFont StylingText::LargeFont(const QFont &original)
{
	return GetFont(original, QStringLiteral("font_large"), "20px");
}

QTextCharFormat StylingText::HDSelectedFormat(const QTextCharFormat &original)
{
	QTextCharFormat format(original);
	format.setForeground(HDColor);
	return format;
}
