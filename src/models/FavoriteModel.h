//
// Created by TheDaChicken on 7/18/2024.
//

#ifndef NRSC5_GUI_SRC_MODELS_FAVORITESMODEL_H_
#define NRSC5_GUI_SRC_MODELS_FAVORITESMODEL_H_

#include <QList>
#include <set>
#include <QtSql/QSqlTableModel>

#include "RadioChannel.h"

#include "images/StationImageProvider.h"

class FavoriteModel : public QAbstractListModel
{
  Q_OBJECT

 public:
  enum Roles
  {
   kIsMoveable = Qt::UserRole + 1,
   kIsDeletable = Qt::UserRole + 2,
  };

  explicit FavoriteModel(SQLite::Database &db,
                         const StationImageProvider &image_provider, QObject *parent = nullptr);
  Q_DISABLE_COPY_MOVE(FavoriteModel)

  bool Set(int row, const Channel &channel);
  bool Add(const Channel &channel);
  bool Extend(int index, const std::vector<Channel>& channels);
  bool Remove(const Channel &channel);

  [[nodiscard]] const Channel &Get(int row) const;
  [[nodiscard]] QModelIndex Find(const Channel &channel) const;

  [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

  [[nodiscard]] QStringList mimeTypes() const override;
  [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;

  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
  [[nodiscard]] Qt::DropActions supportedDropActions() const override;

  bool removeRows(int position, int rows, const QModelIndex &parent) override;
  bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                const QModelIndex &destinationParent, int destinationChild);
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &parent) override;

 public Q_SLOTS:
  bool update();
  bool submit() override;

 private:
  SQLite::Database &db_;
  std::vector<Channel> channels_;
  const StationImageProvider& image_provider_;
};

#endif //NRSC5_GUI_SRC_MODELS_FAVORITESMODEL_H_
