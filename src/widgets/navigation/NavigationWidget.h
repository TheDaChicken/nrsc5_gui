//
// Created by TheDaChicken on 8/15/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGET_NAVIGATIONWIDGET_H_
#define NRSC5_GUI_SRC_WIDGET_NAVIGATIONWIDGET_H_

#include <QHBoxLayout>

#include "widgets/navigation/NavigationHeader.h"
#include "widgets/text/ImageTextLabel.h"

class NavigationWidget : public QFrame
{
	public:
		explicit NavigationWidget(QWidget *parent = nullptr);

		[[nodiscard]] NavigationHeader *ViewLabel() const
		{
			return view_label_;
		}

		[[nodiscard]] ImageTextLabel *ImageLabel() const
		{
			return image_label_;
		}

	private:
		void mousePressEvent(QMouseEvent *event) override;
		QHBoxLayout *layout_;
		ImageTextLabel *image_label_;
		NavigationHeader *view_label_;
};

#endif //NRSC5_GUI_SRC_WIDGET_NAVIGATIONWIDGET_H_
