//
// Created by TheDaChicken on 12/27/2024.
//

#ifndef TUNERDEVICESMODEL_H
#define TUNERDEVICESMODEL_H

#include <PortSDR.h>
#include <QAbstractTableModel>

struct TunerDevice
{
  PortSDR::DeviceInfo info;
  PortSDR::Device device;
};

class TunerDevicesModel final : public QAbstractListModel
{
  public:
    explicit TunerDevicesModel(const std::weak_ptr<PortSDR::PortSDR> &sdr, QObject *parent = nullptr);

    void BuildDevices(bool refresh = false);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    [[nodiscard]] TunerDevice GetDevice(const QModelIndex &index) const;
  private:
    std::weak_ptr<PortSDR::PortSDR> m_sdr;
    std::vector<TunerDevice > m_devices;
};

#endif //TUNERDEVICESMODEL_H
