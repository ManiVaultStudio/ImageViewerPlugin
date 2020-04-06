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
	return ult(Column::End);
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
		case Qt::BackgroundRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
					return QBrush(colorMap.image());

				case ult(Column::Name):
				case ult(Column::Image):
					break;
			}

			break;
		}

		case Qt::DisplayRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
					break;

				case ult(Column::Name):
					return colorMap.name();

				case ult(Column::Image):
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (index.column()) {
				case ult(Column::Preview):
					break;

				case ult(Column::Name):
					return colorMap.name();

				case ult(Column::Image):
					return colorMap.image();
			}
			break;
		}

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
		const auto path = iterator1D.next();
		_colorMaps.append(ColorMap(QFileInfo(path).fileName(), 1, QImage(path)));
	}

	QDirIterator iterator2D(QString("%1/2D/").arg(prefix), QDirIterator::Subdirectories);

	while (iterator2D.hasNext()) {
		const auto path = iterator2D.next();
		_colorMaps.append(ColorMap(QFileInfo(path).fileName(), 2, QImage(path)));
	}

	beginInsertRows(QModelIndex(), 0, _colorMaps.count());
	endInsertRows();
}