#include "ColorMapModel.h"

#include <QDebug>
#include <QDirIterator>
#include <QBrush>

ColorMapModel::ColorMapModel(QObject* parent, const Type& type) :
	QAbstractListModel(parent),
	_colorMaps()
{
	setupModelData();
}

int ColorMapModel::columnCount(const QModelIndex& parent) const
{
	return ult(Column::End) + 1;
}

int ColorMapModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const
{
	return _colorMaps.count();
}

QVariant ColorMapModel::data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const
{
	if (!index.isValid())
		return QVariant();

	const auto colorMap = _colorMaps.at(index.row());

	switch (role) {
		case Qt::DecorationRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
				{
					switch (colorMap.noDimensions())
					{
						case 1:
							return QPixmap::fromImage(colorMap.image().scaled(QSize(100, 20)));

						case 2:
							return QPixmap::fromImage(colorMap.image().scaled(QSize(64, 64)));

						default:
							break;
					}

					break;
				}
					

				case ult(Column::Name):
				case ult(Column::Image):
				case ult(Column::ResourcePath):
					break;
			}

			break;
		}

		/*
		case Qt::BackgroundRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
					return QBrush(colorMap.image());

				case ult(Column::Name):
				case ult(Column::Image):
				case ult(Column::ResourcePath):
					break;
			}

			break;
		}

		*/
		case Qt::DisplayRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
					return colorMap.name();

				case ult(Column::Name):
					return colorMap.name();

				case ult(Column::Image):
					break;

				case ult(Column::ResourcePath):
					colorMap.resourcePath();
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
					return "asd";

				case ult(Column::Name):
					return colorMap.name();

				case ult(Column::Image):
					return colorMap.image();

				case ult(Column::NoDimensions):
					return colorMap.noDimensions();

				case ult(Column::ResourcePath):
					return colorMap.resourcePath();
			}
			break;
		}

		case Qt::TextAlignmentRole:
			return Qt::AlignLeft | Qt::AlignVCenter;

		default:
			break;
	}

	return QVariant();
}

void ColorMapModel::setupModelData()
{
	auto prefix = ":/resources/colormaps";

	QDirIterator iterator1D(QString("%1/1D/").arg(prefix), QDirIterator::Subdirectories);

	while (iterator1D.hasNext()) {
		const auto resourcePath = iterator1D.next();
		_colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, 1, QImage(resourcePath)));
	}

	QDirIterator iterator2D(QString("%1/2D/").arg(prefix), QDirIterator::Subdirectories);

	while (iterator2D.hasNext()) {
		const auto resourcePath = iterator2D.next();
		_colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, 2, QImage(resourcePath)));
	}

	beginInsertRows(QModelIndex(), 0, _colorMaps.count());
	endInsertRows();
}