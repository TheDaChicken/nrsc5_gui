//
// Created by TheDaChicken on 9/16/2024.
//

#ifndef BASEIMAGEDESCRIPTION_H
#define BASEIMAGEDESCRIPTION_H

#include <QFrame>
#include <QTextDocument>
#include <utils/Log.h>

/**
* @brief A widget that displays an image with a description.
*
* Special features:
* - Keeps aspect-ratio of the image.
* - Also handles dynamic changing image background color to prevent image from being invisible.
*/
class ImageText : public QFrame
{
		Q_OBJECT
		Q_PROPERTY(QPixmap image READ pixmap WRITE setPixmap)
		Q_PROPERTY(int alignment READ alignment WRITE setAlignment)
		Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
		Q_PROPERTY(int textWidth READ textWidth WRITE setTextWidth)

	public:
		explicit ImageText(QWidget *parent = nullptr);

		enum WidgetAlignment
		{
			TextTop = 0x01,
			TextBottom = 0x02,
			TextLeft = 0x04,
			TextRight = 0x08,

			ImageCenterX = 0x10, // Center based on image size
			ImageCenterY = 0x20, // Center based on image size

			CenterX = 0x40, // Center based on the widget size
			CenterY = 0x80 // Center based on the widget size
		};

		void setPixmap(const QPixmap &pixmap);
	    void setImage(const QImage &image);

		[[nodiscard]] QPixmap pixmap() const
		{
			return image_;
		}

		/**
		   * @brief Sets the alignment of either image & text or both.
		   */
		void setAlignment(int alignment)
		{
			if (alignment_ != alignment)
			{
				alignment_ = alignment;
				update();
			}
		}

		/**
		 * @brief Sets the spacing between the image and the text.
		 * @param spacing
		 */
		void setSpacing(int spacing)
		{
			if (spacing_ != spacing)
			{
				spacing_ = spacing;
				update();
			}
		}

		void clear() const
		{
			text_document_.clear();
		}

		[[nodiscard]] int alignment() const
		{
			return alignment_;
		}
		[[nodiscard]] int spacing() const
		{
			return spacing_;
		}

		[[nodiscard]] int textWidth() const
		{
			return text_width_;
		}

		/**
		 * @brief Sets the width of the line that will be drawn around the image.
		 * @param width
		 */
		virtual void setTextWidth(int width) = 0;

		void DrawItems(QPainter &painter, const QRect &layoutRect, const QPixmap &pixmap) const;

	private:
		int alignment_;
		int spacing_;
		int text_width_;
		int extra_spacing_;

	protected:
		void AlignWidgetRect(QRectF &widgetRect, const QSizeF &minimumSize) const;

		QRectF CalculatePixmapRect(const QRectF &widgetRect, const QSizeF &minimumSize, const QSizeF &pixmapSize) const;
		QRectF CalculateTextRect(const QRectF &widgetRect, const QSizeF &minimumSize, const QRectF &pixmapRect) const;

		QSizeF MinimumWidgetSize(const QSizeF &pixmap) const;
		QSizeF TextSize() const;
		QSize MaximumImageSize(const QSize &size) const;

		QSize minimumSizeHint() const override
		{
			return {0, 0};
		}

		void paintEvent(QPaintEvent *) override;
		QSize sizeHint() const override;
		int heightForWidth(int width) const override;

		virtual void EnsureTextDocument() const
		{
		}
		virtual void EnsureScaledPixmap();
		QPixmap ScalePixmap(QSize size) const;

		QPixmap image_;
		QPixmap scaled_pixmap_;
		mutable QTextDocument text_document_;
};

#endif //BASEIMAGEDESCRIPTION_H
