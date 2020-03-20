#include "Dataset.h"
#include "ImageViewerPlugin.h"

#include <QVariant>

Dataset::Dataset(ImageViewerPlugin* imageViewerPlugin, const QString& name, const Dataset::Type& type) :
	QObject(imageViewerPlugin),
	_imageViewerPlugin(imageViewerPlugin),
	_name(name),
	_type(type),
	_selectionImage()
{
}

QVariant Dataset::name(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _name;

		case Qt::ToolTipRole:
			return QString("Name: %1").arg(_name);

		default:
			break;
	}

	return QVariant();
}

void Dataset::setName(const QString& name)
{
	_name = name;
}

QVariant Dataset::type(const int& role /*= Qt::DisplayRole*/) const
{
	auto typeString = Dataset::typeName(_type);

	switch (role)
	{
		case Qt::DisplayRole:
			return typeString;

		case Qt::EditRole:
			return static_cast<int>(_type);

		case Qt::ToolTipRole:
			return QString("Type: %1").arg(typeString);

		default:
			break;
	}

	return QVariant();
}

void Dataset::setType(const Dataset::Type& type)
{
	_type = type;
}

QVariant Dataset::selectionImage(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageString = QString("Image %1x%2").arg(QString::number(_selectionImage.width()), QString::number(_selectionImage.height()));

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
			return imageString;

		case Qt::EditRole:
			return _selectionImage;

		default:
			break;
	}

	return QVariant();
}

void Dataset::setSelectionImage(const QImage& selectionImage)
{
	_selectionImage = selectionImage;
}