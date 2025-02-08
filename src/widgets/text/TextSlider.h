//
// Created by TheDaChicken on 12/27/2024.
//

#ifndef NAMEDSLIDER_H
#define NAMEDSLIDER_H

#include <QLabel>
#include <qslider.h>
#include <QVBoxLayout>
#include <QWidget>
#include <QGroupBox>

/**
 * @brief A slider with a name label
 */
class TextSlider : public QWidget
{
		Q_OBJECT

	public:
		TextSlider(const QString &name, const QString &units, double min, double max, QWidget *parent = nullptr);

		[[nodiscard]] QSlider *Slider() const
		{
			assert(slider_);
			return slider_;
		}

	private slots:
		void SliderValueChanged(int value);

	private:
		QVBoxLayout *layout_;
		QHBoxLayout *text_layout_;

		QVBoxLayout *slider_layout_;
		QSlider *slider_;

		QLabel *left_label_;
		QLabel *right_label_;
		QString units_;
};

#endif //NAMEDSLIDER_H
