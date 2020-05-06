#include "ImagesLayer.h"
#include "ImageViewerPlugin.h"
#include "ImagesProp.h"

#include "PointData.h"

#include <QDebug>
#include <QFileInfo>

ImagesLayer::ImagesLayer(const QString& imagesDatasetName, const QString& id, const QString& name, const int& flags) :
	LayerNode(imagesDatasetName, ImagesLayer::Type::Images, id, name, flags),
	_images(nullptr),
	_imageDataType(ImageData::Type::Undefined),
	_noPoints(0),
	_noDimensions(0),
	_imageNames(),
	_imageFilePaths(),
	_pointsName(),
	_currentImageId(0),
	_average()
{
	init();
}

void ImagesLayer::init()
{
	addProp<ImagesProp>(this, "Images");

	_images = &imageViewerPlugin->requestData<Images>(_datasetName);

	Images& images = imageViewerPlugin->core()->requestData<Images>(_datasetName);

	_dataName = hdps::DataSet::getSourceData(*(images.points())).getDataName();

	setImageDataType(_images->type());
	setNoPoints(_images->points()->getNumPoints());
	setNoDimensions(_images->points()->getNumDimensions());

	auto imageFilePaths = QStringList();

	for (const auto& imageFilePath : _images->imageFilePaths()) {
		imageFilePaths << imageFilePath;
	}

	switch (_images->type())
	{
		case ImageData::Type::Sequence:
		{
			auto imageNames = QStringList();

			for (const auto& imageFilePath : _images->imageFilePaths()) {
				imageNames << QFileInfo(imageFilePath).fileName();
			}

			setImageNames(imageNames);
			break;
		}

		case ImageData::Type::Stack:
		{
			auto dimensionNames = QStringList();

			for (const auto& dimensionName : _images->dimensionNames()) {
				dimensionNames << dimensionName;
			}

			setImageNames(dimensionNames);
			break;
		}

		default:
			break;
	}

	setImageFilePaths(imageFilePaths);
	setPointsName(_images->points()->getDataName());

	auto selection = dynamic_cast<Points*>(&imageViewerPlugin->core()->requestSelection(_dataName));
	
	if (selection)
		setSelection(Indices::fromStdVector(selection->indices));

	setCurrentImageId(0);
}

void ImagesLayer::paint(QPainter* painter)
{
}

Qt::ItemFlags ImagesLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::NoImages:
			break;
		
		case Column::WindowNormalized:
		case Column::LevelNormalized:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::NoPoints:
		case Column::NoDimensions:
		case Column::ImageNames:
		case Column::FilteredImageNames:
		case Column::ImageIDs:
		case Column::ImageFilePaths:
		case Column::CurrentImageId:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::CurrentImageName:
		case Column::CurrentImageFilePath:
			break;

		case Column::Average:
		{
			if (imageDataType(Qt::EditRole).toInt() == ult(ImageData::Type::Sequence))
				flags |= Qt::ItemIsEditable;

			break;
		}

		default:
			break;
	}

	return flags;
}

QVariant ImagesLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::NoImages:
			return noImages(role);

		case Column::WindowNormalized:
			return propByName<ImagesProp>("Images")->image().windowNormalized(role);

		case Column::LevelNormalized:
			return propByName<ImagesProp>("Images")->image().levelNormalized(role);

		case Column::NoPoints:
			return noPoints(role);

		case Column::NoDimensions:
			return noDimensions(role);

		case Column::ImageNames:
			return imageNames(role);

		case Column::FilteredImageNames:
			return filteredImageNames(role);

		case Column::ImageIDs:
			return imageIds(role);

		case Column::ImageFilePaths:
			return imageFilePaths(role);

		case Column::CurrentImageId:
			return currentImageId(role);

		case Column::CurrentImageName:
			return currentImageName(role);

		case Column::CurrentImageFilePath:
			return currentImageFilePath(role);

		case Column::Average:
			return average(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList ImagesLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

	if (index.column() == ult(LayerNode::Column::Selection)) {
		computeImage();
		setCurrentImageId(0);

		affectedIds << index.siblingAtColumn(ult(Column::CurrentImageId));
		affectedIds << index.siblingAtColumn(ult(Column::FilteredImageNames));
		affectedIds << index.siblingAtColumn(ult(Column::ImageIDs));
	}

	switch (static_cast<Column>(index.column())) {
		case Column::NoImages:
			break;

		case Column::WindowNormalized:
			propByName<ImagesProp>("Images")->image().setWindowNormalized(value.toFloat());
			break;

		case Column::LevelNormalized:
			propByName<ImagesProp>("Images")->image().setLevelNormalized(value.toFloat());
			break;

		case Column::NoPoints:
		case Column::NoDimensions:
		case Column::ImageNames:
		case Column::FilteredImageNames:
		case Column::ImageIDs:
		case Column::ImageFilePaths:
			break;

		case Column::CurrentImageId:
		{
			setCurrentImageId(value.toInt());
			break;
		}

		case Column::CurrentImageName:
		case Column::CurrentImageFilePath:
			break;

		case Column::Average:
		{
			setAverage(value.toBool());
			affectedIds << index.siblingAtColumn(ult(Column::FilteredImageNames));
			break;
		}

		default:
			break;
	}

	return affectedIds;
}

