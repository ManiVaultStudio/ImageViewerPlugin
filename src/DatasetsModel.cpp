#include "DatasetsModel.h"
#include "LayersModel.h"

#include "ImageData/Images.h"

#include <QItemSelectionModel>
#include <QDebug>

DatasetsModel::DatasetsModel(QObject* parent) :
	QAbstractListModel(parent),
	_selectionModel(new QItemSelectionModel(this))
{
}

DatasetsModel::~DatasetsModel() = default;

int DatasetsModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return datasets().size();
}

int DatasetsModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return 19;
}

QVariant DatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= columnCount(index) || index.row() < 0)
		return QVariant();

	auto dataset = _datasets.at(index.row());

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case Columns::Name:
				return dataset->name(Qt::DisplayRole);

			case Columns::Type:
				return dataset->type(Qt::DisplayRole);

			case Columns::NoImages:
				return dataset->noImages(Qt::DisplayRole);

			case Columns::Size:
				return dataset->size(Qt::DisplayRole);

			case Columns::NoPoints:
				return dataset->noPoints(Qt::DisplayRole);

			case Columns::NoDimensions:
				return dataset->noDimensions(Qt::DisplayRole);

			case Columns::CurrentImage:
				return dataset->currentImage(Qt::DisplayRole);

			case Columns::CurrentImageName:
				return dataset->currentImageName(Qt::DisplayRole);

			case Columns::CurrentDimension:
				return dataset->currentDimension(Qt::DisplayRole);

			case Columns::CurrentDimensionName:
				return dataset->currentDimensionName(Qt::DisplayRole);

			case Columns::ImageNames:
				return dataset->imageNames(Qt::DisplayRole);

			case Columns::DimensionNames:
				return dataset->dimensionNames(Qt::DisplayRole);

			case Columns::Average:
				return dataset->_averageImages ? "true" : "false";

			case Columns::ImageFilePaths:
				return QString("[%1]").arg(dataset->_imageFilePaths.join(", "));

			case Columns::CurrentImageFilepath:
			{
				if (dataset->_currentImage < 0 || dataset->_currentImage >= dataset->_imageFilePaths.size())
					return "";

				return dataset->_imageFilePaths[dataset->_currentImage];
			}

			case Columns::CurrentDimensionFilepath:
			{
				if (dataset->_currentDimension < 0 || dataset->_currentDimension >= dataset->_imageFilePaths.size())
					return "";

				return dataset->_imageFilePaths[dataset->_currentDimension];
			}

			case Columns::PointsName:
				return dataset->_pointsName;

			case Columns::Selection:
			{
				auto selection = QStringList();

				if (dataset->_selection.size() <= 2) {
					for (const auto& id : dataset->_selection)
						selection << QString::number(id);
				}
				else {
					selection << QString::number(dataset->_selection.first());
					selection << "...";
					selection << QString::number(dataset->_selection.last());
				}

				return QString("[%1]").arg(selection.join(", "));
			}

			case Columns::SelectionSize:
				return QString::number(dataset->_selection.size());

			default:
				break;
		}
	}

	if (role == Qt::EditRole) {
		switch (index.column()) {
			case Columns::Name:
				return dataset->name(Qt::EditRole);

			case Columns::Type:
				return dataset->type(Qt::EditRole);

			case Columns::NoImages:
				return dataset->noImages(Qt::EditRole);

			case Columns::Size:
				return dataset->size(Qt::EditRole);

			case Columns::NoPoints:
				return dataset->noPoints(Qt::EditRole);

			case Columns::NoDimensions:
				return dataset->noDimensions(Qt::EditRole);

			case Columns::CurrentImage:
				return dataset->currentImage(Qt::EditRole);

			case Columns::CurrentImageName:
				return dataset->currentImageName(Qt::EditRole);

			case Columns::CurrentDimension:
				return dataset->currentDimension(Qt::EditRole);

			case Columns::CurrentDimensionName:
				return dataset->currentDimensionName(Qt::EditRole);

			case Columns::ImageNames:
				return dataset->imageNames(Qt::EditRole);

			case Columns::DimensionNames:
				return dataset->dimensionNames(Qt::EditRole);

			case Columns::Average:
				return dataset->_averageImages;

			case Columns::ImageFilePaths:
				return dataset->_imageFilePaths;

			case Columns::CurrentImageFilepath:
				if (dataset->_currentImage < 0 || dataset->_currentImage >= dataset->_imageFilePaths.size())
					return "";

				return dataset->_imageFilePaths[dataset->_currentImage];

			case Columns::CurrentDimensionFilepath:
				if (dataset->_currentDimension < 0 || dataset->_currentDimension >= dataset->_imageFilePaths.size())
					return "";

			case Columns::PointsName:
				return dataset->_pointsName;

			case Columns::Selection:
				return QVariant::fromValue(dataset->_selection);

			case Columns::SelectionSize:
				return dataset->_selection.size();

			default:
				break;
		}
	}
	
	if (role == Qt::ToolTipRole) {
		switch (index.column()) {
			case Columns::Name:
				return dataset->name(Qt::ToolTipRole);

			case Columns::Type:
				return dataset->type(Qt::ToolTipRole);

			case Columns::NoImages:
				return dataset->noImages(Qt::ToolTipRole);

			case Columns::Size:
				return dataset->size(Qt::ToolTipRole);

			case Columns::NoPoints:
				return dataset->noPoints(Qt::ToolTipRole);

			case Columns::NoDimensions:
				return dataset->noDimensions(Qt::ToolTipRole);

			case Columns::CurrentImage:
				return dataset->currentImage(Qt::ToolTipRole);

			case Columns::CurrentImageName:
				return dataset->currentImageName(Qt::ToolTipRole);

			case Columns::CurrentDimension:
				return dataset->currentDimension(Qt::ToolTipRole);

			case Columns::CurrentDimensionName:
				return dataset->currentDimensionName(Qt::ToolTipRole);

			case Columns::ImageNames:
				return dataset->imageNames(Qt::ToolTipRole);

			case Columns::DimensionNames:
				return dataset->dimensionNames(Qt::ToolTipRole);

			case Columns::Average:
				return QString("Average images: %1").arg(dataset->_averageImages ? "true" : "false");

			case Columns::ImageFilePaths:
				return QString("Image file paths: [%1]").arg(dataset->_imageFilePaths.join(", "));

			case Columns::CurrentImageFilepath:
			{
				if (dataset->_currentImage < 0 || dataset->_currentImage >= dataset->_imageFilePaths.size())
					return "";

				return dataset->_imageFilePaths[dataset->_currentImage];
			}

			case Columns::CurrentDimensionFilepath:
			{
				if (dataset->_currentDimension < 0 || dataset->_currentDimension >= dataset->_imageFilePaths.size())
					return "";
			}

			case Columns::PointsName:
			{
				return dataset->_pointsName;
			}

			case Columns::Selection:
			{
				auto selection = QStringList();

				for (const auto& id : dataset->_selection)
					selection << QString::number(id);

				return QString("Selected pixels: [%1]").arg(selection.join(", "));
			}

			case Columns::SelectionSize:
				return QString("Number of selected pixels: %1").arg(QString::number(dataset->_selection.size()));

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
			case Columns::Name:
				return "Name";

			case Columns::Type:
				return "Type";

			case Columns::NoImages:
				return "No. Images";

			case Columns::Size:
				return "Size";

			case Columns::NoPoints:
				return "No. points";

			case Columns::NoDimensions:
				return "No. dimensions";

			case Columns::CurrentImage:
				return "Current image ID";

			case Columns::CurrentImageName:
				return "Image name";

			case Columns::CurrentDimension:
				return "Current dimension ID";

			case Columns::CurrentDimensionName:
				return "Dimension name";

			case Columns::ImageNames:
				return "Images";

			case Columns::DimensionNames:
				return "Dimensions";

			case Columns::Average:
				return "Average";

			case Columns::PointsName:
				return "Points dataset name";

			case Columns::Selection:
				return "Selection";

			case Columns::SelectionSize:
				return "No. selected items";

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

	const auto datasetType	= data(index.row(), Columns::Type, Qt::EditRole);
	const auto average		= data(index.row(), Columns::Average, Qt::EditRole);

	switch (index.column()) {
		case Columns::Name:
		case Columns::Type:
		case Columns::NoImages:
		case Columns::Size:
		case Columns::NoPoints:
		case Columns::NoDimensions:
		case Columns::CurrentImageName:
		case Columns::CurrentDimensionName:
		case Columns::ImageNames:
		case Columns::DimensionNames:
			break;

		case Columns::CurrentImage:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Sequence) != average)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::CurrentDimension:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Stack))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Average:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Sequence))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::PointsName:
			break;

		case Columns::Selection:
			break;

		case Columns::SelectionSize:
			break;

		default:
			break;
	}
	
	return flags;
}

