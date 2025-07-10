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

	modes_layout_->addWidget(mode_free_gain_);

	main_layout_->addWidget(modes_frame_);

	modes_button = new QButtonGroup(modes_frame_);
	modes_button->setExclusive(true);
	modes_button->addButton(mode_free_gain_);

	modes_frame_->hide();

	connect(mode_free_gain_,
	        &QRadioButton::toggled,
	        this,
	        [this](bool checked)
	        {
		        freely_gain_mode_ = true;
		        UpdateGainSliders();
		        Logger::Log(info, "Gain mode changed to: Free");
	        });
}

GainSettings::~GainSettings()
= default;

void GainSettings::UpdateTunerStream(PortSDR::Stream *stream)
{
	stream_ = stream;

	UpdateGainModes();
	UpdateGainSliders();
}

void GainSettings::UpdateGainModes()
{
	ClearGainModes();

	if (!stream_)
		return;

	// Create buttons for each gain mode
	for (const std::string &modes : stream_->GetGainModes())
	{
		QRadioButton *gain_mode = new QRadioButton(this);

		gain_mode->setText(QString::fromStdString(modes));
		gain_mode->setCheckable(true);
		gain_mode->setAutoExclusive(true);

		modes_layout_->addWidget(gain_mode);
		modes_button->addButton(gain_mode);

		connect(gain_mode,
		        &QRadioButton::toggled,
		        this,
		        [this, modes](bool checked)
		        {
			        int ret = stream_->SetGainModes(modes);
			        if (ret < 0)
			        {
				        Logger::Log(err, "Failed to set gain mode: {}", modes);
				        return;
			        }
		        	freely_gain_mode_ = false;
			        UpdateGainSliders();
			        Logger::Log(info, "Gain mode changed to: {}", modes);
		        });
	}

	// Show free gain mode if there are gain modes
	if (!stream_->GetGainModes().empty())
		modes_frame_->show();
	else
		modes_frame_->hide();

	// Default option is free gain
	freely_gain_mode_ = true;
	mode_free_gain_->setChecked(true);
}

void GainSettings::UpdateGainSliders()
{
	ClearGainSliders();

	if (!stream_)
		return;

	if (freely_gain_mode_)
	{
		for (const auto &gain : stream_->GetGainStages())
		{
			CreateGainSlider(gain);
		}
	}
	else
	{
		CreateGainSlider(stream_->GetGainStage());
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
		        if (!freely_gain_mode_)
		        {
			        GainChanged(value);
		        }
		        else
		        {
			        GainFreelyChanged(gain.stage, value);
		        }
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

void GainSettings::GainFreelyChanged(std::string_view stage, int value) const
{
	int ret = stream_->SetGain(value, stage);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set gain stage: {} value: {}", stage, value);
		return;
	}

	Logger::Log(info, "Gain stage: {} value: {}", stage, value);
}

void GainSettings::GainChanged(const int value) const
{
	int ret = stream_->SetGain(value);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set gain value: {}", value);
		return;
	}

	Logger::Log(info, "Gain changed to value: {}", value);
}

