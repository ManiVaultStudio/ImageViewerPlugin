#include "DatasetsModel.h"
#include "LayersModel.h"

#include "ImageData/Images.h"

#include <QItemSelectionModel>
#include <QDebug>

DatasetsModel::DatasetsModel(QObject* parent) :
	QAbstractListModel(parent),
	_selectionModel(new QItemSelectionModel(this))
{
	QObject::connect(this, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles) {
		const auto selectedRows = _selectionModel->selectedRows();

		if (selectedRows.size() != 1 || topLeft.row() != selectedRows.first().row())
			return;

		if (topLeft.column() <= Columns::CurrentImageName && bottomRight.column() >= Columns::CurrentImageName) {
			auto dataset = datasets().value(topLeft.row());

			if (dataset->type(Qt::EditRole).toInt() == static_cast<int>(ImageCollectionType::Sequence)) {
				const auto name = dataset->currentImageName(Qt::DisplayRole).toString();

				dataset->layersModel()->renameLayer("default_color", name);
				dataset->layersModel()->renameLayer("default_selection", name);
			}
		}

		if (topLeft.column() <= Columns::CurrentDimensionName && bottomRight.column() >= Columns::CurrentDimensionName) {
			auto dataset = datasets().value(topLeft.row());

			if (dataset->type(Qt::EditRole).toInt() == static_cast<int>(ImageCollectionType::Stack)) {
				const auto name = dataset->currentDimensionName(Qt::DisplayRole).toString();

				dataset->layersModel()->renameLayer("default_color", name);
				dataset->layersModel()->renameLayer("default_selection", name);
			}
		}
	});
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

	return 19;
}

