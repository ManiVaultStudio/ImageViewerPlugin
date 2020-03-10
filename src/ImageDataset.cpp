#include "ImageDataset.h"
#include "LayersModel.h"

#include "ImageData/Images.h"

#include <QDebug>

ImageDataset::ImageDataset(QObject* parent) :
	QObject(parent),
	_name(""),
	_type(0),
	_size(),
	_noPoints(0),
	_noDimensions(0),
	_currentImage(-1),
	_currentDimension(-1),
	_imageNames(),
	_dimensionNames(),
	_average(),
	_imageFilePaths(),
	_pointsName(),
	_selection(),
	_layers(),
	_layersModel(QSharedPointer<LayersModel>::create(&_layers))
{
	addLayer("Image", Layer::Type::Image, Layer::Flags::Enabled | Layer::Flags::Fixed);
	addLayer("Selection", Layer::Type::Selection, Layer::Flags::Enabled | Layer::Flags::Fixed);
	addLayer("MetaDataA", Layer::Type::Metadata, Layer::Flags::Enabled | Layer::Flags::Removable | Layer::Flags::Renamable);
	addLayer("MetaDataB", Layer::Type::Metadata, Layer::Flags::Enabled | Layer::Flags::Removable | Layer::Flags::Renamable);
	addLayer("MetaDataC", Layer::Type::Metadata, Layer::Flags::Enabled | Layer::Flags::Removable | Layer::Flags::Renamable);
}

void ImageDataset::addLayer(const QString& name, const Layer::Type& type, const std::uint32_t& flags)
{
	_layers.append(new Layer(this, name, type, flags, _layers.size()));
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
	switch (role)
	{
		case Qt::DisplayRole:
			return imageCollectionTypeName(static_cast<ImageCollectionType>(_type));

		case Qt::EditRole:
			return _type;

		case Qt::ToolTipRole:
			return QString("Dataset type: %1").arg(imageCollectionTypeName(static_cast<ImageCollectionType>(_type)));

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

	return std::uint32_t{};
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

void ImageDataset::setCurrentImage(const std::uint32_t& currentImage)
{
	if (_selection.isEmpty())
		_currentImage = currentImage;
	else
		_currentImage = _selection[currentImage];
}

QVariant ImageDataset::currentDimension(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_currentDimension);

		case Qt::EditRole:
			return _currentDimension;

		case Qt::ToolTipRole:
			return QString("Current dimension: %1").arg(QString::number(_currentDimension));
	}

	return std::uint32_t{};
}

QVariant ImageDataset::currentDimensionName(const int& role /*= Qt::DisplayRole*/) const
{
	const auto dimensionName = _dimensionNames.isEmpty() ? "" : _dimensionNames[_currentDimension];

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return dimensionName;

		case Qt::ToolTipRole:
			return QString("Current dimension: %1").arg(dimensionName);
	}

	return QString();
}

void ImageDataset::setCurrentDimension(const std::uint32_t& currentDimension)
{
	_currentDimension = currentDimension;
}

QVariant ImageDataset::imageNames(const int& role /*= Qt::DisplayRole*/) const
{
	auto names = QStringList();

	switch (_type)
	{
		case (static_cast<int>(ImageCollectionType::Sequence)):
		{
			const auto selectionSize	= _selection.size();
			const auto noImages			= _imageNames.size();

			if (_average) {
				if (selectionSize == 0) {
					if (noImages == 1)
						names << _imageNames.first();

					if (noImages == 2)
						names << QString("[%1, %2]").arg(_imageNames.first(), _imageNames.last());

					if (noImages > 2)
						names << QString("[%1, ..., %2]").arg(_imageNames.first(), _imageNames.last());
				}

				if (selectionSize == 1)
					names << _imageNames[_selection.first()];

				if (selectionSize == 2)
					names << QString("[%1, %2]").arg(_imageNames[_selection.first()], _imageNames[_selection.last()]);

				if (selectionSize > 2)
					names << QString("[%1, ..., %2]").arg(_imageNames[_selection.first()], _imageNames[_selection.last()]);
			}
			else {
				if (selectionSize <= 0) {
					return _imageNames;
				}
				else {
					for (auto selectionIndex : _selection) {
						names << _imageNames[selectionIndex];
					}

					return names;
				}
			}

			break;
		}

		case (static_cast<int>(ImageCollectionType::Stack)):
			names = _imageNames;

		default:
			break;
	}

	switch (role)
	{
		case Qt::DisplayRole:
			return QString("[%1]").arg(names.join(", "));

		case Qt::EditRole:
			return names;

		case Qt::ToolTipRole:
			return QString("Image names: [%1]").arg(names.join(", "));

		default:
			break;
	}

	return QStringList();
}

void ImageDataset::setImageNames(const QStringList& imageNames)
{
	_imageNames = imageNames;
}

QVariant ImageDataset::dimensionNames(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString("[%1]").arg(_dimensionNames.join(", "));

		case Qt::EditRole:
			return _dimensionNames;

		case Qt::ToolTipRole:
			return QString("Dimension names: [%1]").arg(_dimensionNames.join(", "));

		default:
			break;
	}

	return QStringList();
}

void ImageDataset::setDimensionNames(const QStringList& dimensionNames)
{
	_dimensionNames = dimensionNames;
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

void ImageDataset::setAverage(const bool& average)
{
	_average = average;
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

QVariant ImageDataset::currentImageFilePath(const int& role /*= Qt::DisplayRole*/) const
{
	if (_currentImage < 0 || _currentImage >= _imageFilePaths.size())
		return "";

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _imageFilePaths[_currentImage];

		case Qt::ToolTipRole:
			return QString("Current image file path: %1").arg(_imageFilePaths[_currentImage]);

		default:
			break;
	}

	return QString();
}

QVariant ImageDataset::currentDimensionFilePath(const int& role /*= Qt::DisplayRole*/) const
{
	if (_currentDimension < 0 || _currentDimension >= _imageFilePaths.size())
		return "";

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _imageFilePaths[_currentDimension];

		case Qt::ToolTipRole:
			return QString("Current dimension file path: %1").arg(_imageFilePaths[_currentDimension]);

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
	_selection = selection;
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
				case (static_cast<int>(ImageCollectionType::Sequence)):
					return QString("No. selected images: %1").arg(selectionSizeString);

				case (static_cast<int>(ImageCollectionType::Stack)):
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