//
// Created by TheDaChicken on 7/18/2024.
//

#include "FavoriteModel.h"
#include "utils/Log.h"

#include <QMimeData>

#include <nlohmann/json.hpp>
#include <utils/StylingText.h>

constexpr auto kSettingKey = "favorite_channels";

FavoriteModel::FavoriteModel(
	SQLite::Database &db,
	const StationImageProvider &image_provider,
	QObject *parent)
	: QAbstractListModel(parent),
	  db_(db),
	  image_provider_(image_provider)
{
}

bool FavoriteModel::update()
{
	beginResetModel();

	channels_.clear();

	bool result = true;
	std::string value;

	if (UTILS::StatusCodes ret = db_.GetSettingValue(kSettingKey, value);
		ret == UTILS::StatusCodes::Ok)
	{
		const nlohmann::json array = nlohmann::json::parse(value);
		channels_ = array.get<std::vector<Channel> >();
	}
	else if (ret != UTILS::StatusCodes::Empty)
	{
		result = false;
		Logger::Log(err, "Failed to get favorite channels: {}", static_cast<int>(ret));
	}

	endResetModel();
	return result;
}

bool FavoriteModel::Set(const int row, const Channel &channel)
{
	channels_[row] = channel;

	emit dataChanged(
		createIndex(row, 0),
		createIndex(row, 0),
		{Qt::DisplayRole, Qt::DecorationRole});
	return submit();
}

bool FavoriteModel::Add(const Channel &channel)
{
	const int row = rowCount();

	beginInsertRows(QModelIndex(), row, row);
	channels_.push_back(channel);
	endInsertRows();

	return submit();
}

bool FavoriteModel::Extend(const int index, const std::vector<Channel> &channels)
{
	beginInsertRows(QModelIndex(), index, static_cast<int>(index + channels.size() - 1));
	channels_.insert(channels_.begin() + index, channels.begin(), channels.end());
	endInsertRows();

	return submit();
}

bool FavoriteModel::Remove(const Channel &channel)
{
	const QModelIndex index = Find(channel);
	if (!index.isValid())
		return false;

	return removeRows(index.row(), 1, index.parent());
}

const Channel &FavoriteModel::Get(const int row) const
{
	return channels_[row];
}

QModelIndex FavoriteModel::Find(const Channel &channel) const
{
	if (const auto &it =
				std::find_if(channels_.begin(),
				             channels_.end(),
				             [channel](const Channel &c)
				             {
					             return c.tuner_opts.freq == channel.tuner_opts.freq
							             && channel.station_info.current_program == c.station_info.current_program;
				             });
		it != channels_.end())
	{
		return createIndex(static_cast<int>(std::distance(channels_.begin(), it)), 0);
	}
	return {};
}

QVariant FavoriteModel::headerData(int column, Qt::Orientation orientation, int role) const
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

QVariant FavoriteModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return {};

	const Channel &channel = channels_[index.row()];

	switch (role)
	{
		case Qt::DisplayRole:
		{
			return StylingText::GetDisplayChannel(channel);
		}
		case Qt::DecorationRole:
		{
			return image_provider_.FetchStationImage(channel).image;
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

int FavoriteModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return channels_.size();
}

bool FavoriteModel::removeRows(const int position, const int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent)

	beginRemoveRows(QModelIndex(), position, position + rows - 1);
	channels_.erase(channels_.begin() + position, channels_.begin() + position + rows);
	endRemoveRows();

	return submit();
}

bool FavoriteModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
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
	if (fromRow < destinationChild)
		std::rotate(
			channels_.begin() + fromRow,
			channels_.begin() + fromRow + 1,
			channels_.begin() + destinationChild + 1);
	else
		std::rotate(
			channels_.begin() + destinationChild,
			channels_.begin() + fromRow,
			channels_.begin() + fromRow + 1);

	endMoveRows();
	return submit();
}

bool FavoriteModel::dropMimeData(const QMimeData *data, Qt::DropAction action, const int row, int column,
                                 const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/json"))
		return false;

	const QByteArray byteArray = data->data("application/json");
	const nlohmann::json array = nlohmann::json::parse(byteArray.toStdString());

	int beginRow;

	if (row != -1)
		beginRow = row;
	else if (parent.isValid())
		beginRow = parent.row();
	else
		beginRow = rowCount(QModelIndex());

	if (array.is_array())
		Extend(beginRow, array.get<std::vector<Channel> >());
	else
		return false;

	return true;
}

QStringList FavoriteModel::mimeTypes() const
{
	return QStringList() << "application/json";
}

QMimeData *FavoriteModel::mimeData(const QModelIndexList &indexes) const
{
	auto *mimeData = new QMimeData;
	nlohmann::json array = nlohmann::json::array();

	for (const QModelIndex &index : indexes)
	{
		if (!index.isValid())
			continue;

		array.push_back(Get(index.row()));
	}

	mimeData->setData("application/json",
	                  QByteArray::fromStdString(array.dump()));
	return mimeData;
}

Qt::ItemFlags FavoriteModel::flags(const QModelIndex &index) const
{
	if (index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	else
		return Qt::ItemIsDropEnabled;
}

Qt::DropActions FavoriteModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

bool FavoriteModel::submit()
{
	const auto array = nlohmann::json(channels_);
	const std::string value = array.dump();

	if (UTILS::StatusCodes ret = db_.SetSettingValue(kSettingKey, value);
		ret != UTILS::StatusCodes::Ok)
	{
		Logger::Log(err, "Failed to save favorite channels: {}", static_cast<int>(ret));
		return false;
	}

	return true;
}

