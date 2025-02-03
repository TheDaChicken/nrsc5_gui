//
// Created by TheDaChicken on 12/13/2024.
//

#ifndef TUNERPAGE_H
#define TUNERPAGE_H


#include <QComboBox>
#include <QFuture>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "widgets/sdr/GainSettings.h"

class TunerPage : public QWidget
{
		Q_OBJECT

	public:
		explicit TunerPage(QWidget *parent = nullptr);
		~TunerPage() override;

	private:
		QVBoxLayout *layout_;

		QGroupBox *device_group_box_;
		QComboBox *device_list_;
		QPushButton *refresh_button_;
		QVBoxLayout *device_layout_;

		GainSettings *gain_settings;

		QFuture<int> set_device_future_;

		void DeviceChangedCallback(const std::shared_ptr<PortSDR::Device> &shared, int ret);

	private slots:
		void DeviceChanged(int index);
		void RefreshDevices();
};

#endif //TUNERPAGE_H
