//
// Created by TheDaChicken on 12/13/2024.
//

#include "TunerPage.h"

#include <Application.h>
#include <QGroupBox>
#include <QVBoxLayout>

#include "models/TunerDevicesModel.h"

TunerPage::TunerPage(QWidget *parent)
	: QWidget(parent)
{
	layout_ = new QVBoxLayout(this);
	layout_->setContentsMargins(18, 18, 18, 18);
	layout_->setSpacing(8);
	layout_->setObjectName("TunerPageLayout");

	device_group_box_ = new QGroupBox(tr("Device:"), this);
	device_group_box_->setAlignment(Qt::AlignLeft);
	device_group_box_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	device_layout_ = new QVBoxLayout(device_group_box_);
	device_layout_->setContentsMargins(8, 3, 8, 3);
	device_layout_->setSpacing(5);
	device_layout_->setObjectName("DeviceLayout");

	device_list_ = new QComboBox(this);
	device_layout_->addWidget(device_list_);

	// refresh_button_ = new QPushButton(tr("Refresh"), this);
	// refresh_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	// device_layout_->addWidget(refresh_button_);

	gain_settings = new GainSettings(this);

	layout_->addWidget(device_group_box_);
	layout_->addWidget(gain_settings);
	layout_->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	connect(&getApp()->GetRadioController().GetTuner(),
	        &HybridTuner::TunerStream,
	        this,
	        &TunerPage::UpdateTunerStream);
	connect(device_list_,
	        &QComboBox::currentIndexChanged,
	        this,
	        &TunerPage::DeviceChanged);
	// connect(refresh_button_,
	//         &QPushButton::clicked,
	//         this,
	//         &TunerPage::RefreshDevices);

	device_list_->setModel(getApp()->GetTunerDevicesModel());
}

TunerPage::~TunerPage()
= default;

void TunerPage::UpdateTunerStream(PortSDR::Stream *stream) const
{
	gain_settings->UpdateTunerStream(stream);
}

void TunerPage::DeviceChanged(const int index)
{
	if (index < 0)
	{
		getApp()->GetRadioController().ClearSDRDevice();
	}
	else
	{
		const QModelIndex modelIndex = device_list_->model()->index(index, 0);
		const TunerDevice device = getApp()->GetTunerDevicesModel()->GetDevice(modelIndex);

		getApp()->GetRadioController().SetSDRDevice(device.device);

		Logger::Log(debug, "Device changed to: {}", device.info.name);
	}
}

void TunerPage::RefreshDevices()
{
	getApp()->GetRadioController().ClearSDRDevice();
	getApp()->GetTunerDevicesModel()->BuildDevices(true);
}

