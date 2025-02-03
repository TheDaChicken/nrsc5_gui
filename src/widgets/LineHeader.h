//
// Created by TheDaChicken on 9/6/2024.
//

#ifndef LINEHEADER_H
#define LINEHEADER_H

#include <QLabel>
#include <QWidget>

/**
* @brief A widget that displays a line with text above on the left and right.
*/
class LineHeader : public QFrame
{
		Q_OBJECT

	public:
	    constexpr static int kHeaderMargin = 20;
		constexpr static int kHeaderThickness = 2;
		constexpr static int kHeaderHozMarginText = 2;

		explicit LineHeader(QWidget *parent = nullptr);

		void SetText(const QString &leftText, const QString &rightText);

	protected:
		void paintEvent(QPaintEvent *event) override;
		[[nodiscard]] QSize sizeHint() const override;

	protected:
		QString leftText_;
		QString rightText_;

};

#endif //LINEHEADER_H
