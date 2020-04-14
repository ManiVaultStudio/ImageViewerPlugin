#include "LayerNode.h"
#include "ImageViewerPlugin.h"

#include "ImageData/Images.h"
#include "PointData.h"

#include <QFont>
#include <QDebug>

ImageViewerPlugin* LayerNode::imageViewerPlugin = nullptr;

LayerNode::LayerNode(const QString& dataset, const Type& type, const QString& id, const QString& name, const int& flags) :
	RenderNode(id, name, flags),
	_datasetName(dataset),
	_rawDataName(),
	_type(type),
	_colorMap()
{
	if (!dataset.isEmpty()) {
		switch (_type)
		{
			case LayerNode::Type::Points:
			{
				const Points& points = imageViewerPlugin->core()->requestData<Points>(_datasetName);
				_rawDataName = hdps::DataSet::getSourceData(points).getDataName();
				break;
			}

			case LayerNode::Type::Images:
			{
				Images& images = imageViewerPlugin->core()->requestData<Images>(_datasetName);
				_rawDataName = hdps::DataSet::getSourceData(*(images.points())).getDataName();
				break;
			}

			case LayerNode::Type::Clusters:
			case LayerNode::Type::Selection:
			case LayerNode::Type::Group:
			default:
				break;
		}
	}
}

LayerNode::~LayerNode() = default;

int LayerNode::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags LayerNode::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

	const auto type = static_cast<Type>(_type);

	switch (static_cast<Column>(index.column())) {
		case Column::Name:
		{
			flags |= Qt::ItemIsUserCheckable;

			if (flag(LayerNode::Flag::Renamable, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Type:
			break;

		case Column::ID:
		case Column::DatasetName:
		case Column::RawDataName:
			break;

		case Column::Opacity:
			flags |= Qt::ItemIsEditable;
			break;
			
		case Column::ColorMap:
		{
			if (type == Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Flags:
			break;

		case Column::Selection:
		case Column::SelectionSize:
			break;

		default:
			break;
	}

	return flags;
}

QVariant LayerNode::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		case Column::Name:
			return name(role);

		case Column::Type:
			return type(role);

		case Column::ID:
			return id(role);

		case Column::DatasetName:
			return datasetName(role);

		case Column::RawDataName:
			return rawDataName(role);

		case Column::Opacity:
			return opacity(role);

		case Column::ColorMap:
			return colorMap(role);

		case Column::Flags:
			return Node::flags(role);

		case Column::Selection:
			return selection(role);

		case Column::SelectionSize:
			return selectionSize(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList LayerNode::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIndices{index};

	const auto column = static_cast<Column>(index.column());

	switch (role)
	{
		case Qt::CheckStateRole:
		{
			switch (column) {
				case Column::Name:
				{
					setFlag(LayerNode::Flag::Enabled, value.toBool());

					for (int column = ult(Column::Type); column <= ult(Column::End); ++column) {
						affectedIndices.append(index.siblingAtColumn(column));
					}

					break;
				}

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (column) {
				case Column::Name:
					setName(value.toString());
					break;

				case Column::Type:
					setType(static_cast<Type>(value.toInt()));
					break;

				case Column::ID:
					setId(value.toString());
					break;

				case Column::DatasetName:
				case Column::RawDataName:
					break;

				case Column::Opacity:
					setOpacity(value.toFloat());
					break;
				
				case Column::ColorMap:
					setColorMap(value.value<QImage>());
					break;

				case Column::Flags:
					setFlags(value.toInt());
					break;

				case Column::Selection:
					setSelection(value.value<Indices>());
					affectedIndices << index.siblingAtColumn(ult(Column::SelectionSize));
					break;

				case Column::SelectionSize:
					break;

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	rootItem()->render();

	return affectedIndices;
}

QVariant LayerNode::datasetName(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _datasetName;

		case Qt::ToolTipRole:
			return QString("Dataset name: %1").arg(_datasetName);

		default:
			break;
	}

	return QVariant();
}

QVariant LayerNode::rawDataName(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _rawDataName;

		case Qt::ToolTipRole:
			return QString("Raw data name: %1").arg(_rawDataName);

		default:
			break;
	}

	return QVariant();
}

QVariant LayerNode::type(const int& role) const
{
	const auto typeName = LayerNode::typeName(_type);

	switch (role)
	{
		case Qt::FontRole:
			return QFont("Font Awesome 5 Free Solid", 9);

		case Qt::EditRole:
			return static_cast<int>(_type);

		case Qt::ToolTipRole:
			return QString("Type: %1").arg(typeName);

		case Qt::DisplayRole:
		{
			switch (_type) {
				case Type::Images:
					return u8"\uf03e";

				case Type::Selection:
					return u8"\uf065";

				case Type::Clusters:
					return u8"\uf141";

				case Type::Points:
					return u8"\uf03e";

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QVariant();
}

void LayerNode::setType(const Type& type)
{
	_type = type;
}

QVariant LayerNode::colorMap(const int& role) const
{
	const auto colorMapString = "Image";

	switch (role)
	{
		case Qt::DisplayRole:
			return colorMapString;

		case Qt::EditRole:
			return _colorMap;

		case Qt::ToolTipRole:
			return QString("%1").arg(colorMapString);

		default:
			break;
	}

	return QVariant();
}

void LayerNode::setColorMap(const QImage& colorMap)
{
	_colorMap = colorMap;
}

QVariant LayerNode::selection(const int& role /*= Qt::DisplayRole*/) const
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

	return QVariant();
}

void LayerNode::setSelection(const Indices& selection)
{
	_selection = selection;
}

QVariant LayerNode::selectionSize(const int& role /*= Qt::DisplayRole*/) const
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
			return QString("Selection size: %1").arg(selectionSizeString);
		}

		default:
			break;
	}

	return QVariant();
}