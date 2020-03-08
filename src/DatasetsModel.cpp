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

int DatasetsModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return datasets().size();
}

int DatasetsModel::columnCount(const QModelIndex& parent) const
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
				return dataset->_name;

			case Columns::Type:
				return imageCollectionTypeName(static_cast<ImageCollectionType>(dataset->_type));

			case Columns::NoImages:
				return QString::number(dataset->_noImages);

			case Columns::Size:
				return QString("%1x%2").arg(QString::number(dataset->_size.width()), QString::number(dataset->_size.height()));

			case Columns::NoPoints:
				return QString::number(dataset->_noPoints);

			case Columns::NoDimensions:
				return QString::number(dataset->_noDimensions);

			case Columns::CurrentImage:
				return QString::number(dataset->_currentImage);

			case Columns::CurrentImageName:
				return dataset->_imageNames.isEmpty() ? "" : dataset->_imageNames[dataset->_currentImage];

			case Columns::CurrentDimension:
				return QString::number(dataset->_currentDimension);

			case Columns::CurrentDimensionName:
				return dataset->_dimensionNames.isEmpty() ? "" : dataset->_dimensionNames[dataset->_currentDimension];

			case Columns::ImageNames:
				return QString("[%1]").arg(dataset->_imageNames.join(", "));

			case Columns::DimensionNames:
				return QString("[%1]").arg(dataset->_dimensionNames.join(", "));

			case Columns::AverageImages:
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
				return dataset->_name;

			case Columns::Type:
				return dataset->_type;

			case Columns::NoImages:
				return dataset->_noImages;

			case Columns::Size:
				return dataset->_size.height();

			case Columns::NoPoints:
				return dataset->_noPoints;

			case Columns::NoDimensions:
				return dataset->_noDimensions;

			case Columns::CurrentImage:
				return dataset->_currentImage;

			case Columns::CurrentImageName:
				return dataset->_imageNames.isEmpty() ? "" : dataset->_imageNames[dataset->_currentImage];

			case Columns::CurrentDimension:
				return dataset->_currentDimension;

			case Columns::CurrentDimensionName:
				return dataset->_dimensionNames.isEmpty() ? "" : dataset->_dimensionNames[dataset->_currentDimension];

			case Columns::ImageNames:
				return dataset->_imageNames;

			case Columns::DimensionNames:
				return dataset->_dimensionNames;

			case Columns::AverageImages:
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
				return QString("Dataset name: %1").arg(dataset->_name);

			case Columns::Type:
				return QString("Dataset type: %1").arg(imageCollectionTypeName(static_cast<ImageCollectionType>(dataset->_type)));

			case Columns::NoImages:
				return QString("Number of images: %1").arg(QString::number(dataset->_noImages));

			case Columns::Size:
				return QString("Image resolution: %1x%2 pixels").arg(QString::number(dataset->_size.width()), QString::number(dataset->_size.height()));

			case Columns::NoPoints:
				return QString("Number of data points: %1").arg(QString::number(dataset->_noPoints));

			case Columns::NoDimensions:
				return QString("Number of data dimensions: %1").arg(QString::number(dataset->_noDimensions));

			case Columns::CurrentImage:
				return dataset->_currentImage < dataset->_imageNames.size() ? QString("Current image: %1").arg(dataset->_imageNames[dataset->_currentImage]) : "";

			case Columns::CurrentImageName:
				return dataset->_currentImage < dataset->_imageNames.size() ? QString("Current image: %1").arg(dataset->_imageNames[dataset->_currentImage]) : "";

			case Columns::CurrentDimension:
				return dataset->_currentDimension < dataset->_dimensionNames.size() ? QString("Current dimension: %1").arg(dataset->_dimensionNames[dataset->_currentDimension]) : "";

			case Columns::CurrentDimensionName:
				return dataset->_currentDimension < dataset->_dimensionNames.size() ? QString("Current dimension: %1").arg(dataset->_dimensionNames[dataset->_currentDimension]) : "";

			case Columns::ImageNames:
				return QString("Image names: [%1]").arg(dataset->_imageNames.join(", "));

			case Columns::DimensionNames:
				return QString("Dimension names: [%1]").arg(dataset->_dimensionNames.join(", "));

			case Columns::AverageImages:
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
				return "#Images";

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

			case Columns::AverageImages:
				return "Average";

			case Columns::PointsName:
				return "Points dataset name";

			case Columns::Selection:
				return "Selection";

			case Columns::SelectionSize:
				return "No. selected pixels";

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

	const auto datasetType = data(index.row(), Columns::Type, Qt::EditRole);

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
			if (datasetType == static_cast<int>(ImageCollectionType::Sequence))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::CurrentDimension:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Stack))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::AverageImages:
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
				dataset->_name = value.toString();
				break;

			case Columns::Type:
				dataset->_type = value.toInt();
				break;

			case Columns::NoImages:
				dataset->_noImages = value.toInt();
				break;

			case Columns::Size:
				dataset->_size = value.toSize();
				break;

			case Columns::NoPoints:
				dataset->_noPoints = value.toInt();
				break;

			case Columns::NoDimensions:
				dataset->_noDimensions = value.toInt();
				break;

			case Columns::CurrentImage:
				dataset->_currentImage = value.toInt();
				break;

			case Columns::CurrentDimension:
				dataset->_currentDimension = value.toInt();
				break;

			case Columns::ImageNames:
				dataset->_imageNames = value.toStringList();
				break;

			case Columns::DimensionNames:
				dataset->_dimensionNames = value.toStringList();
				break;

			case Columns::AverageImages:
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
			case Columns::Selection:
				emit dataChanged(this->index(row, Columns::Selection), this->index(row, Columns::SelectionSize));
				break;

			default:
				emit dataChanged(index, index);
		}

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
	setData(index(0, Columns::NoImages, QModelIndex()), dataset->_noImages);
	setData(index(0, Columns::Size, QModelIndex()), dataset->_size);
	setData(index(0, Columns::NoPoints, QModelIndex()), dataset->_noPoints);
	setData(index(0, Columns::NoDimensions, QModelIndex()), dataset->_noDimensions);
	setData(index(0, Columns::ImageNames, QModelIndex()), dataset->_imageNames);
	setData(index(0, Columns::DimensionNames, QModelIndex()), dataset->_dimensionNames);
	setData(index(0, Columns::AverageImages, QModelIndex()), dataset->_averageImages);
	setData(index(0, Columns::ImageFilePaths, QModelIndex()), dataset->_imageFilePaths);
	setData(index(0, Columns::CurrentImage, QModelIndex()), dataset->_currentImage);
	setData(index(0, Columns::CurrentDimension, QModelIndex()), dataset->_currentDimension);
	setData(index(0, Columns::PointsName, QModelIndex()), dataset->_pointsName);
}

LayersModel* DatasetsModel::layersModel(const int& row)
{
	return _datasets[row]->_layersModel.get();
}