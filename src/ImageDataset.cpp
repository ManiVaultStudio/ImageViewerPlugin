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
	_averageImages(),
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

	return "";
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

	return "";
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

	return 0;
}

QVariant ImageDataset::currentImageName(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _imageNames.isEmpty() ? "" : _imageNames[_currentImage];
	}

	return "";
}

QVariant ImageDataset::imageNames(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
		{
			switch (_type)
			{
				case (static_cast<int>(ImageCollectionType::Sequence)):
				{
					const auto selectionSize	= _selection.size();
					const auto noImages			= _imageNames.size();

					if (_averageImages) {
						if (selectionSize == 0) {
							if (noImages == 1)
								return QStringList() << QString("[%1]").arg(_imageNames.first());

							if (noImages == 2)
								return QStringList() << QString("[%1, %2]").arg(_imageNames.first(), _imageNames.last());

							if (noImages > 2)
								return QStringList() << QString("[%1, ..., %2]").arg(_imageNames.first(), _imageNames.last());
						}

						if (selectionSize == 1)
							return QStringList() << _imageNames[_selection.first()];

						if (selectionSize == 2)
							return QStringList() << QString("[%1, %3]").arg(_imageNames[_selection.first()], _imageNames[_selection.last()]);

						if (selectionSize > 2)
							return QStringList() << QString("[%1, .., %3]").arg(_imageNames[_selection.first()], _imageNames[_selection.last()]);
					}
					else {
						if (selectionSize <= 0) {
							return _imageNames;
						}
						else {
							auto names = QStringList();

							for (auto selectionIndex : _selection) {
								names << _imageNames[selectionIndex];
							}

							return names;
						}
					}

					break;
				}

				case (static_cast<int>(ImageCollectionType::Stack)):
					return _imageNames;

				default:
					break;
			}

			break;
		}
	}

	return QStringList();
}