bool DatasetsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto dataset = datasets().value(row);

		switch (index.column()) {
			case Columns::Name:
				dataset->setName(value.toString());
				break;

			case Columns::Type:
				dataset->setType(value.toInt());
				break;

			case Columns::NoImages:
				break;

			case Columns::Size:
				dataset->setSize(value.toSize());
				break;

			case Columns::NoPoints:
				dataset->setNoPoints(value.toInt());
				break;

			case Columns::NoDimensions:
				dataset->setNoDimensions(value.toInt());
				break;

			case Columns::CurrentImage:
				dataset->setCurrentImage(value.toInt());
				break;

			case Columns::CurrentDimension:
				dataset->setCurrentDimension(value.toInt());
				break;

			case Columns::ImageNames:
				dataset->setImageNames(value.toStringList());
				break;

			case Columns::DimensionNames:
				dataset->setDimensionNames(value.toStringList());
				break;

			case Columns::Average:
				dataset->_averageImages = value.toBool();
				break;

			case Columns::ImageFilePaths:
				dataset->_imageFilePaths = value.toStringList();
				break;

			case Columns::PointsName:
				dataset->_pointsName = value.toString();
				break;

			case Columns::Selection:
				dataset->_selection = value.value<Indices>();
				break;

			case Columns::SelectionSize:
				break;

			default:
				return false;
		}

		switch (index.column())
		{
			case Columns::Average:
				emit dataChanged(this->index(row, Columns::ImageNames), this->index(row, Columns::ImageNames));
				break;

			case Columns::Selection:
				emit dataChanged(this->index(row, Columns::Selection), this->index(row, Columns::SelectionSize));
				emit dataChanged(this->index(row, Columns::ImageNames), this->index(row, Columns::ImageNames));
				break;

			default:
				break;
		}

		emit dataChanged(index, index);

		return true;
	}

	return false;
}

