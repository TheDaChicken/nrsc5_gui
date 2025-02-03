//
// Created by TheDaChicken on 12/27/2024.
//

#include "TunerDevicesModel.h"

#include <utils/Log.h>

TunerDevicesModel::TunerDevicesModel(const std::weak_ptr<PortSDR::PortSDR> &sdr, QObject *parent)
	: QAbstractListModel(parent),
	  m_sdr(sdr)
{
	BuildDevices();
}

void TunerDevicesModel::BuildDevices(const bool refresh)
{
	beginResetModel();

	m_devices.clear();

	if (const auto sdr = m_sdr.lock())
	{
		for (const auto &host : sdr->GetHosts())
		{
			if (refresh)
				host->RefreshDevices();

			for (const auto &device : host->Devices())
			{
				m_devices.push_back(device);
			}
		}
	}

	endResetModel();
}

std::shared_ptr<PortSDR::Device> TunerDevicesModel::GetDevice(const QModelIndex &index) const
{
	return m_devices[index.row()];
}

int TunerDevicesModel::rowCount(const QModelIndex &parent) const
{
	return m_devices.size();
}

QVariant TunerDevicesModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return {};

	if (index.row() >= m_devices.size())
		return {};

	if (role == Qt::DisplayRole)
	{
		return QString::fromStdString(m_devices[index.row()]->name);
	}

	return {};
}

Qt::ItemFlags TunerDevicesModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	if (index.row() >= m_devices.size())
		return {};

	const std::shared_ptr<PortSDR::Device> device = m_devices[index.row()];

	return device->unavailable ? Qt::NoItemFlags : (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}
