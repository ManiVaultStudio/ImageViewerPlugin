#include "DatasetsModel.h"

#include "ImageData/Images.h"

#include <QDebug>

DatasetsModel::DatasetsModel(MainModel* mainModel) :
	QAbstractListModel(mainModel),
	_mainModel(mainModel),
	_currentDatasetName()
{
}

DatasetsModel::~DatasetsModel() = default;

int DatasetsModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return datasets()->size();
}

int DatasetsModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return 11;
}

QVariant DatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= columnCount(index) || index.row() < 0)
		return QVariant();

	auto dataset = datasets()->at(index.row());

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case (static_cast<int>(Columns::Name)) :
				return dataset._name;

			case (static_cast<int>(Columns::Type)):
				return imageCollectionTypeName(static_cast<ImageCollectionType>(dataset._type));

			case (static_cast<int>(Columns::NoImages)):
				return QString::number(dataset._noImages);

			case (static_cast<int>(Columns::Size)):
				return QString("%1x%2").arg(QString::number(dataset._size.width()), QString::number(dataset._size.height()));

			case (static_cast<int>(Columns::NoPoints)):
				return QString::number(dataset._noPoints);

			case (static_cast<int>(Columns::NoDimensions)):
				return QString::number(dataset._noDimensions);

			case (static_cast<int>(Columns::CurrentImage)):
				return QString::number(dataset._currentImage);

			case (static_cast<int>(Columns::CurrentDimension)):
				return QString::number(dataset._currentDimension);

			case (static_cast<int>(Columns::ImageNames)):
				return QString("[%1]").arg(dataset._imageNames.join(", "));

			case (static_cast<int>(Columns::DimensionNames)):
				return QString("[%1]").arg(dataset._dimensionNames.join(", "));

			case (static_cast<int>(Columns::AverageImages)):
				return dataset._averageImages ? "true" : "false";

			default:
				break;
		}
	}

	/*
	if (role == Qt::ToolTipRole) {
		return "Tooltip!";
	}
	*/

	if (role == Qt::EditRole) {
		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				return dataset._name;

			case (static_cast<int>(Columns::Type)):
				return dataset._type;

			case (static_cast<int>(Columns::NoImages)):
				return QString::number(dataset._noImages);

			case (static_cast<int>(Columns::Size)):
				return dataset._size;

			case (static_cast<int>(Columns::NoPoints)):
				return dataset._noPoints;

			case (static_cast<int>(Columns::NoDimensions)):
				return dataset._noDimensions;

			case (static_cast<int>(Columns::CurrentImage)):
				return dataset._currentImage;

			case (static_cast<int>(Columns::CurrentDimension)):
				return dataset._currentDimension;

			case (static_cast<int>(Columns::ImageNames)):
				return dataset._imageNames;

			case (static_cast<int>(Columns::DimensionNames)):
				return dataset._dimensionNames;

			case (static_cast<int>(Columns::AverageImages)):
				return dataset._averageImages;

			default:
				break;
		}
	}

	return QVariant();
}

QVariant DatasetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case (static_cast<int>(Columns::Name)):
				return "Name";

			case (static_cast<int>(Columns::Type)):
				return "Type";

			case (static_cast<int>(Columns::NoImages)):
				return "No. images";

			case (static_cast<int>(Columns::Size)):
				return "Size";

			case (static_cast<int>(Columns::NoPoints)):
				return "No. Points";

			case (static_cast<int>(Columns::NoDimensions)):
				return "No. Dimensions";

			case (static_cast<int>(Columns::CurrentImage)):
				return "Image ID";

			case (static_cast<int>(Columns::CurrentDimension)):
				return "Dimension ID";

			case (static_cast<int>(Columns::ImageNames)):
				return "Image Names";

			case (static_cast<int>(Columns::DimensionNames)):
				return "Dimension Names";

			case (static_cast<int>(Columns::AverageImages)):
				return "Average Images";

			default:
				return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags DatasetsModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (index.column()) {
		case (static_cast<int>(Columns::Name)):
			break;

		case (static_cast<int>(Columns::Type)):
			break;

		case (static_cast<int>(Columns::NoImages)):
			break;

		case (static_cast<int>(Columns::Size)):
			break;

		case (static_cast<int>(Columns::NoPoints)):
			break;

		case (static_cast<int>(Columns::NoDimensions)):
			break;

		case (static_cast<int>(Columns::CurrentImage)):
		{
			if (type(index.row()) == static_cast<int>(ImageCollectionType::Sequence))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case (static_cast<int>(Columns::CurrentDimension)):
		{
			if (type(index.row()) == static_cast<int>(ImageCollectionType::Stack))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case (static_cast<int>(Columns::ImageNames)):
			break;

		case (static_cast<int>(Columns::DimensionNames)):
			break;

		case (static_cast<int>(Columns::AverageImages)):
		{
			if (type(index.row()) == static_cast<int>(ImageCollectionType::Sequence))
				flags |= Qt::ItemIsEditable;

			break;
		}

		default:
			break;
	}
	
	return flags;
}

bool DatasetsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto dataset = datasets()->value(row);

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				dataset._name = value.toString();
				break;

			case (static_cast<int>(Columns::Type)):
				dataset._type = value.toInt();
				break;

			case (static_cast<int>(Columns::NoImages)):
				dataset._noImages = value.toInt();
				break;

			case (static_cast<int>(Columns::Size)):
				dataset._size = value.toSize();
				break;

			case (static_cast<int>(Columns::NoPoints)):
				dataset._noPoints = value.toInt();
				break;

			case (static_cast<int>(Columns::NoDimensions)):
				dataset._noDimensions = value.toInt();
				break;

			case (static_cast<int>(Columns::CurrentImage)):
				dataset._currentImage = value.toInt();
				break;

			case (static_cast<int>(Columns::CurrentDimension)):
				dataset._currentDimension = value.toInt();
				break;

			case (static_cast<int>(Columns::ImageNames)):
				dataset._imageNames = value.toStringList();
				break;

			case (static_cast<int>(Columns::DimensionNames)):
				dataset._dimensionNames = value.toStringList();
				break;

			case (static_cast<int>(Columns::AverageImages)):
				dataset._averageImages = value.toBool();
				break;

			default:
				return false;
		}

		datasets()->replace(row, dataset);

		emit(dataChanged(index, index));

		return true;
	}

	return false;
}