QVariant ImagesLayer::imageDataType(const int& role) const
{
	const auto imageDataTypeString = ImageData::typeName(_imageDataType);

	switch (role)
	{
		case Qt::DisplayRole:
			return imageDataTypeString;

		case Qt::EditRole:
			return _imageDataType;

		case Qt::ToolTipRole:
			return QString("Image data type: %1").arg(imageDataTypeString);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setImageDataType(const ImageData::Type& imageDataType)
{
	_imageDataType = imageDataType;
}

QVariant ImagesLayer::noImages(const int& role) const
{
	const auto count = _imageFilePaths.size();

	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(count);

		case Qt::EditRole:
			return count;

		case Qt::ToolTipRole:
			return QString("Number of images: %1").arg(QString::number(count));

		default:
			break;
	}

	return QVariant();
}

QSize ImagesLayer::imageSize() const
{
	return _images->imageSize();
}

QVariant ImagesLayer::noPoints(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_noPoints);

		case Qt::EditRole:
			return _noPoints;

		case Qt::ToolTipRole:
			return QString("Number of data points: %1").arg(QString::number(_noPoints));

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setNoPoints(const std::uint32_t& noPoints)
{
	_noPoints = noPoints;
}

QVariant ImagesLayer::noDimensions(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_noDimensions);

		case Qt::EditRole:
			return _noDimensions;

		case Qt::ToolTipRole:
			return QString("Number of data dimensions: %1").arg(QString::number(_noDimensions));

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setNoDimensions(const std::uint32_t& noDimensions)
{
	_noDimensions = noDimensions;
}

QVariant ImagesLayer::imageNames(const int& role) const
{
	const auto imageNamesString = abbreviatedStringList(_imageNames);

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return _imageNames;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}

	return QVariant();
}

