#include "Layer.h"
#include "Dataset.h"

#include <QFont>
#include <QDebug>

Layer::Layer(Dataset* dataset, const Type& type, const QString& id, const QString& name, const int& flags) :
	_children(),
	_parent(nullptr),
	_dataset(dataset),
	_id(id),
	_name(name),
	_type(type),
	_flags(flags),
	_order(0),
	_opacity(1.0f),
	_colorMap(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(),
	_level()
{
}

Layer::~Layer()
{
	qDeleteAll(_children);
}

void Layer::appendChild(Layer* child)
{
	_children.append(child);

	child->setParent(this);
	child->setOrder(_children.size());
}

const Layer* Layer::child(const int& row) const
{
	if (row < 0 || row >= _children.size())
		return nullptr;

	return _children.at(row);
}

Layer* Layer::child(const int& row)
{
	const auto constThis = const_cast<const Layer*>(this);
	return const_cast<Layer*>(constThis->child(row));
}

int Layer::childCount() const
{
	return _children.count();
}

int Layer::row() const
{
	if (_parent)
		return _parent->_children.indexOf(const_cast<Layer*>(this));

	return 0;
}

const Layer* Layer::parent() const
{
	return _parent;
}

Layer* Layer::parent()
{
	const auto constThis = const_cast<const Layer*>(this);
	return const_cast<Layer*>(constThis->parent());
}

void Layer::setParent(Layer* parent)
{
	_parent = parent;
}

int Layer::columnCount()
{
	return static_cast<int>(Column::End);
}

QVariant Layer::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	if (orientation == Qt::Horizontal) {
		return columnName(static_cast<Column>(section));
	}

	return QVariant();
}