bool DatasetsModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		datasets().insert(position, new ImageDataset(this));
	}

	endInsertRows();

	return true;
}

bool DatasetsModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		datasets().removeAt(position);
	}

	endRemoveRows();

	return true;
}

const Datasets& DatasetsModel::datasets() const
{
	return _datasets;
}

Datasets& DatasetsModel::datasets()
{
	const auto constThis = const_cast<const DatasetsModel*>(this);
	return const_cast<Datasets&>(constThis->datasets());
}

QVariant DatasetsModel::data(const int& row, const int& column, int role /*= Qt::DisplayRole*/) const
{
	const auto modelIndex = index(row, column);

	if (!modelIndex.isValid())
		return QVariant();

	return data(index(row, column), role);
}

void DatasetsModel::setData(const int& row, const int& column, const QVariant& value)
{
	const auto modelIndex = index(row, column);

	if (!modelIndex.isValid())
		return;

	setData(modelIndex, value);
}

void DatasetsModel::add(ImageDataset* dataset)
{
	insertRows(0, 1);

	setData(index(0, Columns::Name, QModelIndex()), dataset->_name);
	setData(index(0, Columns::Type, QModelIndex()), dataset->_type);
	setData(index(0, Columns::Size, QModelIndex()), dataset->_size);
	setData(index(0, Columns::NoPoints, QModelIndex()), dataset->_noPoints);
	setData(index(0, Columns::NoDimensions, QModelIndex()), dataset->_noDimensions);
	setData(index(0, Columns::ImageNames, QModelIndex()), dataset->_imageNames);
	setData(index(0, Columns::DimensionNames, QModelIndex()), dataset->_dimensionNames);
	setData(index(0, Columns::Average, QModelIndex()), dataset->_averageImages);
	setData(index(0, Columns::ImageFilePaths, QModelIndex()), dataset->_imageFilePaths);
	setData(index(0, Columns::CurrentImage, QModelIndex()), dataset->_currentImage);
	setData(index(0, Columns::CurrentDimension, QModelIndex()), dataset->_currentDimension);
	setData(index(0, Columns::PointsName, QModelIndex()), dataset->_pointsName);
}

LayersModel* DatasetsModel::layersModel(const int& row)
{
	return _datasets[row]->_layersModel.get();
}