QVariant ImagesLayer::imageIds(const int& role) const
{
	const auto selectionSize = _selection.size();

	auto ids = Indices();

	switch (_imageDataType)
	{
		case ImageData::Type::Sequence:
		{
			if (_average) {
				if (selectionSize > 0) {
					for (auto id : _selection) {
						ids << id;
					}
				}
				else {
					ids.resize(_imageNames.size());
					std::iota(ids.begin(), ids.end(), 0);
				}
			}
			else {
				ids << _currentImageId;
			}

			break;
		}

		case ImageData::Type::Stack:
		{
			if (_average) {
				ids.resize(_imageNames.size());
				std::iota(ids.begin(), ids.end(), 0);
			}
			else {
				ids << _currentImageId;
			}

			break;
		}

		default:
			break;
	}
	
	auto imageIdsString = QString();

	if (ids.size() == 1)
		imageIdsString = QString::number(ids.first());

	if (ids.size() == 2)
		imageIdsString = QString("[%1, %2]").arg(QString::number(ids.first()), QString::number(ids.last()));

	if (ids.size() > 2)
		imageIdsString = QString("[%1, ..., %2]").arg(QString::number(ids.first()), QString::number(ids.last()));

	switch (role)
	{
		case Qt::DisplayRole:
			return imageIdsString;

		case Qt::EditRole:
			return QVariant::fromValue(ids);

		case Qt::ToolTipRole:
			return QString("Image ID's: %1").arg(imageIdsString);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setImageNames(const QStringList& imageNames)
{
	_imageNames = imageNames;
}

QVariant ImagesLayer::imageFilePaths(const int& role) const
{
	const auto imageFilePathsString = QString("[%1]").arg(_imageFilePaths.join(", "));

	switch (role)
	{
		case Qt::DisplayRole:
			return imageFilePathsString;

		case Qt::EditRole:
			return _imageFilePaths;

		case Qt::ToolTipRole:
			return QString("Image file paths: %1").arg(imageFilePathsString);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setImageFilePaths(const QStringList& imageFilePaths)
{
	_imageFilePaths = imageFilePaths;
}

QVariant ImagesLayer::pointsName(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _pointsName;

		case Qt::ToolTipRole:
			return QString("Points dataset name: %1").arg(_pointsName);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setPointsName(const QString& pointsName)
{
	_pointsName = pointsName;
}

QVariant ImagesLayer::filteredImageNames(const int& role) const
{
	const auto selectionSize	= this->selectionSize(Qt::EditRole).toInt();
	const auto imageNames		= this->imageNames(Qt::EditRole).toStringList();

	auto filtered = QStringList();

	switch (imageDataType(Qt::EditRole).toInt())
	{
		case ImageData::Type::Sequence:
		{
			if (selectionSize > 0) {
				for (auto id : _selection) {
					filtered << imageNames[id];
				}
			}
			else {
				filtered << imageNames;
			}

			break;
		}

		case ImageData::Type::Stack:
		{
			filtered = imageNames;
			break;
		}

		default:
			break;
	}

	auto imageNamesString = QString();

	if (_average) {
		imageNamesString = abbreviatedStringList(filtered);
	}
	else {
		imageNamesString = imageNames.isEmpty() ? "" : imageNames[_currentImageId];
	}

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return filtered;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}
	
	return QVariant();
}

QVariant ImagesLayer::currentImageId(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_currentImageId);

		case Qt::EditRole:
			return _currentImageId;

		case Qt::ToolTipRole:
			return QString("Current image: %1").arg(QString::number(_currentImageId));
	}

	return QVariant();
}

QVariant ImagesLayer::currentImageName(const int& role) const
{
	const auto imageNames	= this->imageNames(Qt::EditRole).toStringList();
	const auto imageName	= imageNames.isEmpty() ? "" : imageNames[_currentImageId];

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return imageName;

		case Qt::ToolTipRole:
			return QString("Current image: %1").arg(imageName);
	}

	return QVariant();
}

QVariant ImagesLayer::currentImageFilePath(const int& role) const
{
	const auto imageFilePathString = imageFilePaths(Qt::EditRole).toStringList()[_currentImageId];

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return imageFilePathString;

		case Qt::ToolTipRole:
			return QString("Image file path: %1").arg(imageFilePathString);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setCurrentImageId(const std::uint32_t& currentImageId)
{
	switch (static_cast<ImageData::Type>(imageDataType(Qt::EditRole).toInt()))
	{
		case ImageData::Type::Sequence:
		{
			if (_selection.isEmpty())
				_currentImageId = currentImageId;
			else
				_currentImageId = _selection[currentImageId];

			break;
		}

		case ImageData::Type::Stack:
			_currentImageId = currentImageId;
			break;

		default:
			break;
	}

	computeImage();
}

QVariant ImagesLayer::average(const int& role) const
{
	const auto averageString = _average ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return averageString;

		case Qt::EditRole:
			return _average;

		case Qt::ToolTipRole:
			return QString("Average images: %1").arg(averageString);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setAverage(const bool& average)
{
	_average = average;

	computeImage();
}

void ImagesLayer::mousePressEvent(QMouseEvent* mouseEvent)
{
}

void ImagesLayer::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
}

void ImagesLayer::mouseMoveEvent(QMouseEvent* mouseEvent)
{
}

void ImagesLayer::mouseWheelEvent(QWheelEvent* wheelEvent, const QModelIndex& index)
{
}

void ImagesLayer::keyPressEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
}

void ImagesLayer::keyReleaseEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
}

void ImagesLayer::computeImage()
{
	const auto imageIds = this->imageIds(Qt::EditRole).value<Indices>();

	switch (static_cast<ImageData::Type>(imageDataType(Qt::EditRole).toInt()))
	{
		case ImageData::Type::Sequence:
			propByName<ImagesProp>("Images")->setImage(_images->sequenceImage(imageIds.toStdVector()));
			break;

		case ImageData::Type::Stack:
			propByName<ImagesProp>("Images")->setImage(_images->stackImage(imageIds.first()));
			break;

		default:
			break;
	}
}