Qt::ItemFlags Layer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	const auto type = static_cast<Type>(_type);

	switch (static_cast<Column>(index.column())) {
		case Column::Enabled:
			flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
			break;

		case Column::Type:
		case Column::Locked:
		case Column::ID:
			break;

		case Column::Name:
		{
			if (flag(Layer::Flag::Renamable, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Dataset:
		case Column::Frozen:
		case Column::Removable:
			break;

		case Column::Mask:
		{
			if (type == Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Order:
			break;

		case Column::Opacity:
			flags |= Qt::ItemIsEditable;
			break;

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

		case Column::ColorMap:
		{
			if (type == Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Image:
			break;

		case Column::ImageRange:
		case Column::DisplayRange:
			break;

		default:
			break;
	}

	return flags;
}

QVariant Layer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		case Column::Enabled:
			return flag(Layer::Flag::Enabled, role);

		case Column::Type:
			return type(role);

		case Column::Locked:
			return flag(Layer::Flag::Frozen, role);

		case Column::ID:
			return id(role);

		case Column::Name:
			return name(role);

		case Column::Dataset:
			return dataset(role);

		case Column::Frozen:
			return flag(Layer::Flag::Frozen, role);

		case Column::Removable:
			return flag(Layer::Flag::Removable, role);

		case Column::Mask:
			return flag(Layer::Flag::Mask, role);

		case Column::Renamable:
			return flag(Layer::Flag::Renamable, role);

		case Column::Order:
			return order(role);

		case Column::Opacity:
			return opacity(role);

		case Column::WindowNormalized:
			return windowNormalized(role);

		case Column::LevelNormalized:
			return levelNormalized(role);

		case Column::ColorMap:
			return colorMap(role);

		case Column::Image:
			return image(role);

		case Column::ImageRange:
			return imageRange(role);

		case Column::DisplayRange:
			return displayRange(role);

		default:
			break;
	}

	return QVariant();
}

void Layer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	const auto column = static_cast<Column>(index.column());

	if (role == Qt::CheckStateRole) {
		switch (column) {
			case Column::Enabled:
				setFlag(Layer::Flag::Enabled, value == Qt::Checked ? true : false);
				break;

			default:
				break;
		}
	}

	if (role == Qt::DisplayRole) {
		switch (column) {
			case Column::Enabled:
				setFlag(Layer::Flag::Enabled, value.toBool());
				break;

			case Column::Type:
				setType(static_cast<Type>(value.toInt()));
				break;

			case Column::Locked:
				break;

			case Column::ID:
				setId(value.toString());
				break;

			case Column::Name:
				setName(value.toString());
				break;

			case Column::Dataset:
				break;

			case Column::Frozen:
				setFlag(Layer::Flag::Frozen, value.toBool());
				break;

			case Column::Removable:
				setFlag(Layer::Flag::Removable, value.toBool());
				break;

			case Column::Mask:
				setFlag(Layer::Flag::Mask, value.toBool());
				break;

			case Column::Renamable:
				setFlag(Layer::Flag::Renamable, value.toBool());
				break;

			case Column::Order:
				setOrder(value.toInt());
				break;

			case Column::Opacity:
				setOpacity(value.toFloat());
				break;

			case Column::WindowNormalized:
				setWindowNormalized(value.toFloat());
				break;

			case Column::LevelNormalized:
				setLevelNormalized(value.toFloat());
				break;

			case Column::ColorMap:
				setColorMap(value.value<QImage>());
				break;

			case Column::Image:
				setImage(value.value<QImage>());
				break;

			case Column::ImageRange:
			case Column::DisplayRange:
				break;

			default:
				break;
		}
	}
}

bool Layer::isSettingsIndex(const QModelIndex& index) const
{
	return index.parent().isValid() && index.parent().column() == static_cast<int>(Layer::Column::Settings);
}

QVariant Layer::id(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _id;

		case Qt::ToolTipRole:
			return QString("ID: %1").arg(_id);

		default:
			break;
	}

	return QVariant();
}

void Layer::setId(const QString& id)
{
	_id = id;
}

QVariant Layer::name(const int& role) const
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

void Layer::setName(const QString& name)
{
	_name = name;
}

QVariant Layer::dataset(const int& role) const
{
	const auto name = _dataset->name(role).toString();

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

QVariant Layer::type(const int& role) const
{
	const auto typeName = Layer::typeName(_type);

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

void Layer::setType(const Type& type)
{
	_type = type;
}

QVariant Layer::flag(const Layer::Flag& flag, const int& role) const
{
	const auto isFlagSet = _flags & static_cast<int>(flag);
	const auto flagString = isFlagSet ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return flagString;

		case Qt::EditRole:
			return isFlagSet;

		case Qt::ToolTipRole:
		{
			switch (flag)
			{
				case Layer::Flag::Enabled:
					return QString("Enabled: %1").arg(flagString);

				case Layer::Flag::Frozen:
					return QString("Frozen: %1").arg(flagString);

				case Layer::Flag::Removable:
					return QString("Removable: %1").arg(flagString);

				case Layer::Flag::Mask:
					return QString("Mask: %1").arg(flagString);

				case Layer::Flag::Renamable:
					return QString("Renamable: %1").arg(flagString);

				default:
					break;
			}

			break;
		}

		case Qt::CheckStateRole:
		{
			if (flag == Layer::Flag::Enabled)
				return isFlagSet ? Qt::Checked : Qt::Unchecked;

			break;
		}

		default:
			break;
	}

	return QVariant();
}

void Layer::setFlag(const Layer::Flag& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= static_cast<int>(flag);
	else
		_flags = _flags & ~static_cast<int>(flag);
}

void Layer::setFlags(const int& flags)
{
	_flags = flags;
}

QVariant Layer::order(const int& role) const
{
	const auto orderString = QString::number(_order);

	switch (role)
	{
		case Qt::DisplayRole:
			return orderString;

		case Qt::EditRole:
			return _order;

		case Qt::ToolTipRole:
			return QString("Order: %1").arg(orderString);

		default:
			break;
	}

	return QVariant();
}

void Layer::setOrder(const std::uint32_t& order)
{
	_order = order;
}

QVariant Layer::opacity(const int& role) const
{
	const auto opacityString = QString("%1%").arg(QString::number(100.0f * _opacity, 'f', 1));

	switch (role)
	{
		case Qt::DisplayRole:
			return opacityString;

		case Qt::EditRole:
			return _opacity;

		case Qt::ToolTipRole:
			return QString("Opacity: %1").arg(opacityString);

		default:
			break;
	}

	return QVariant();
}

void Layer::setOpacity(const float& opacity)
{
	_opacity = opacity;
}

QVariant Layer::colorMap(const int& role) const
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

void Layer::setColorMap(const QImage& colorMap)
{
	_colorMap = colorMap;
}

QVariant Layer::image(const int& role) const
{
	const auto imageString = "image";

	switch (role)
	{
		case Qt::DisplayRole:
			return imageString;

		case Qt::EditRole:
			return _image;

		case Qt::ToolTipRole:
			return QString("Image: %1").arg(imageString);

		default:
			break;
	}

	return QVariant();
}

void Layer::setImage(const QImage& image)
{
	_image = image;
	_windowNormalized = 1.0f;
	_levelNormalized = 0.5f;

	computeImageRange();
	computeDisplayRange();
}

QVariant Layer::imageRange(const int& role) const
{
	const auto imageRangeString = QString("[%1, %2]").arg(QString::number(_imageRange.min(), 'f', 2), QString::number(_imageRange.max(), 'f', 2));

	switch (role)
	{
		case Qt::DisplayRole:
			return imageRangeString;

		case Qt::EditRole:
			return QVariant::fromValue(_imageRange);

		case Qt::ToolTipRole:
			return QString("Image range: %1").arg(imageRangeString);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::displayRange(const int& role) const
{
	const auto displayRangeString = QString("[%1, %2]").arg(QString::number(_displayRange.min(), 'f', 2), QString::number(_displayRange.max(), 'f', 2));

	switch (role)
	{
		case Qt::DisplayRole:
			return displayRangeString;

		case Qt::EditRole:
			return QVariant::fromValue(_displayRange);

		case Qt::ToolTipRole:
			return QString("Display range: %1").arg(displayRangeString);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::windowNormalized(const int& role) const
{
	const auto windowNormalizedString = QString::number(_windowNormalized, 'f', 2);

	switch (role)
	{
		case Qt::DisplayRole:
			return windowNormalizedString;

		case Qt::EditRole:
			return _windowNormalized;

		case Qt::ToolTipRole:
			return QString("Window (normalized): %1").arg(windowNormalizedString);

		default:
			break;
	}

	return QVariant();
}

void Layer::setWindowNormalized(const float& windowNormalized)
{
	_windowNormalized = windowNormalized;

	computeDisplayRange();
}

QVariant Layer::levelNormalized(const int& role) const
{
	const auto levelNormalizedString = QString::number(_levelNormalized, 'f', 2);

	switch (role)
	{
		case Qt::DisplayRole:
			return levelNormalizedString;

		case Qt::EditRole:
			return _levelNormalized;

		case Qt::ToolTipRole:
			return QString("Level (normalized): %1").arg(levelNormalizedString);

		default:
			break;
	}

	return QVariant();
}

void Layer::setLevelNormalized(const float& levelNormalized)
{
	_levelNormalized = levelNormalized;

	computeDisplayRange();
}

QVariant Layer::window(const int& role) const
{
	return _window;
}

void Layer::setWindow(const float& window)
{
	_window = window;
}

QVariant Layer::level(const int& role) const
{
	return _level;
}

void Layer::setLevel(const float& level)
{
	_level = level;
}

void Layer::computeImageRange()
{
	if (_image.isNull())
		return;

	//qDebug() << "Compute image range";

	_imageRange.setFullRange();

	switch (_image.format())
	{
		case QImage::Format_RGBX64:
		case QImage::Format_RGBA64:
		case QImage::Format_RGBA64_Premultiplied:
		{
			for (std::int32_t y = 0; y < _image.height(); y++)
			{
				for (std::int32_t x = 0; x < _image.width(); x++)
				{
					const auto pixelColor = _image.pixelColor(x, y).rgba64();

					_imageRange.include(pixelColor.red());
					_imageRange.include(pixelColor.green());
					_imageRange.include(pixelColor.blue());
				}
			}
			break;
		}

		default:
		{
			for (std::int32_t y = 0; y < _image.height(); y++)
			{
				for (std::int32_t x = 0; x < _image.width(); x++)
				{
					const auto pixelColor = _image.pixelColor(x, y);

					_imageRange.include(pixelColor.red());
					_imageRange.include(pixelColor.green());
					_imageRange.include(pixelColor.blue());
				}
			}
			break;
		}
	}
}

void Layer::computeDisplayRange()
{
	//qDebug() << "Compute display range";

	const auto maxWindow = _imageRange.length();

	_level = std::clamp(_imageRange.min() + (_levelNormalized * maxWindow), _imageRange.min(), _imageRange.max());
	_window = std::clamp(_windowNormalized * maxWindow, _imageRange.min(), _imageRange.max());

	_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _imageRange.min(), _imageRange.max()));
	_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _imageRange.min(), _imageRange.max()));
}


