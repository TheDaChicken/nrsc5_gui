//
// Created by TheDaChicken on 9/12/2024.
//

#ifndef CLICKABLEQLABEL_H
#define CLICKABLEQLABEL_H

#include <QLabel>

class ClickableQLabel : public QLabel
{
		Q_OBJECT

	public:
		explicit ClickableQLabel(QWidget *parent = nullptr);

	Q_SIGNALS:
		void clickable();
	private:
		void mousePressEvent(QMouseEvent *event) override;
};

#endif //CLICKABLEQLABEL_H