QVariant DatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= columnCount(index) || index.row() < 0)
		return QVariant();

	auto dataset = _datasets.at(index.row());

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case Columns::Name:
				return dataset->name(Qt::DisplayRole);

			case Columns::Type:
				return dataset->type(Qt::DisplayRole);

			case Columns::NoImages:
				return dataset->noImages(Qt::DisplayRole);

			case Columns::Size:
				return dataset->size(Qt::DisplayRole);

			case Columns::NoPoints:
				return dataset->noPoints(Qt::DisplayRole);

			case Columns::NoDimensions:
				return dataset->noDimensions(Qt::DisplayRole);

			case Columns::CurrentImage:
				return dataset->currentImage(Qt::DisplayRole);

			case Columns::CurrentImageName:
				return dataset->currentImageName(Qt::DisplayRole);

			case Columns::CurrentDimension:
				return dataset->currentDimension(Qt::DisplayRole);

			case Columns::CurrentDimensionName:
				return dataset->currentDimensionName(Qt::DisplayRole);

			case Columns::ImageNames:
				return dataset->imageNames(Qt::DisplayRole);

			case Columns::DimensionNames:
				return dataset->dimensionNames(Qt::DisplayRole);

			case Columns::Average:
				return dataset->average(Qt::DisplayRole);

			case Columns::ImageFilePaths:
				return dataset->imageFilePaths(Qt::DisplayRole);

			case Columns::CurrentImageFilepath:
				return dataset->currentImageFilePath(Qt::DisplayRole);

			case Columns::CurrentDimensionFilepath:
				return dataset->currentDimensionFilePath(Qt::DisplayRole);

			case Columns::PointsName:
				return dataset->pointsName(Qt::DisplayRole);

			case Columns::Selection:
				return dataset->selection(Qt::DisplayRole);

			case Columns::SelectionSize:
				return dataset->selectionSize(Qt::DisplayRole);

			default:
				break;
		}
	}

	if (role == Qt::EditRole) {
		switch (index.column()) {
			case Columns::Name:
				return dataset->name(Qt::EditRole);

			case Columns::Type:
				return dataset->type(Qt::EditRole);

			case Columns::NoImages:
				return dataset->noImages(Qt::EditRole);

			case Columns::Size:
				return dataset->size(Qt::EditRole);

			case Columns::NoPoints:
				return dataset->noPoints(Qt::EditRole);

			case Columns::NoDimensions:
				return dataset->noDimensions(Qt::EditRole);

			case Columns::CurrentImage:
				return dataset->currentImage(Qt::EditRole);

			case Columns::CurrentImageName:
				return dataset->currentImageName(Qt::EditRole);

			case Columns::CurrentDimension:
				return dataset->currentDimension(Qt::EditRole);

			case Columns::CurrentDimensionName:
				return dataset->currentDimensionName(Qt::EditRole);

			case Columns::ImageNames:
				return dataset->imageNames(Qt::EditRole);

			case Columns::DimensionNames:
				return dataset->dimensionNames(Qt::EditRole);

			case Columns::Average:
				return dataset->average(Qt::EditRole);

			case Columns::ImageFilePaths:
				return dataset->imageFilePaths(Qt::EditRole);

			case Columns::CurrentImageFilepath:
				return dataset->currentImageFilePath(Qt::EditRole);

			case Columns::CurrentDimensionFilepath:
				return dataset->currentDimensionFilePath(Qt::EditRole);

			case Columns::PointsName:
				return dataset->pointsName(Qt::EditRole);

			case Columns::Selection:
				return dataset->selection(Qt::EditRole);

			case Columns::SelectionSize:
				return dataset->selectionSize(Qt::EditRole);

			default:
				break;
		}
	}
	
	if (role == Qt::ToolTipRole) {
		switch (index.column()) {
			case Columns::Name:
				return dataset->name(Qt::ToolTipRole);

			case Columns::Type:
				return dataset->type(Qt::ToolTipRole);

			case Columns::NoImages:
				return dataset->noImages(Qt::ToolTipRole);

			case Columns::Size:
				return dataset->size(Qt::ToolTipRole);

			case Columns::NoPoints:
				return dataset->noPoints(Qt::ToolTipRole);

			case Columns::NoDimensions:
				return dataset->noDimensions(Qt::ToolTipRole);

			case Columns::CurrentImage:
				return dataset->currentImage(Qt::ToolTipRole);

			case Columns::CurrentImageName:
				return dataset->currentImageName(Qt::ToolTipRole);

			case Columns::CurrentDimension:
				return dataset->currentDimension(Qt::ToolTipRole);

			case Columns::CurrentDimensionName:
				return dataset->currentDimensionName(Qt::ToolTipRole);

			case Columns::ImageNames:
				return dataset->imageNames(Qt::ToolTipRole);

			case Columns::DimensionNames:
				return dataset->dimensionNames(Qt::ToolTipRole);

			case Columns::Average:
				return dataset->average(Qt::ToolTipRole);

			case Columns::ImageFilePaths:
				return dataset->imageFilePaths(Qt::ToolTipRole);

			case Columns::CurrentImageFilepath:
				return dataset->currentImageFilePath(Qt::ToolTipRole);

			case Columns::CurrentDimensionFilepath:
				return dataset->currentDimensionFilePath(Qt::ToolTipRole);

			case Columns::PointsName:
				return dataset->pointsName(Qt::ToolTipRole);

			case Columns::Selection:
				return dataset->selection(Qt::ToolTipRole);

			case Columns::SelectionSize:
				return dataset->selectionSize(Qt::ToolTipRole);

			default:
				break;
		}
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

			case Columns::NoImages:
				return "No. Images";

			case Columns::Size:
				return "Size";

			case Columns::NoPoints:
				return "No. points";

			case Columns::NoDimensions:
				return "No. dimensions";

			case Columns::CurrentImage:
				return "Current image ID";

			case Columns::CurrentImageName:
				return "Image name";

			case Columns::CurrentDimension:
				return "Current dimension ID";

			case Columns::CurrentDimensionName:
				return "Dimension name";

			case Columns::ImageNames:
				return "Images";

			case Columns::DimensionNames:
				return "Dimensions";

			case Columns::Average:
				return "Average";

			case Columns::PointsName:
				return "Points dataset name";

			case Columns::Selection:
				return "Selection";

			case Columns::SelectionSize:
				return "No. selected items";

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

	const auto datasetType	= data(index.row(), Columns::Type, Qt::EditRole);
	const auto average		= data(index.row(), Columns::Average, Qt::EditRole);

	switch (index.column()) {
		case Columns::Name:
		case Columns::Type:
		case Columns::NoImages:
		case Columns::Size:
		case Columns::NoPoints:
		case Columns::NoDimensions:
		case Columns::CurrentImageName:
		case Columns::CurrentDimensionName:
		case Columns::ImageNames:
		case Columns::DimensionNames:
			break;

		case Columns::CurrentImage:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Sequence) != average)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::CurrentDimension:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Stack))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Average:
		{
			if (datasetType == static_cast<int>(ImageCollectionType::Sequence))
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::PointsName:
		case Columns::Selection:
		case Columns::SelectionSize:
			break;

		default:
			break;
	}
	
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
				dataset->setType(value.toInt());
				break;

			case Columns::NoImages:
				break;

			case Columns::Size:
				dataset->setSize(value.toSize());
				break;

			case Columns::NoPoints:
				dataset->setNoPoints(value.toInt());
				break;

			case Columns::NoDimensions:
				dataset->setNoDimensions(value.toInt());
				break;

			case Columns::CurrentImage:
				dataset->setCurrentImage(value.toInt());
				break;

			case Columns::CurrentDimension:
				dataset->setCurrentDimension(value.toInt());
				break;

			case Columns::ImageNames:
				dataset->setImageNames(value.toStringList());
				break;

			case Columns::DimensionNames:
				dataset->setDimensionNames(value.toStringList());
				break;

			case Columns::Average:
				dataset->setAverage(value.toBool());
				break;

			case Columns::ImageFilePaths:
				dataset->setImageFilePaths(value.toStringList());
				break;

			case Columns::CurrentImageFilepath:
			case Columns::CurrentDimensionFilepath:
				break;

			case Columns::PointsName:
				dataset->setPointsName(value.toString());
				break;

			case Columns::Selection:
				dataset->setSelection(value.value<Indices>());
				break;

			case Columns::SelectionSize:
				break;

			default:
				return false;
		}

		/* TODO
		switch (index.column())
		{
			case Columns::Average:
			case Columns::CurrentImage:
			case Columns::CurrentDimension:
			case Columns::Selection:
				emit dataChanged(this->index(row, Columns::CurrentImage), this->index(row, Columns::CurrentImage));
				emit dataChanged(this->index(row, Columns::CurrentDimension), this->index(row, Columns::CurrentDimension));
				emit dataChanged(this->index(row, Columns::CurrentImageName), this->index(row, Columns::CurrentImageName));
				emit dataChanged(this->index(row, Columns::CurrentDimensionName), this->index(row, Columns::CurrentDimensionName));
				emit dataChanged(this->index(row, Columns::ImageNames), this->index(row, Columns::ImageNames));
				break;

			default:
				
		}
		*/

		emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));

		return true;
	}

	return false;
}