bool DatasetsModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		datasets()->insert(position, MainModel::Dataset());
	}

	endInsertRows();

	return true;
}

bool DatasetsModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		datasets()->removeAt(position);
	}

	endRemoveRows();

	return true;
}

const Datasets* DatasetsModel::datasets() const
{
	return _mainModel->datasets();
}

Datasets* DatasetsModel::datasets()
{
	const auto constThis = const_cast<const DatasetsModel*>(this);
	return const_cast<Datasets*>(constThis->datasets());
}

int DatasetsModel::type(const std::uint32_t& row) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::Type)), Qt::EditRole).toInt();
}

int DatasetsModel::currentDimension(const std::uint32_t& row) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::CurrentDimension)), Qt::EditRole).toInt();
}

void DatasetsModel::setCurrentDimension(const std::uint32_t& row, const std::uint32_t& currentDimensionID)
{
	setData(index(row, static_cast<int>(DatasetsModel::Columns::CurrentDimension)), currentDimensionID, Qt::DisplayRole);
}

int DatasetsModel::currentImage(const std::uint32_t& row) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::CurrentImage)), Qt::EditRole).toInt();
}

void DatasetsModel::setCurrentImage(const std::uint32_t& row, const std::uint32_t& currentImageID)
{
	setData(index(row, static_cast<int>(DatasetsModel::Columns::CurrentImage)), currentImageID, Qt::DisplayRole);
}

QStringList DatasetsModel::imageNames(const std::uint32_t& row) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::ImageNames)), Qt::EditRole).toStringList();
}

QStringList DatasetsModel::dimensionNames(const std::uint32_t& row) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::DimensionNames)), Qt::EditRole).toStringList();
}

bool DatasetsModel::averageImages(const std::uint32_t& row) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::AverageImages)), Qt::EditRole).toBool();
}

void DatasetsModel::setAverageImages(const std::uint32_t& row, const bool& averageImages)
{
	setData(index(row, static_cast<int>(DatasetsModel::Columns::AverageImages)), averageImages, Qt::DisplayRole);
}

void DatasetsModel::add(const MainModel::Dataset& dataset)
{
	insertRows(0, 1, QModelIndex());

	setData(index(0, static_cast<int>(Columns::Name), QModelIndex()), dataset._name);
	setData(index(0, static_cast<int>(Columns::Type), QModelIndex()), dataset._type);
	setData(index(0, static_cast<int>(Columns::NoImages), QModelIndex()), dataset._noImages);
	setData(index(0, static_cast<int>(Columns::Size), QModelIndex()), dataset._size);
	setData(index(0, static_cast<int>(Columns::NoPoints), QModelIndex()), dataset._noPoints);
	setData(index(0, static_cast<int>(Columns::NoDimensions), QModelIndex()), dataset._noDimensions);
	setData(index(0, static_cast<int>(Columns::CurrentImage), QModelIndex()), dataset._currentImage);
	setData(index(0, static_cast<int>(Columns::CurrentDimension), QModelIndex()), dataset._currentDimension);
	setData(index(0, static_cast<int>(Columns::ImageNames), QModelIndex()), dataset._imageNames);
	setData(index(0, static_cast<int>(Columns::DimensionNames), QModelIndex()), dataset._dimensionNames);
	setData(index(0, static_cast<int>(Columns::AverageImages), QModelIndex()), dataset._averageImages);
}