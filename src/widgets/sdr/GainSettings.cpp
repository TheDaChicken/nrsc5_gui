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

	layout_main_ = new QVBoxLayout(this);
	layout_main_->setObjectName("GainLayout");
	layout_main_->setContentsMargins(0, 0, 0, 0);

	layout_modes_ = new QHBoxLayout(this);
	layout_modes_->setObjectName("GainModesLayout");
	layout_modes_->setContentsMargins(0, 0, 0, 0);

	gain_modes_buttons = new QButtonGroup(this);
	gain_modes_buttons->setExclusive(true);

	// Add a button for free gain. (For all gain stages)
	free_gain_mode_ = new QRadioButton(this);
	free_gain_mode_->setText("Free");
	free_gain_mode_->setCheckable(true);
	free_gain_mode_->setAutoExclusive(true);
	free_gain_mode_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	gain_modes_buttons->addButton(free_gain_mode_);

	layout_modes_->addWidget(free_gain_mode_);
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

		layout_modes_->addWidget(button);
		gain_modes_buttons->addButton(button);
	}

	// Show free gain mode if there are any gain modes
	if (stream->GetGainModes().size() > 0)
	{
		free_gain_mode_->show();
	}
	else
	{
		free_gain_mode_->hide();
	}

	// Default option is free gain
	freely_gain_mode_ = true;
	free_gain_mode_->setChecked(true);
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

void GainSettings::ClearGainSliders()
{
	// Clear buttons
	for (TextSlider *slider : gain_sliders_)
	{
		layout_modes_->removeWidget(slider);
		slider->deleteLater();
	}

	gain_sliders_.clear();
}

void GainSettings::ClearGainModes()
{
	// Clear buttons
	for (QAbstractButton *button : gain_modes_buttons->buttons())
	{
		layout_modes_->removeWidget(button);
		gain_modes_buttons->removeButton(button);
	}

	gain_modes_buttons->buttons().clear();
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

void GainSettings::CreateGainSlider(PortSDR::Gain gain)
{
	TextSlider *slider = new TextSlider(QString::fromStdString(gain.stage),
	                                    "db",
	                                    gain.range.min(),
	                                    gain.range.max(),
	                                    this);

	gain_sliders_.push_back(slider);
	layout_main_->addWidget(slider);

	connect(slider->Slider(),
	        &QSlider::valueChanged,
	        this,
	        [this, gain](const int value)
	        {
		        GainFreelyChanged(gain.stage, value);
	        });
}
