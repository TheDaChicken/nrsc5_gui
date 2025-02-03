//
// Created by TheDaChicken on 7/27/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGETS_IMAGEDESCRIPTION_H_
#define NRSC5_GUI_SRC_WIDGETS_IMAGEDESCRIPTION_H_

#include "ImageText.h"

/**
 * @brief see @ref ImageText
 * A subclass of ImageText that allows for a description to be displayed.
 */
class ImageTextLabel : public ImageText
{
		Q_OBJECT
		Q_PROPERTY(QString description READ description WRITE setDescription)
	public:
		explicit ImageTextLabel(QWidget *parent = nullptr);

		void setDescription(const QString &description)
		{
			if (description_ != description)
			{
				description_ = description;
				text_dry_ = true;
				update();
			}
		}

		/**
		 * @brief Sets the width of the line that will be drawn around the image.
		 * @param width
		 */
		void setTextWidth(int width) override
		{
			if (text_width_ != width)
			{
				text_width_ = width;
				layout_dry_ = true;
				update();
			}
		}

		[[nodiscard]] QString description() const
		{
			return description_;
		}
	signals:
		void clicked();

	protected:
		void mousePressEvent(QMouseEvent *event) override;
		void mouseMoveEvent(QMouseEvent *event) override;
		bool event(QEvent *event) override;

	private:
		void EnsureTextDocument() const override;

		int text_width_;
		mutable bool text_dry_;
		mutable bool layout_dry_;
		QString description_;
};

#endif //NRSC5_GUI_SRC_WIDGETS_IMAGEDESCRIPTION_H_
