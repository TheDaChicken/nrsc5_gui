//
// Created by TheDaChicken on 7/18/2024.
//

#ifndef NRSC5_GUI_SRC_MODELS_FAVORITESMODEL_H_
#define NRSC5_GUI_SRC_MODELS_FAVORITESMODEL_H_

#include <QAbstractTableModel>
#include <QList>
#include <QtSql/QSqlTableModel>

#include "RadioChannel.h"

#include "images/StationImageProvider.h"

class StationInfoManager;

class ChannelModel : public QAbstractListModel
{
  Q_OBJECT

 public:
  enum Roles
  {
   kIsMoveable = Qt::UserRole + 1,
   kIsDeletable = Qt::UserRole + 2,
  };

  explicit ChannelModel(QObject *parent,
                        const QSqlDatabase &db,
                        const std::weak_ptr<StationImageProvider> &image_provider);
  Q_DISABLE_COPY(ChannelModel)

  virtual void SetTable(const QString &table);

  bool SetChannel(int row, const RadioChannel &channel);
  bool AddChannel(const RadioChannel &channel, bool duplicate = false);
  bool RemoveChannel(const RadioChannel &channel);

  [[nodiscard]] RadioChannel GetChannel(int row) const;
  [[nodiscard]] QModelIndex FindChannel(const RadioChannel &channel) const;

  [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

  [[nodiscard]] QStringList mimeTypes() const override;
  [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
  bool removeRows(int position, int rows, const QModelIndex &parent) override;

 public Q_SLOTS:
  bool select();
  bool submit() override;
  void revert() override;

 protected:
  virtual void CreateTable(const QString &table);
  QSqlRecord CreateRecord(int row, const RadioChannel &channel) const;

  QSqlTableModel *sql;
 private:
  std::weak_ptr<StationImageProvider> image_provider_;
};

#endif //NRSC5_GUI_SRC_MODELS_FAVORITESMODEL_H_
