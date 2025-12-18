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

	main_layout_->addWidget(modes_frame_);

	modes_button = new QButtonGroup(modes_frame_);
	modes_button->setExclusive(true);

	modes_frame_->hide();
}

GainSettings::~GainSettings()
= default;

void GainSettings::UpdateTunerStream(PortSDR::Stream *stream)
{
	stream_ = stream;

	UpdateGainModes();
	UpdateGainSliders();
}

std::string ConvertModeToSTR(const PortSDR::GainMode gain_mode)
{
	switch (gain_mode)
	{
		case PortSDR::GAIN_MODE_FREE:
			return "Free";
		case PortSDR::GAIN_MODE_LINEARITY:
			return "Linearity";
		case PortSDR::GAIN_MODE_SENSITIVITY:
			return "Sensitivity";
		default:
			return "";
	}
}

void GainSettings::UpdateGainModes()
{
	ClearGainModes();

	if (!stream_)
		return;

	// Create buttons for each gain mode
	for (const PortSDR::GainMode &mode : stream_->GetGainModes())
	{
		const auto gain_mode = new QRadioButton(this);

		gain_mode->setText(QString::fromStdString(ConvertModeToSTR(mode)));
		gain_mode->setCheckable(true);
		gain_mode->setAutoExclusive(true);
		gain_mode->setChecked(mode == stream_->GetGainMode());

		modes_layout_->addWidget(gain_mode);
		modes_button->addButton(gain_mode);

		connect(gain_mode,
		        &QRadioButton::toggled,
		        this,
		        [this, mode](bool checked)
		        {
		        	auto str = ConvertModeToSTR(mode);
			        const auto ret = stream_->SetGainMode(mode);
			        if (ret != PortSDR::ErrorCode::OK)
			        {
				        Logger::Log(err, "Failed to set gain mode: {}", str);
				        return;
			        }
			        UpdateGainSliders();
			        Logger::Log(info, "Gain mode changed to: {}", str);
		        });
	}

	// Show free gain mode if there are gain modes
	if (!stream_->GetGainModes().empty())
		modes_frame_->show();
	else
		modes_frame_->hide();
}

void GainSettings::UpdateGainSliders()
{
	ClearGainSliders();

	if (!stream_)
		return;

	for (const auto &gain : stream_->GetGainStages())
	{
		CreateGainSlider(gain);
	}
}

void GainSettings::CreateGainSlider(const PortSDR::Gain &gain)
{
	auto *slider = new TextSlider(QString::fromStdString(gain.stage),
	                              "db",
	                              gain.range.Min(),
	                              gain.range.Max(),
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
		modes_layout_->removeWidget(button);
		modes_button->removeButton(button);

		button->deleteLater();
	}
}

void GainSettings::GainFreelyChanged(std::string_view stage, int value) const
{
	const auto ret = stream_->SetGain(value, stage);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set gain stage: {} value: {}", stage, value);
		return;
	}

	Logger::Log(info, "Gain stage: {} value: {}", stage, value);
}
