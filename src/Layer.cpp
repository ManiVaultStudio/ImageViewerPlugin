#include "Layer.h"
#include "Dataset.h"

#include <QFont>
#include <QDebug>

_Layer::_Layer(Dataset* dataset, const Type& type, const QString& id, const QString& name, const int& flags) :
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

_Layer::~_Layer()
{
	qDeleteAll(_children);
}

void _Layer::appendChild(_Layer* child)
{
	child->setParent(this);
	child->setOrder(_children.size());

	_children.append(child);
}

const _Layer* _Layer::child(const int& row) const
{
	if (row < 0 || row >= _children.size())
		return nullptr;

	return _children.at(row);
}

_Layer* _Layer::child(const int& row)
{
	const auto constThis = const_cast<const _Layer*>(this);
	return const_cast<_Layer*>(constThis->child(row));
}

int _Layer::childCount() const
{
	return _children.count();
}

int _Layer::row() const
{
	if (_parent)
		return _parent->_children.indexOf(const_cast<_Layer*>(this));

	return 0;
}

const _Layer* _Layer::parent() const
{
	return _parent;
}

_Layer* _Layer::parent()
{
	const auto constThis = const_cast<const _Layer*>(this);
	return const_cast<_Layer*>(constThis->parent());
}

void _Layer::setParent(_Layer* parent)
{
	_parent = parent;
}

void _Layer::sortChildren()
{
	std::sort(_children.begin(), _children.end(), [] (auto a, auto b) {
		return a->order(Qt::EditRole) > b->order(Qt::EditRole);
	});
}

int _Layer::columnCount()
{
	return static_cast<int>(Column::End);
}

QVariant _Layer::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	if (orientation == Qt::Horizontal) {
		return columnName(static_cast<Column>(section));
	}

	return QVariant();
}

Qt::ItemFlags _Layer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

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
			if (flag(_Layer::Flag::Renamable, Qt::EditRole).toBool())
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

QVariant _Layer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		case Column::Enabled:
			return flag(_Layer::Flag::Enabled, role);

		case Column::Type:
			return type(role);

		case Column::Locked:
			return flag(_Layer::Flag::Frozen, role);

		case Column::ID:
			return id(role);

		case Column::Name:
			return name(role);

		case Column::Dataset:
			return dataset(role);

		case Column::Frozen:
			return flag(_Layer::Flag::Frozen, role);

		case Column::Removable:
			return flag(_Layer::Flag::Removable, role);

		case Column::Mask:
			return flag(_Layer::Flag::Mask, role);

		case Column::Renamable:
			return flag(_Layer::Flag::Renamable, role);

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

void _Layer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	const auto column = static_cast<Column>(index.column());

	if (role == Qt::CheckStateRole) {
		switch (column) {
			case Column::Enabled:
				setFlag(_Layer::Flag::Enabled, value == Qt::Checked ? true : false);
				break;

			default:
				break;
		}
	}

	if (role == Qt::DisplayRole) {
		switch (column) {
			case Column::Enabled:
				setFlag(_Layer::Flag::Enabled, value.toBool());
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
				setFlag(_Layer::Flag::Frozen, value.toBool());
				break;

			case Column::Removable:
				setFlag(_Layer::Flag::Removable, value.toBool());
				break;

			case Column::Mask:
				setFlag(_Layer::Flag::Mask, value.toBool());
				break;

			case Column::Renamable:
				setFlag(_Layer::Flag::Renamable, value.toBool());
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

bool _Layer::isSettingsIndex(const QModelIndex& index) const
{
	return index.parent().isValid() && index.parent().column() == static_cast<int>(_Layer::Column::Settings);
}

QVariant _Layer::id(const int& role) const
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

void _Layer::setId(const QString& id)
{
	_id = id;
}

QVariant _Layer::name(const int& role) const
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

void _Layer::setName(const QString& name)
{
	_name = name;
}

QVariant _Layer::dataset(const int& role) const
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

QVariant _Layer::type(const int& role) const
{
	const auto typeName = _Layer::typeName(_type);

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

void _Layer::setType(const Type& type)
{
	_type = type;
}

QVariant _Layer::flag(const _Layer::Flag& flag, const int& role) const
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
				case _Layer::Flag::Enabled:
					return QString("Enabled: %1").arg(flagString);

				case _Layer::Flag::Frozen:
					return QString("Frozen: %1").arg(flagString);

				case _Layer::Flag::Removable:
					return QString("Removable: %1").arg(flagString);

				case _Layer::Flag::Mask:
					return QString("Mask: %1").arg(flagString);

				case _Layer::Flag::Renamable:
					return QString("Renamable: %1").arg(flagString);

				default:
					break;
			}

			break;
		}

		case Qt::CheckStateRole:
		{
			if (flag == _Layer::Flag::Enabled)
				return isFlagSet ? Qt::Checked : Qt::Unchecked;

			break;
		}

		default:
			break;
	}

	return QVariant();
}

void _Layer::setFlag(const _Layer::Flag& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= static_cast<int>(flag);
	else
		_flags = _flags & ~static_cast<int>(flag);
}

void _Layer::setFlags(const int& flags)
{
	_flags = flags;
}

QVariant _Layer::order(const int& role) const
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

void _Layer::setOrder(const std::uint32_t& order)
{
	_order = order;
}

QVariant _Layer::opacity(const int& role) const
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

void _Layer::setOpacity(const float& opacity)
{
	_opacity = opacity;
}

QVariant _Layer::colorMap(const int& role) const
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

void _Layer::setColorMap(const QImage& colorMap)
{
	_colorMap = colorMap;
}

QVariant _Layer::image(const int& role) const
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

void _Layer::setImage(const QImage& image)
{
	_image = image;
	_windowNormalized = 1.0f;
	_levelNormalized = 0.5f;

	computeImageRange();
	computeDisplayRange();
}

QVariant _Layer::imageRange(const int& role) const
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

QVariant _Layer::displayRange(const int& role) const
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

QVariant _Layer::windowNormalized(const int& role) const
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

void _Layer::setWindowNormalized(const float& windowNormalized)
{
	_windowNormalized = windowNormalized;

	computeDisplayRange();
}

QVariant _Layer::levelNormalized(const int& role) const
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

void _Layer::setLevelNormalized(const float& levelNormalized)
{
	_levelNormalized = levelNormalized;

	computeDisplayRange();
}

QVariant _Layer::window(const int& role) const
{
	return _window;
}

void _Layer::setWindow(const float& window)
{
	_window = window;
}

QVariant _Layer::level(const int& role) const
{
	return _level;
}

void _Layer::setLevel(const float& level)
{
	_level = level;
}

void _Layer::computeImageRange()
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

void _Layer::computeDisplayRange()
{
	//qDebug() << "Compute display range";

	const auto maxWindow = _imageRange.length();

	_level = std::clamp(_imageRange.min() + (_levelNormalized * maxWindow), _imageRange.min(), _imageRange.max());
	_window = std::clamp(_windowNormalized * maxWindow, _imageRange.min(), _imageRange.max());

	_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _imageRange.min(), _imageRange.max()));
	_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _imageRange.min(), _imageRange.max()));
}


