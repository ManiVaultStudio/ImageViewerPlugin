#include "DatasetsModel.h"
#include "LayersModel.h"

#include "ImageData/Images.h"

#include <QItemSelectionModel>
#include <QDebug>

DatasetsModel::DatasetsModel(MainModel* mainModel) :
	QAbstractListModel(mainModel),
	_mainModel(mainModel),
	_currentDatasetName(),
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

	return 14;
}

QVariant DatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= columnCount(index) || index.row() < 0)
		return QVariant();

	auto dataset = _mainModel->datasets().at(index.row());

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
				return dataset._imageNames.isEmpty() ? "" : dataset._imageNames[dataset._currentImage];

			case (static_cast<int>(Columns::CurrentDimension)):
				return dataset._dimensionNames.isEmpty() ? "" : dataset._dimensionNames[dataset._currentDimension];

			case (static_cast<int>(Columns::ImageNames)):
				return QString("[%1]").arg(dataset._imageNames.join(", "));

			case (static_cast<int>(Columns::DimensionNames)):
				return QString("[%1]").arg(dataset._dimensionNames.join(", "));

			case (static_cast<int>(Columns::AverageImages)):
				return dataset._averageImages ? "true" : "false";

			case (static_cast<int>(Columns::ImageFilePaths)):
				return QString("[%1]").arg(dataset._imageFilePaths.join(", "));

			case (static_cast<int>(Columns::CurrentImageFilepath)):
				return dataset._currentImage < dataset._imageFilePaths.size() ? dataset._imageFilePaths[dataset._currentImage] : "";

			case (static_cast<int>(Columns::CurrentDimensionFilepath)):
				return dataset._currentDimension < dataset._imageFilePaths.size() ? dataset._imageFilePaths[dataset._currentDimension] : "";

			default:
				break;
		}
	}

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
	
	if (role == Qt::ToolTipRole) {
		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				return QString("Dataset name: %1").arg(dataset._name);

			case (static_cast<int>(Columns::Type)):
				return QString("Dataset type: %1").arg(imageCollectionTypeName(static_cast<ImageCollectionType>(dataset._type)));

			case (static_cast<int>(Columns::NoImages)):
				return QString("Number of images: %1").arg(QString::number(dataset._noImages));

			case (static_cast<int>(Columns::Size)):
				return QString("Image resolution: %1x%2 pixels").arg(QString::number(dataset._size.width()), QString::number(dataset._size.height()));

			case (static_cast<int>(Columns::NoPoints)):
				return QString("Number of data points: %1").arg(QString::number(dataset._noPoints));

			case (static_cast<int>(Columns::NoDimensions)):
				return QString("Number of data dimensions: %1").arg(QString::number(dataset._noDimensions));

			case (static_cast<int>(Columns::CurrentImage)):
				return dataset._currentImage < dataset._imageNames.size() ? QString("Current image: %1").arg(dataset._imageNames[dataset._currentImage]) : "";

			case (static_cast<int>(Columns::CurrentDimension)):
				return dataset._currentDimension < dataset._dimensionNames.size() ? QString("Current dimension: %1").arg(dataset._dimensionNames[dataset._currentDimension]) : "";

			case (static_cast<int>(Columns::ImageNames)):
				return QString("Image names: [%1]").arg(dataset._imageNames.join(", "));

			case (static_cast<int>(Columns::DimensionNames)):
				return QString("Dimension names: [%1]").arg(dataset._dimensionNames.join(", "));

			case (static_cast<int>(Columns::AverageImages)):
				return QString("Average images: %1").arg(dataset._averageImages ? "true" : "false");

			case (static_cast<int>(Columns::ImageFilePaths)):
				return QString("Image file paths: [%1]").arg(dataset._imageFilePaths.join(", "));

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
				return "#Images";

			case (static_cast<int>(Columns::Size)):
				return "Size";

			case (static_cast<int>(Columns::NoPoints)):
				return "#Points";

			case (static_cast<int>(Columns::NoDimensions)):
				return "#Dimensions";

			case (static_cast<int>(Columns::CurrentImage)):
				return "Image";

			case (static_cast<int>(Columns::CurrentDimension)):
				return "Dimension";

			case (static_cast<int>(Columns::ImageNames)):
				return "Images";

			case (static_cast<int>(Columns::DimensionNames)):
				return "Dimensions";

			case (static_cast<int>(Columns::AverageImages)):
				return "Average";

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

		auto dataset = datasets().value(row);

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

			case (static_cast<int>(Columns::ImageFilePaths)):
				dataset._imageFilePaths = value.toStringList();
				break;

			default:
				return false;
		}

		datasets().replace(row, dataset);

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
		datasets().insert(position, ImageDataset());
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
	return _mainModel->datasets();
}

Datasets& DatasetsModel::datasets()
{
	const auto constThis = const_cast<const DatasetsModel*>(this);
	return const_cast<Datasets&>(constThis->datasets());
}

QVariant DatasetsModel::type(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::Type)), Qt::EditRole).toInt();
}

QVariant DatasetsModel::currentImage(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::CurrentImage)), Qt::EditRole).toInt();
}

QVariant DatasetsModel::currentDimension(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::CurrentDimension)), Qt::EditRole).toInt();
}

QVariant DatasetsModel::imageNames(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::ImageNames)), Qt::EditRole).toStringList();
}

QVariant DatasetsModel::dimensionNames(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::DimensionNames)), Qt::EditRole).toStringList();
}

QVariant DatasetsModel::averageImages(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::AverageImages)), Qt::EditRole).toBool();
}

QVariant DatasetsModel::currentImageFilepath(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::CurrentImageFilepath)), role);
}

QVariant DatasetsModel::currentDimensionFilepath(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(DatasetsModel::Columns::CurrentDimensionFilepath)), role);
}

QSharedPointer<LayersModel> DatasetsModel::layersModel(const int& row)
{
	if (row < 0 || row >= _mainModel->datasets().size())
		return QSharedPointer<LayersModel>();

	return QSharedPointer<LayersModel>::create(&_mainModel->datasets()[row]._layers);
}

void DatasetsModel::setCurrentImage(const int& row, const std::uint32_t& currentImageID)
{
	setData(index(row, static_cast<int>(DatasetsModel::Columns::CurrentImage)), currentImageID, Qt::DisplayRole);
}

void DatasetsModel::setCurrentDimension(const int& row, const std::uint32_t& currentDimensionID)
{
	setData(index(row, static_cast<int>(DatasetsModel::Columns::CurrentDimension)), currentDimensionID, Qt::DisplayRole);
}

void DatasetsModel::setAverageImages(const int& row, const bool& averageImages)
{
	setData(index(row, static_cast<int>(DatasetsModel::Columns::AverageImages)), averageImages, Qt::DisplayRole);
}

void DatasetsModel::add(const ImageDataset& dataset)
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
	setData(index(0, static_cast<int>(Columns::ImageFilePaths), QModelIndex()), dataset._imageFilePaths);
}