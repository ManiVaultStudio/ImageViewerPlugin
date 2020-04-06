#include "ColorMapModel.h"

#include <QDebug>
#include <QDirIterator>

ColorMapModel::ColorMapModel(QObject* parent, const Type& type /*= Type::OneDimensional*/) :
	QAbstractListModel(parent),
	_type(type),
	_colorMaps()
{
}

int ColorMapModel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const
{
	return _colorMaps.count();
}

void ColorMapModel::setupModelData()
{
	QDirIterator it("/colormaps", QDirIterator::Subdirectories);

	while (it.hasNext()) {
		qDebug() << it.next();
	}
}