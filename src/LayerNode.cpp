#include "LayerNode.h"
#include "Dataset.h"

#include <QFont>
#include <QDebug>

LayerNode::LayerNode(Dataset* dataset, const Type& type, const QString& id, const QString& name, const int& flags) :
	RenderNode(id, name, flags),
	_dataset(dataset),
	_type(type),
	_colorMap()
{
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
		case Column::Dataset:
			break;

		case Column::Opacity:
			flags |= Qt::ItemIsEditable;
			break;
			
		/*
		case Column::WindowNormalized:
		{
			if (type == Type::Images)
				flags |= Qt::ItemIsEditable;

			break;
		}
		
		case Column::LevelNormalized:
		{
			if (type == Type::Images)
				flags |= Qt::ItemIsEditable;

			break;
		}
		*/

		case Column::ColorMap:
		{
			if (type == Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Image:
			break;
		
		/*
		case Column::ImageRange:
		case Column::DisplayRange:
		*/

		case Column::Flags:
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

		case Column::Dataset:
			return dataset(role);

		case Column::Opacity:
			return opacity(role);

		/*
		case Column::WindowNormalized:
			return windowNormalized(role);

		case Column::LevelNormalized:
			return levelNormalized(role);
		*/

		case Column::ColorMap:
			return colorMap(role);
		
		/*
		case Column::Image:
			return image(role);
		
		case Column::ImageRange:
			return imageRange(role);

		case Column::DisplayRange:
			return displayRange(role);
		*/

		case Column::Flags:
			return Node::flags(role);

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

				case Column::Dataset:
					break;

				case Column::Opacity:
					setOpacity(value.toFloat());
					break;
				
				/*
				case Column::WindowNormalized:
					setWindowNormalized(value.toFloat());
					break;

				case Column::LevelNormalized:
					setLevelNormalized(value.toFloat());
					break;
				*/

				case Column::ColorMap:
					setColorMap(value.value<QImage>());
					break;
				
				/*
				case Column::Image:
					setImage(value.value<QImage>());
					break;

				case Column::ImageRange:
				case Column::DisplayRange:
					break;
				*/

				case Column::Flags:
					setFlags(value.toInt());
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

QVariant LayerNode::dataset(const int& role) const
{
	const auto name = _dataset ? _dataset->name(role).toString() : "";

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return name;

		case Qt::ToolTipRole:
			return QString("Dataset name: %1").arg(name);

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