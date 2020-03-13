#include "ImageDataset.h"
#include "LayersModel.h"

#include "ImageData/Images.h"

#include <QFont>
#include <QDebug>

ImageDataset::ImageDataset(QObject* parent) :
	QObject(parent),
	_name(""),
	_type(0),
	_size(),
	_noPoints(0),
	_noDimensions(0),
	_imageNames(),
	_imageFilePaths(),
	_average(),
	_pointsName(),
	_selection(Indices({ 0 })),
	_layers(),
	_layersModel(QSharedPointer<LayersModel>::create(this))
{
}

void ImageDataset::addLayer(const QString& id, const QString& name, const Layer::Type& type, const std::uint32_t& flags)
{
	_layers.append(new Layer(this, id, name, type, flags, _layers.size()));
}

QVariant ImageDataset::name(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _name;

		case Qt::ToolTipRole:
			return QString("Dataset name: %1").arg(_name);

		default:
			break;
	}

	return QString();
}

void ImageDataset::setName(const QString& name)
{
	_name = name;
}

QVariant ImageDataset::type(const int& role /*= Qt::DisplayRole*/) const
{
	const auto typeName = ImageData::typeName(static_cast<ImageData::Type>(_type));

	switch (role)
	{
		case Qt::FontRole:
			return QFont("Font Awesome 5 Free Solid", 9);

		case Qt::DisplayRole:
			return typeName;
		
		case Qt::EditRole:
			return static_cast<int>(_type);

		case Qt::ToolTipRole:
			return QString("Type: %1").arg(typeName);

		case Roles::FontIconText:
		{
			switch (_type) {
				case ImageData::Type::Sequence:
					return u8"\uf00a";

				case ImageData::Type::Stack:
					return u8"\uf5fd";

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QString();
}

void ImageDataset::setType(const int& type)
{
	_type = type;
}

QVariant ImageDataset::noImages(const int& role /*= Qt::DisplayRole*/) const
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

	return std::uint32_t{};
}

QVariant ImageDataset::size(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString("%1x%2").arg(QString::number(_size.width()), QString::number(_size.height()));

		case Qt::EditRole:
			return _size;

		case Qt::ToolTipRole:
			return QString("Image size: %1x%2").arg(QString::number(_size.width()), QString::number(_size.height()));

		default:
			break;
	}

	return QSize();
}

void ImageDataset::setSize(const QSize& size)
{
	_size = size;
}

QVariant ImageDataset::noPoints(const int& role /*= Qt::DisplayRole*/) const
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

	return std::uint32_t{};
}

void ImageDataset::setNoPoints(const std::uint32_t& noPoints)
{
	_noPoints = noPoints;
}

QVariant ImageDataset::noDimensions(const int& role /*= Qt::DisplayRole*/) const
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

	return std::uint32_t{};
}

void ImageDataset::setNoDimensions(const std::uint32_t& noDimensions)
{
	_noDimensions = noDimensions;
}

QVariant ImageDataset::imageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize	= _selection.size();
	const auto imageNamesString	= ImageDataset::displayStringList(_imageNames);

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

	return QStringList();
}

QVariant ImageDataset::filteredImageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize = _selection.size();

	auto imageNames = QStringList();

	switch (_type)
	{
		case ImageData::Type::Sequence:
		{
			if (selectionSize > 0) {
				for (auto id : _selection) {
					imageNames << _imageNames[id];
				}
			}
			else {
				imageNames << _imageNames;
			}

			break;
		}

		case ImageData::Type::Stack:
		{
			imageNames = _imageNames;
			break;
		}

		default:
			break;
	}

	auto imageNamesString = QString();

	if (_average) {
		imageNamesString = ImageDataset::displayStringList(imageNames);
	}
	else {
		imageNamesString = _imageNames[_currentImage];
	}

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return imageNames;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}

	return QStringList();
}

QVariant ImageDataset::imageIds(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize = _selection.size();

	auto ids = Indices();

	switch (_type)
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
				ids << _currentImage;
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
				ids << _currentImage;
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

	return QStringList();
}

void ImageDataset::setImageNames(const QStringList& imageNames)
{
	_imageNames = imageNames;
}

QVariant ImageDataset::imageFilePaths(const int& role /*= Qt::DisplayRole*/) const
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

	return QStringList();
}

void ImageDataset::setImageFilePaths(const QStringList& imageFilePaths)
{
	_imageFilePaths = imageFilePaths;
}

QVariant ImageDataset::currentImage(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_currentImage);

		case Qt::EditRole:
			return _currentImage;

		case Qt::ToolTipRole:
			return QString("Current image: %1").arg(QString::number(_currentImage));
	}

	return QString();
}

void ImageDataset::setCurrentImage(const std::uint32_t& currentImage)
{
	if (_selection.isEmpty())
		_currentImage = currentImage;
	else
		_currentImage = _selection[currentImage];
}

QVariant ImageDataset::currentImageName(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageName = _imageNames.isEmpty() ? "" : _imageNames[_currentImage];

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return imageName;

		case Qt::ToolTipRole:
			return QString("Current image: %1").arg(imageName);
	}

	return "";
}

void ImageDataset::setAverage(const bool& average)
{
	_average = average;
}

QVariant ImageDataset::average(const int& role /*= Qt::DisplayRole*/) const
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

	return false;
}

QVariant ImageDataset::currentImageFilePath(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageFilePathString = _imageFilePaths[_currentImage];

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

	return QString();
}

QVariant ImageDataset::pointsName(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void ImageDataset::setPointsName(const QString& pointsName)
{
	_pointsName = pointsName;
}

QVariant ImageDataset::selection(const int& role /*= Qt::DisplayRole*/) const
{
	auto selection = QStringList();

	if (_selection.size() <= 2) {
		for (const auto& id : _selection)
			selection << QString::number(id);
	}
	else {
		selection << QString::number(_selection.first());
		selection << "...";
		selection << QString::number(_selection.last());
	}

	const auto selectionString = QString("[%1]").arg(selection.join(", "));

	switch (role)
	{
		case Qt::DisplayRole:
			return selectionString;

		case Qt::EditRole:
			return QVariant::fromValue(_selection);

		case Qt::ToolTipRole:
			return QString("Selection: %1").arg(selectionString);

		default:
			break;
	}

	return QString();
}

void ImageDataset::setSelection(const Indices& selection)
{
	_selection		= selection;
	_currentImage	= selection.isEmpty() ? 0 : selection.first();
}

QVariant ImageDataset::selectionSize(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSizeString = QString::number(_selection.size());

	switch (role)
	{
		case Qt::DisplayRole:
			return selectionSizeString;

		case Qt::EditRole:
			return _selection.size();

		case Qt::ToolTipRole:
		{
			switch (_type)
			{
				case ImageData::Type::Sequence:
					return QString("No. selected images: %1").arg(selectionSizeString);

				case ImageData::Type::Stack:
					return QString("No. selected pixels: %1").arg(selectionSizeString);

				default:
					break;
			}
		}

		default:
			break;
	}

	return QString();
}

Layers& ImageDataset::layers()
{
	return _layers;
}

QSharedPointer<LayersModel> ImageDataset::layersModel()
{
	return _layersModel;
}

QString ImageDataset::displayStringList(const QStringList& stringList)
{
	const auto noStrings = stringList.size();

	if (noStrings == 1)
		return QString("%1").arg(stringList.first());

	if (noStrings == 2)
		return QString("[%1, %2]").arg(stringList.first(), stringList.last());

	if (noStrings > 2)
		return QString("[%1, ..., %2]").arg(stringList.first(), stringList.last());

	return QString("[]");
}