bool DatasetsModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		datasets().insert(position, new ImageDataset(this));
	}

	endInsertRows();

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

QVariant DatasetsModel::data(const int& row, const int& column, int role /*= Qt::DisplayRole*/) const
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

void DatasetsModel::add(ImageDataset* dataset)
{
	insertRows(0, 1);

	setData(index(0, Columns::Name, QModelIndex()), dataset->name(Qt::EditRole));
	setData(index(0, Columns::Type, QModelIndex()), dataset->type(Qt::EditRole));
	setData(index(0, Columns::Size, QModelIndex()), dataset->size(Qt::EditRole));
	setData(index(0, Columns::NoPoints, QModelIndex()), dataset->noPoints(Qt::EditRole));
	setData(index(0, Columns::NoDimensions, QModelIndex()), dataset->noDimensions(Qt::EditRole));
	setData(index(0, Columns::ImageNames, QModelIndex()), dataset->imageNames(Qt::EditRole));
	setData(index(0, Columns::DimensionNames, QModelIndex()), dataset->dimensionNames(Qt::EditRole));
	setData(index(0, Columns::Average, QModelIndex()), dataset->average(Qt::EditRole));
	setData(index(0, Columns::ImageFilePaths, QModelIndex()), dataset->imageFilePaths(Qt::EditRole));
	setData(index(0, Columns::CurrentImage, QModelIndex()), dataset->currentImage(Qt::EditRole));
	setData(index(0, Columns::CurrentDimension, QModelIndex()), dataset->currentDimension(Qt::EditRole));
	setData(index(0, Columns::PointsName, QModelIndex()), dataset->pointsName(Qt::EditRole));
}

LayersModel* DatasetsModel::layersModel(const int& row)
{
	return _datasets[row]->layersModel().get();
}