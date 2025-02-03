//
// Created by TheDaChicken on 8/10/2024.
//

#ifndef NRSC5_GUI_SRC_CHANNELS_LINKEDCHANNELMODEL_H_
#define NRSC5_GUI_SRC_CHANNELS_LINKEDCHANNELMODEL_H_

#include "ChannelModel.h"

/**
 * @brief Channel model that allows for moving channels around.
 */
class LinkedChannelModel : public ChannelModel
{
		Q_OBJECT

	public:
		explicit LinkedChannelModel(QObject *parent,
		                            const QSqlDatabase &db,
		                            std::weak_ptr<StationImageProvider> image_provider);

		void SetTable(const QString &table) override;
		bool InsertChannel(int row, const RadioChannel &channel);
		bool AddChannels(int row, const std::vector<RadioChannel> &channels);

		bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
		              int destinationChild) override;

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
		[[nodiscard]] Qt::DropActions supportedDropActions() const override;
		[[nodiscard]] bool dropMimeData(const QMimeData *data, Qt::DropAction action,
		                                int row, int column, const QModelIndex &parent) override;
		[[nodiscard]] QSqlRecord GetRecord(int row) const;

	public Q_SLOTS:
		bool submit() override;
		void revert() override;

	private:
		float CalculatePosition(int row, bool move = false) const;
		void CreateTable(const QString &table) override;
};

#endif //NRSC5_GUI_SRC_CHANNELS_LINKEDCHANNELMODEL_H_
