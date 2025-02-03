//
// Created by TheDaChicken on 12/13/2024.
//

#include "TunerPage.h"

#include <Application.h>
#include <QGroupBox>
#include <QVBoxLayout>

#include "models/TunerDevicesModel.h"

TunerPage::TunerPage(QWidget *parent) : QWidget(parent)
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
	device_list_->setModel(dApp->GetTunerDevicesModel());

	//refresh_button_ = new QPushButton(tr("Refresh"), this);
	//refresh_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	device_layout_->addWidget(device_list_);
	//device_layout_->addWidget(refresh_button_);

	gain_settings = new GainSettings(this);

	layout_->addWidget(device_group_box_);
	layout_->addWidget(gain_settings);
	layout_->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	connect(device_list_,
	        &QComboBox::currentIndexChanged,
	        this,
	        &TunerPage::DeviceChanged);
	// connect(refresh_button_,
	//         &QPushButton::clicked,
	//         this, &TunerPage::RefreshDevices);

	if (device_list_->count() > 0)
		DeviceChanged(0);
}

TunerPage::~TunerPage()
{
}

void TunerPage::DeviceChanged(int index)
{
	if (index < 0)
		return;

	const QModelIndex modelIndex = device_list_->model()->index(index, 0);
	const std::shared_ptr<PortSDR::Device> device = dApp->GetTunerDevicesModel()->GetDevice(modelIndex);

	set_device_future_.cancel();
	set_device_future_ = dApp->GetRadioController().SetSDRDevice(device);
	set_device_future_.then(this, [this, device](int ret)
	{
		DeviceChangedCallback(device, ret);
	});
}

void TunerPage::DeviceChangedCallback(const std::shared_ptr<PortSDR::Device> &device, int ret)
{
	if (ret != 0)
	{
		// TODO: Show error message
		Logger::Log(err, "Failed to change Tuner device to: {}", device->name.c_str());
	}
	else
	{
		Logger::Log(info, "Changed Tuner device to: {}", device->name.c_str());
	}

	gain_settings->UpdateGainSettings();
}

void TunerPage::RefreshDevices()
{
	dApp->GetTunerDevicesModel()->BuildDevices(true);
}
