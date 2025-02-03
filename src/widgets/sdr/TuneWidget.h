//
// Created by TheDaChicken on 9/12/2024.
//

#ifndef TUNEWIDGET_H
#define TUNEWIDGET_H

#include <QGridLayout>
#include <QPushButton>
#include <RadioChannel.h>

#include "widgets/ClickableQLabel.h"

class TuneWidget : public QWidget
{
		Q_OBJECT

	public:
		constexpr static int NUM_COLUMNS = 5;
		constexpr static int FONT_SIZE = 25;

		explicit TuneWidget(QWidget *parent = nullptr);

		// Adds a digit to the tune number
		void AddDigit(int number);
		// Tunes to the station
		void TuneToStation();
		// Clears the tune number
		void ClearTune();

	Q_SIGNALS:
		void TuneChanged();

	private:
		// Number button clicked event
		void DigitClicked();
		// Update Tune numbers to limit against min and max values for the modulation type (FM/AM)
		void UpdateNumbers() const;
		void UpdateDecimalPoint();
		void keyPressEvent(QKeyEvent *event) override;

		QGridLayout *grid_layout_;

		QLabel *tune_label_;
		// Array of 10 ClickableQLabel objects for the digits 0-9
		ClickableQLabel *digit_buttons[10];
		// Tune to a station
		QPushButton *tune_button_;

		QString tune_number;
		Modulation::Type modulation_type;
};

#endif //TUNEWIDGET_H
