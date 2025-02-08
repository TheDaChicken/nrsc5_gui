//
// Created by TheDaChicken on 12/13/2024.
//

#include "GainSettings.h"

#include <Application.h>
#include <QRadioButton>
#include <widgets/text/TextSlider.h>

GainSettings::GainSettings(QWidget *parent) : QGroupBox(parent)
{
	setObjectName("GainSettings");
	setTitle(tr("Gains:"));
	setAlignment(Qt::AlignLeft);

	main_layout_ = new QVBoxLayout(this);
	main_layout_->setObjectName("GainLayout");
	main_layout_->setContentsMargins(9, 3, 9, 3);

	modes_frame_ = new QFrame(this);
	modes_frame_->setObjectName("GainModesFrame");
	modes_frame_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	modes_layout_ = new QHBoxLayout(modes_frame_);
	modes_layout_->setObjectName("GainModesLayout");
	modes_layout_->setContentsMargins(0, 0, 0, 0);

	// Add a button for free gain. (For all gain stages)
	mode_free_gain_ = new QRadioButton(modes_frame_);
	mode_free_gain_->setText("Free");
	mode_free_gain_->setCheckable(true);
	mode_free_gain_->setAutoExclusive(true);
	mode_free_gain_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	modes_layout_->addWidget(mode_free_gain_);

	main_layout_->addWidget(modes_frame_);

	modes_button = new QButtonGroup(modes_frame_);
	modes_button->setExclusive(true);
	modes_button->addButton(mode_free_gain_);

	modes_frame_->hide();
}

GainSettings::~GainSettings()
{
}

void GainSettings::UpdateGainSettings()
{
	UpdateGainModes();
	UpdateGainSliders();
}

void GainSettings::UpdateGainModes()
{
	ClearGainModes();

	PortSDR::Stream *stream = dApp->GetRadioController().GetRadio().GetSDRStream();

	if (!stream)
		return;

	// Create buttons for each gain mode
	for (const std::string &modes : stream->GetGainModes())
	{
		QRadioButton *button = new QRadioButton(this);

		button->setText(QString::fromStdString(modes));
		button->setCheckable(true);
		button->setAutoExclusive(true);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		modes_layout_->addWidget(button);
		modes_button->addButton(button);
	}

	// Show free gain mode if there are any gain modes
	if (!stream->GetGainModes().empty())
	{
		modes_frame_->show();
	}
	else
	{
		modes_frame_->hide();
	}

	// Default option is free gain
	freely_gain_mode_ = true;
	mode_free_gain_->setChecked(true);
}

void GainSettings::UpdateGainSliders()
{
	ClearGainSliders();

	PortSDR::Stream *stream = dApp->GetRadioController().GetRadio().GetSDRStream();

	if (!stream)
		return;

	if (freely_gain_mode_)
	{
		for (const auto &gain : stream->GetGainStages())
		{
			CreateGainSlider(gain);
		}
	}
	else
	{
		CreateGainSlider(stream->GetGainStage());
	}
}

void GainSettings::CreateGainSlider(const PortSDR::Gain &gain)
{
	auto *slider = new TextSlider(QString::fromStdString(gain.stage),
	                              "db",
	                              gain.range.min(),
	                              gain.range.max(),
	                              this);

	gain_sliders_.push_back(slider);
	main_layout_->addWidget(slider);

	connect(slider->Slider(),
	        &QSlider::valueChanged,
	        this,
	        [this, gain](const int value)
	        {
		        GainFreelyChanged(gain.stage, value);
	        });
}

void GainSettings::ClearGainSliders()
{
	// Clear buttons
	for (TextSlider *slider : gain_sliders_)
	{
		modes_layout_->removeWidget(slider);
		gain_sliders_.removeOne(slider);

		slider->deleteLater();
	}
}

void GainSettings::ClearGainModes()
{
	// Clear buttons
	for (QAbstractButton *button : modes_button->buttons())
	{
		if (button == mode_free_gain_)
			continue;

		modes_layout_->removeWidget(button);
		modes_button->removeButton(button);

		button->deleteLater();
	}
}

void GainSettings::GainFreelyChanged(std::string_view stage, int value)
{
	PortSDR::Stream *stream = dApp->GetRadioController().GetRadio().GetSDRStream();

	int ret = stream->SetGain(value, stage);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set gain stage: {} value: {}", stage, value);
		return;
	}

	Logger::Log(info, "Gain stage: {} value: {}", stage, value);
}

void GainSettings::GainChanged(int value)
{
	PortSDR::Stream *stream = dApp->GetRadioController().GetRadio().GetSDRStream();

	stream->SetGain(value);
}

