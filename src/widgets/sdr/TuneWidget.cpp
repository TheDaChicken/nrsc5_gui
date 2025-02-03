//
// Created by TheDaChicken on 9/12/2024.
//

#include "TuneWidget.h"

#include <QLabel>
#include <Application.h>

TuneWidget::TuneWidget(QWidget *parent)
	: QWidget(parent), grid_layout_(new QGridLayout(this)),
	  tune_label_(new QLabel(this)), digit_buttons{},
	  tune_button_(new QPushButton(this)), modulation_type(Modulation::Type::MOD_FM)
{
	setFocusPolicy(Qt::ClickFocus);

	grid_layout_->setObjectName(QString::fromUtf8("tune_layout_"));
	grid_layout_->setSpacing(4);
	grid_layout_->setContentsMargins(20, 20, 20, 20);

	tune_label_->setObjectName("TuneLabel");
	tune_label_->setAlignment(Qt::AlignCenter);
	tune_label_->setStyleSheet("font-size: 25pt; color: #1f71d4;");

	tune_button_->setObjectName("tune_button");
	tune_button_->setText("Tune");
	tune_button_->setStyleSheet("font-size: 25pt; color: #1f71d4;");
	tune_button_->setMinimumSize({200, 100});
	tune_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	// Create Digit Buttons
	for (int i = 0; i < 10; i++)
	{
		digit_buttons[i] = new ClickableQLabel(this);
		digit_buttons[i]->setObjectName("tune_number_" + QString::number(i));
		digit_buttons[i]->setAlignment(Qt::AlignCenter);
		digit_buttons[i]->setText(QString::number(i));
		digit_buttons[i]->setStyleSheet("font-size: 25pt;");

		// Add Widget to Layout
		// 1-4 is on first row
		if (i <= 4)
		{
			grid_layout_->addWidget(digit_buttons[i], 1, i, 1, 1);
		}
		else
		{
			grid_layout_->addWidget(digit_buttons[i], 2, i - 5, 1, 1);
		}
	}

	grid_layout_->addWidget(tune_label_, 0, 0, 1, 5);
	grid_layout_->addWidget(tune_button_, 3, 0, 1, 5);

	// Connect signals
	for (const auto &digit_button : digit_buttons)
	{
		connect(digit_button, &ClickableQLabel::clickable, this, &TuneWidget::DigitClicked);
	}

	connect(tune_button_, &QPushButton::clicked, this, &TuneWidget::TuneToStation);

	ClearTune();
}

void TuneWidget::TuneToStation()
{
	const Modulation& modulation = GetModulation(modulation_type);
	const auto station = tune_number.toFloat();

	emit TuneChanged();
	dApp->GetRadioController().SetChannel({modulation.type, station, 0});

	ClearTune();
}

void TuneWidget::AddDigit(const int number)
{
	tune_number += QString::number(number);
	tune_label_->setText(tune_number);

	UpdateDecimalPoint();
	UpdateNumbers();
}

void TuneWidget::ClearTune()
{
	tune_number.clear();
	tune_label_->setText("Tune to a station");

	UpdateNumbers();
}

void TuneWidget::DigitClicked()
{
	const auto *label = qobject_cast<ClickableQLabel *>(sender());
	const auto digit = label->text().toInt();

	AddDigit(digit);
}

/**
 * Hide buttons and show only the ones that user can type based on the modulation type
 * We are typing each individual digit, we disable the buttons that are not in the range
 *
 * Example for FM: I can type either 8-9 or 1.
 * 1 will allow more digits to be typed in. 8-9 will not because the range is too large.
 */
void TuneWidget::UpdateNumbers() const
{
	const Modulation& modulation = GetModulation(modulation_type);

	const QString minStr = QString::number(modulation.min);
	const QString maxStr = QString::number(modulation.max);
	const qsizetype position = tune_number.length() + 1;

	// Get the range of the number based on the position typed in to compare new input to
	const double rangeMin = QStringView(minStr).left(position).toDouble();
	const double rangeMax = QStringView(maxStr).left(position).toDouble();

	for (int i = 0; i < 10; i++)
	{
		const double potential = (tune_number + QString::number(i)).toDouble();

		// Max range may have more digits than the minimum range.
		// This causes the range to be invalid. Check below || condition to fix this
		digit_buttons[i]->setEnabled(rangeMax < rangeMin
			                             ? potential >= rangeMin || (potential <= rangeMax && potential > 0)
			                             : potential >= rangeMin && potential <= rangeMax);
	}

	// Enable the tune button if the number is in the range
	tune_button_->setEnabled(tune_number.toDouble() >= modulation.min && tune_number.toDouble() <= modulation.max);
}

/**
 * Adds a decimal point to the tune number if we cannot add any more digits on FM.
 * This is to prevent the user from typing in a number that is too large for FM
 */
void TuneWidget::UpdateDecimalPoint()
{
	const Modulation& modulation = GetModulation(modulation_type);

	if (QString::number(modulation.min).contains('.'))
	{
		if ((tune_number + "0").toDouble() > modulation.max)
		{
			tune_number.append('.');
			tune_label_->setText(tune_number);
		}
	}
}

void TuneWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_0:
			AddDigit(0);
			break;
		case Qt::Key_1:
			AddDigit(1);
			break;
		case Qt::Key_2:
			AddDigit(2);
			break;
		case Qt::Key_3:
			AddDigit(3);
			break;
		case Qt::Key_4:
			AddDigit(4);
			break;
		case Qt::Key_5:
			AddDigit(5);
			break;
		case Qt::Key_6:
			AddDigit(6);
			break;
		case Qt::Key_7:
			AddDigit(7);
			break;
		case Qt::Key_8:
			AddDigit(8);
			break;
		case Qt::Key_9:
			AddDigit(9);
			break;
		case Qt::Key_Return:
			ClearTune();
			break;
		default:
			QWidget::keyPressEvent(event);
	}
}
