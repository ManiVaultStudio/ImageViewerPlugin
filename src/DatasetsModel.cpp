#include "DatasetsModel.h"
#include "ImageViewerPlugin.h"
#include "ImagesDataset.h"
#include "PointsDataset.h"
#include "ClustersDataset.h"

#include <QDebug>

DatasetsModel::DatasetsModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractListModel(imageViewerPlugin),
	_imageViewerPlugin(imageViewerPlugin),
	_datasets()
{
}

DatasetsModel::~DatasetsModel() = default;

int DatasetsModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return datasets().size();
}

int DatasetsModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return 3;
}

QVariant DatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= columnCount(index) || index.row() < 0)
		return QVariant();

	auto dataset = _datasets.at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (index.column()) {
				case Columns::Name:
					return dataset->name(Qt::DisplayRole);

				case Columns::Type:
					return dataset->type(Qt::DisplayRole);

				case Columns::SelectionImage:
					return dataset->selectionImage(Qt::DisplayRole);

				default:
					break;
			}
		}

		case Qt::EditRole:
		{
			switch (index.column()) {
				case Columns::Name:
					return dataset->name(Qt::EditRole);

				case Columns::Type:
					return dataset->type(Qt::EditRole);

				case Columns::SelectionImage:
					return dataset->selectionImage(Qt::EditRole);

				default:
					break;
			}
		}

		case Qt::ToolTipRole:
		{
			switch (index.column()) {
				case Columns::Name:
					return dataset->name(Qt::ToolTipRole);

				case Columns::Type:
					return dataset->type(Qt::ToolTipRole);

				case Columns::SelectionImage:
					return dataset->selectionImage(Qt::ToolTipRole);

				default:
					break;
			}
		}

		default:
			break;
	}

	return QVariant();
}

QVariant DatasetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case Columns::Name:
				return "Name";

			case Columns::Type:
				return "Type";

			case Columns::SelectionImage:
				return "Selection Image";

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
	
	return flags;
}

bool DatasetsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto dataset = datasets().value(row);

		switch (index.column()) {
			case Columns::Name:
				dataset->setName(value.toString());
				break;

			case Columns::Type:
				dataset->setType(static_cast<Dataset::Type>(value.toInt()));
				break;

			case Columns::SelectionImage:
				dataset->setSelectionImage(value.value<QImage>());

			default:
				return false;
		}

		emit dataChanged(this->index(row, index.column()), this->index(row, index.column()));
		
		return true;
	}

	return false;
}

Dataset* DatasetsModel::findDataset(const QString& name)
{
	const auto hits = match(index(0, DatasetsModel::Columns::Name), Qt::EditRole, name, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return nullptr;

	return _datasets[hits.first().row()];
}

bool DatasetsModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	/*
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		datasets().insert(position, new Dataset(this));
	}

	endInsertRows();
	*/

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
	return _datasets;
}

Datasets& DatasetsModel::datasets()
{
	const auto constThis = const_cast<const DatasetsModel*>(this);
	return const_cast<Datasets&>(constThis->datasets());
}

QVariant DatasetsModel::data(const int& row, const int& column, int role) const
{
	const auto modelIndex = index(row, column);

	if (!modelIndex.isValid())
		return QVariant();

	return data(index(row, column), role);
}

void DatasetsModel::setData(const int& row, const int& column, const QVariant& value)
{
	const auto modelIndex = index(row, column);

	if (!modelIndex.isValid())
		return;

	setData(modelIndex, value);
}

Dataset* DatasetsModel::addDataset(const QString& name, const Dataset::Type& type)
{
	if (findDataset(name) == nullptr) {
		beginInsertRows(QModelIndex(), 0, 0);

		switch (type)
		{
			case Dataset::Type::Images:
				datasets().insert(0, new ImagesDataset(_imageViewerPlugin, name));
				break;

			case Dataset::Type::Points:
				datasets().insert(0, new PointsDataset(_imageViewerPlugin, name));
				break;

			case Dataset::Type::Clusters:
				datasets().insert(0, new ClustersDataset(_imageViewerPlugin, name));
				break;

			default:
				break;
		}

		endInsertRows();
	}
		
	return findDataset(name);
}