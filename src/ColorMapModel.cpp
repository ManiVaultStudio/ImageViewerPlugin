#include "ColorMapModel.h"

#include <QDebug>
#include <QDirIterator>
#include <QPainter>

ColorMapModel::ColorMapModel(QObject* parent, const ColorMap::Type& type) :
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

	auto icon = [](const QImage& image, const QSize& size) {
		auto pixmap		= QPixmap::fromImage(image).scaled(size);
		auto painter	= QPainter(&pixmap);

		painter.setPen(QPen(QBrush(QColor(30, 30, 30)), 1.5f));

		QPointF points[5] = {
			QPointF(0.0f, 0.0f),
			QPointF(size.width(), 0.0f),
			QPointF(size.width(), size.height()),
			QPointF(0.0f, size.height()),
			QPointF(0.0f, 0.0f)
		};

		painter.drawPolyline(points, 5);

		return pixmap;
	};

	switch (role) {
		case Qt::DecorationRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
				{
					switch (colorMap.noDimensions())
					{
						case 1:
							return icon(colorMap.image(), QSize(60, 12));

						case 2:
							return icon(colorMap.image(), QSize(32, 32));

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
					return QVariant();

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
		_colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::OneDimensional, QImage(resourcePath)));
	}

	QDirIterator iterator2D(QString("%1/2D/").arg(prefix), QDirIterator::Subdirectories);

	while (iterator2D.hasNext()) {
		const auto resourcePath = iterator2D.next();
		_colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::TwoDimensional, QImage(resourcePath)));
	}

	beginInsertRows(QModelIndex(), 0, _colorMaps.count());
	endInsertRows();
}

const ColorMap* ColorMapModel::colorMap(const int& row) const
{
	const auto colorMapIndex = index(row, 0);

	if (colorMapIndex.isValid())
		return nullptr;

	return &_colorMaps.at(row);
}