//
// Created by TheDaChicken on 8/10/2024.
//

#include "LinkedChannelModel.h"
#include "utils/Log.h"

#include <QMimeData>
#include <QSqlRecord>
#include <QSqlError>

#include <nlohmann/json.hpp>
#include <QSqlQuery>

// We use a linked list model (prev column & id) to allow for reordering of channels
// without having to update the entire table.
constexpr auto kCreateLinkedChannelTable = QLatin1String(R"(
	CREATE TABLE IF NOT EXISTS %1 (
		id INTEGER PRIMARY KEY,
		pos REAL,
		type INTEGER,
		frequency INTEGER,
		program_id_ INTEGER,
		callSign TEXT,
		FOREIGN KEY (callSign) REFERENCES HDRADIO_PROGRAMS(callSign)
	);
	)");

constexpr auto kChannelPosId = 1;
constexpr auto kChannelStep = 100;

LinkedChannelModel::LinkedChannelModel(QObject *parent, const QSqlDatabase &database,
                                       std::weak_ptr<StationImageProvider> image_provider)
	: ChannelModel(parent, database, image_provider)
{
	connect(sql,
	        &QSqlTableModel::primeInsert,
	        this,
	        [this](int row, QSqlRecord &record)
	        {
		        // Use our hidden values for the linked list
		        // Set the previous id to the previous row's id
		        // Create our own ID from primary keys
		        record.setValue(kChannelPosId, CalculatePosition(row));
		        record.setGenerated(kChannelPosId, true);
	        });
}

void LinkedChannelModel::CreateTable(const QString &table)
{
	const QString kCreateTableQuery = kCreateLinkedChannelTable.arg(table);

	QSqlQuery q(sql->database());
	if (!q.exec(kCreateTableQuery))
	{
		Logger::Log(info, "Failed to create table: {} {}", table, q.lastError().text());
	}
}

void LinkedChannelModel::SetTable(const QString &table)
{
	ChannelModel::SetTable(table);

	// Sort it by previous
	sql->sort(kChannelPosId, Qt::AscendingOrder);
}

bool LinkedChannelModel::InsertChannel(int row, const RadioChannel &channel)
{
	beginInsertRows(QModelIndex(), row, row);

	if (!sql->insertRows(row, row + 1))
	{
		Logger::Log(info, "Failed to insert rows: {}", sql->lastError().text());
		return false;
	}

	QSqlRecord record = CreateRecord(row, channel);
	if (!sql->setRecord(row, record))
	{
		Logger::Log(info, "Failed to set channel");
		sql->revertAll();
		return false;
	}

	endInsertRows();
	return true;
}

bool LinkedChannelModel::AddChannels(int row, const std::vector<RadioChannel> &channels)
{
	int count = static_cast<int>(channels.size());
	beginInsertRows(QModelIndex(), row, row + count - 1);

	if (!sql->insertRows(row, count))
	{
		Logger::Log(info, "Failed to insert rows: {}", sql->lastError().text());
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		QSqlRecord record = CreateRecord(row + i, channels[i]);
		if (!sql->setRecord(row + i, record))
		{
			Logger::Log(info, "Failed to set channel");
			sql->revertAll();
			return false;
		}
	}

	endInsertRows();
	return true;
}

/**
 * @brief Moves rows safely by updating the previous id of the rows.
 * @param sourceParent
 * @param sourceRow
 * @param count
 * @param destinationParent
 * @param destinationChild
 * @return
 */
bool LinkedChannelModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                                  const QModelIndex &destinationParent, int destinationChild)
{
	if (sourceParent.isValid() || destinationParent.isValid())
		return false;

	if (!beginMoveRows(QModelIndex(),
	                   sourceRow,
	                   sourceRow + count - 1,
	                   QModelIndex(),
	                   destinationChild))
		return false;

	int fromRow = sourceRow;
	if (destinationChild < sourceRow)
		fromRow += count - 1;
	else
		destinationChild--;

	// Move current rows
	for (int i = 0; i < count; ++i)
	{
		int row = fromRow + i;
		int des = destinationChild + i;

		QSqlRecord record = sql->record(row);
		record.setValue(kChannelPosId, CalculatePosition(des, true));
		record.setGenerated(kChannelPosId, true);

		if (!sql->setRecord(row, record))
		{
			Logger::Log(err, "Failed to set record: {}", sql->lastError().text());
			return false;
		}
	}

	endMoveRows();

	select();
	return true;
}

Qt::DropActions LinkedChannelModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::ItemFlags LinkedChannelModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid())
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	else
		return Qt::ItemIsDropEnabled | defaultFlags;
}

bool LinkedChannelModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                      const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/json"))
		return false;

	QByteArray byteArray = data->data("application/json");
	nlohmann::json array = nlohmann::json::parse(byteArray.toStdString());

	Logger::Log(info, "Dropped: {}", array.dump());

	int beginRow;

	if (row != -1)
		beginRow = row;
	else if (parent.isValid())
		beginRow = parent.row();
	else
		beginRow = rowCount(QModelIndex());

	if (array.is_array())
	{
		AddChannels(beginRow, array.get<std::vector<RadioChannel> >());
	}
	else
	{
		// TODO: Implement single drop
		return false;
	}

	return true;
}

float LinkedChannelModel::CalculatePosition(const int row, const bool move) const
{
	float posId;

	if (row == 0)
	{
		if(move)
		{
			posId = sql->record(row).value(kChannelPosId).toFloat() - kChannelStep;
		}
		else
		{
			// Minimum value for the first row
			if (row + 1 < rowCount())
				posId = sql->record(row + 1).value(kChannelPosId).toFloat() - kChannelStep;
			else
				posId = 0;
		}
	}
	else if (row == rowCount() - 1)
	{
		if (move)
		{
			// We need to find the latest row's position and add the step to it.
			// Then our new row will be the last row.
			// This is because: row = destination row
			posId = sql->record(row).value(kChannelPosId).toFloat() + kChannelStep;
			Logger::Log(debug, "Max Pos: {}", posId);
		}
		else
		{
			// When creating a new row,
			// We need to find the previous row's position and add the step to it.
			posId = sql->record(row - 1).value(kChannelPosId).toFloat() + kChannelStep;
			Logger::Log(debug, "Max Pos: {}", posId);
		}
	}
	else
	{
		// Middle rows
		float belowPos = sql->record(row - 1).value(kChannelPosId).toFloat();
		float abovePos = sql->record(row + 1).value(kChannelPosId).toFloat();
		posId = (abovePos + belowPos) / 2;
		Logger::Log(debug, "Pos: {}", posId);
	}

	return posId;
}

bool LinkedChannelModel::submit()
{
	return ChannelModel::submit();
}

void LinkedChannelModel::revert()
{
	ChannelModel::revert();
}

QSqlRecord LinkedChannelModel::GetRecord(int row) const
{
	return sql->record(row);
}