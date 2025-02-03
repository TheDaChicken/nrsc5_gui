//
// Created by TheDaChicken on 9/16/2024.
//

#ifndef EDITIMAGEDESCRIPTION_H
#define EDITIMAGEDESCRIPTION_H

#include "ImageText.h"

/**
 * @brief see @ref ImageText
 * This class is a subclass of ImageText that allows for editing text.
 */
class ImageTextEdit : public ImageText
{
		Q_OBJECT

	public:
		explicit ImageTextEdit(QWidget *parent = nullptr);

		[[nodiscard]] QTextDocument *document() const
		{
			return &text_document_;
		}

		void setTextWidth(int width) override
		{
			text_document_.setTextWidth(width);
			update();
		}

	protected:
		bool event(QEvent *event) override;
};

#endif //EDITIMAGEDESCRIPTION_H
