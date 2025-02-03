//
// Created by TheDaChicken on 7/18/2024.
//

#include "ChannelModel.h"
#include "utils/Log.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QMimeData>

#include <nlohmann/json.hpp>

// Create a table for the channel model
const auto kCreateLinkedChannelTable = QLatin1String(R"(
	CREATE TABLE IF NOT EXISTS %1 (
		type INTEGER,
		frequency INTEGER,
		program_id_ INTEGER,
		callSign TEXT,
		PRIMARY KEY (frequency, program_id_, callSign),
		FOREIGN KEY (callSign) REFERENCES HDRADIO_PROGRAMS(callSign)
	);
	)");

const auto kChannelTypeRow = QLatin1String("type");
const auto kChannelFrequencyRow = QLatin1String("frequency");
const auto kChannelProgramIdRow = QLatin1String("program_id_");
const auto kChannelCallSignRow = QLatin1String("callSign");

ChannelModel::ChannelModel(QObject *parent, const QSqlDatabase &db,
                           const std::weak_ptr<StationImageProvider> &image_provider)
	: QAbstractListModel(parent),
	  sql(new QSqlTableModel(parent, db)),
	  image_provider_(image_provider)
{
}

void ChannelModel::CreateTable(const QString &table)
{
	const QString kCreateTableQuery = kCreateLinkedChannelTable.arg(table);

	QSqlQuery q(sql->database());
	if (!q.exec(kCreateTableQuery))
	{
		Logger::Log(err, "Failed to create table: {}", table);

		if (q.lastError().isValid())
		{
			Logger::Log(info, "Error: {}", q.lastError().text());
		}
	}
}

void ChannelModel::SetTable(const QString &table)
{
	CreateTable(table);

	sql->setTable(table);
	sql->setEditStrategy(QSqlTableModel::OnRowChange);

	if (!sql->select())
	{
		Logger::Log(err, "Failed to select table: {}", table);
	}

	if (sql->lastError().isValid())
	{
		Logger::Log(err, "Error: {}", sql->lastError().text());
	}
}

QModelIndex ChannelModel::FindChannel(const RadioChannel &channel) const
{
	RadioChannel temp_channel;
	for (int i = 0; i < rowCount(); ++i)
	{
		if (GetChannel(i) == channel)
			return createIndex(i, 0);
	}
	return {};
}

bool ChannelModel::SetChannel(int row, const RadioChannel &channel)
{
	QSqlRecord record = CreateRecord(row, channel);
	if (!sql->setRecord(row, record))
		return false;

	emit dataChanged(createIndex(row, 0), createIndex(row, 0), {Qt::DisplayRole, Qt::DecorationRole});
	return true;
}

bool ChannelModel::AddChannel(const RadioChannel &channel, bool duplicate)
{
	int row = rowCount();

	if (!duplicate && FindChannel(channel).isValid())
	{
		Logger::Log(warn, "Channel already exists");
		return false;
	}

	beginInsertRows(QModelIndex(), row, row);

	if (!sql->insertRows(row, 1))
	{
		Logger::Log(warn, "Failed to insert row");
		return false;
	}

	if (!sql->setRecord(row, CreateRecord(row, channel)))
	{
		Logger::Log(warn, "Failed to add channel");
		sql->revertAll();
		return false;
	}

	endInsertRows();
	return true;
}

bool ChannelModel::RemoveChannel(const RadioChannel &channel)
{
	QModelIndex index = FindChannel(channel);

	if (!index.isValid())
		return false;

	return removeRows(index.row(), 1, index.parent());
}

QVariant ChannelModel::headerData(int column, Qt::Orientation orientation, int role) const
{
	// Vertical header not supported
	if (orientation == Qt::Vertical)
	{
		return {};
	}

	// TODO: Implement this later
	if (role == kIsMoveable)
	{
		return true;
	}
	return {};
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return {};

	RadioChannel temp_channel(GetChannel(index.row()));

	switch (role)
	{
		case Qt::DisplayRole:
		{
			return temp_channel.GetDisplayChannel();
		}
		case Qt::DecorationRole:
		{
			if (auto p = image_provider_.lock())
			{
				return p->FetchStationImage(temp_channel).image;
			}

			return {};
		}
		case kIsDeletable:
		{
			// TODO: Implement this later
			return true;
		}
		default:
			return {};
	}
}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return sql->rowCount();
}

bool ChannelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	return false;
}

bool ChannelModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent)

	beginInsertRows(QModelIndex(), position, position + rows - 1);
	bool success = sql->insertRows(position, rows);
	endInsertRows();

	return success;
}

bool ChannelModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent)

	beginRemoveRows(QModelIndex(), position, position + rows - 1);
	bool success = sql->removeRows(position, rows);
	endRemoveRows();

	select();
	return success;
}

QStringList ChannelModel::mimeTypes() const
{
	return QStringList() << "application/json";
}

QMimeData *ChannelModel::mimeData(const QModelIndexList &indexes) const
{
	auto *mimeData = new QMimeData;
	nlohmann::json array = nlohmann::json::array();

	for (const QModelIndex &index : indexes)
	{
		if (index.isValid())
		{
			array.push_back(GetChannel(index.row()));
		}
	}

	mimeData->setData("application/json", QByteArray::fromStdString(array.dump()));

	Logger::Log(info, "MimeData: {}", array.dump());
	return mimeData;
}

RadioChannel ChannelModel::GetChannel(int row) const
{
	NRSC5::Station station;
	TunerOptions tuner_data;

	QSqlRecord record = sql->record(row);

	tuner_data.modulation_type = static_cast<Modulation::Type>(record.value(kChannelTypeRow).toInt());
	tuner_data.frequency_ = record.value(kChannelFrequencyRow).toLongLong();

	station.name = record.value(kChannelCallSignRow).toString().toStdString();
	station.current_program = record.value(kChannelProgramIdRow).toUInt();

	return {tuner_data, station};
}

QSqlRecord ChannelModel::CreateRecord(int row, const RadioChannel &channel) const
{
	QSqlRecord record = sql->record(row);

	record.setValue(kChannelTypeRow, channel.tuner_options.modulation_type);
	record.setValue(kChannelFrequencyRow, channel.tuner_options.frequency_);
	record.setValue(kChannelProgramIdRow, channel.hd_station_.current_program);

	record.setGenerated(kChannelTypeRow, true);
	record.setGenerated(kChannelFrequencyRow, true);
	record.setGenerated(kChannelProgramIdRow, true);

	if (!channel.hd_station_.name.empty())
	{
		record.setValue(kChannelCallSignRow, QString::fromStdString(channel.hd_station_.name));
		record.setGenerated(kChannelCallSignRow, true);
	}
	return record;
}

bool ChannelModel::submit()
{
	bool success = sql->submit();

	if (sql->lastError().isValid() || !success)
	{
		sql->revertAll();
		Logger::Log(info, "SQL Error: {}", sql->lastError().text());
	}
	return success;
}

void ChannelModel::revert()
{
	return sql->revert();
}

bool ChannelModel::select()
{
	beginResetModel();
	bool success = sql->select();
	endResetModel();

	return success;
}

