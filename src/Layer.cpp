#include "Layer.h"
#include "Dataset.h"

#include "Layer.h"
#include "PointsSettings.h"
#include "ImagesSettings.h"
#include "ClustersSettings.h"

#include <QFont>
#include <QDebug>

Layer::Layer(Dataset* dataset, const QString& id /*= ""*/, const QString& name /*= ""*/, const Type& type /*= Type::Image*/, const std::uint32_t& flags) :
	QObject(dataset),
	_dataset(dataset),
	_id(id),
	_name(name),
	_type(type),
	_flags(0),
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

int Layer::columnCount()
{
	return static_cast<int>(Layer::Column::DisplayRange);
}

QVariant Layer::headerData(int section, Qt::Orientation orientation, int role)
{
	if (orientation == Qt::Horizontal) {
		return columnName(static_cast<Column>(section));
	}

	return QVariant();
}

Qt::ItemFlags Layer::itemFlags(const Column& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	const auto type = static_cast<Layer::Type>(_type);

	switch (static_cast<Layer::Column>(column)) {
		case Layer::Column::Enabled:
			flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
			break;

		case Layer::Column::Type:
		case Layer::Column::Locked:
		case Layer::Column::ID:
			break;

		case Layer::Column::Name:
		{
			if (flag(Layer::Flag::Renamable, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Layer::Column::Dataset:
		case Layer::Column::Flags:
		case Layer::Column::Frozen:
		case Layer::Column::Removable:
			break;

		case Layer::Column::Mask:
		{
			if (type == Layer::Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Layer::Column::Order:
			break;

		case Layer::Column::Opacity:
			flags |= Qt::ItemIsEditable;
			break;

		case Layer::Column::WindowNormalized:
		{
			if (type == Layer::Type::Images)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Layer::Column::LevelNormalized:
		{
			if (type == Layer::Type::Images)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Layer::Column::ColorMap:
		{
			if (type == Layer::Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Layer::Column::Image:
			break;

		case Layer::Column::ImageRange:
		case Layer::Column::DisplayRange:
			break;

		default:
			break;
	}

	return flags;
}

QVariant Layer::data(const Column& column, int role) const
{
	switch (role)
	{
		case Qt::FontRole:
		{
			switch (static_cast<Layer::Column>(column)) {
				case Layer::Column::Type:
					return type(Qt::FontRole).toString();

				default:
					break;
			}

			break;
		}

		case Qt::CheckStateRole:
		{
			switch (static_cast<Layer::Column>(column)) {
				case Layer::Column::Enabled:
					return flag(Layer::Flag::Enabled, Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;
			}

			break;
		}

		case Qt::DisplayRole:
		{
			switch (static_cast<Layer::Column>(column)) {
				case Layer::Column::Enabled:
					break;

				case Layer::Column::Type:
					return type(Roles::FontIconText).toString();

				case Layer::Column::Locked:
					return flag(Layer::Flag::Frozen, Qt::EditRole).toBool() ? u8"\uf023" : u8"\uf09c";

				case Layer::Column::ID:
					return id(Qt::DisplayRole);

				case Layer::Column::Name:
					return name(Qt::DisplayRole);

				case Layer::Column::Dataset:
					return dataset(Qt::DisplayRole);

				case Layer::Column::Flags:
					return flags(Qt::DisplayRole);

				case Layer::Column::Frozen:
					return flag(Layer::Flag::Frozen, Qt::DisplayRole);

				case Layer::Column::Removable:
					return flag(Layer::Flag::Removable, Qt::DisplayRole);

				case Layer::Column::Mask:
					return flag(Layer::Flag::Mask, Qt::DisplayRole);

				case Layer::Column::Renamable:
					return flag(Layer::Flag::Renamable, Qt::DisplayRole);

				case Layer::Column::Order:
					return order(Qt::DisplayRole);

				case Layer::Column::Opacity:
					return opacity(Qt::DisplayRole);

				case Layer::Column::WindowNormalized:
					return windowNormalized(Qt::DisplayRole);

				case Layer::Column::LevelNormalized:
					return levelNormalized(Qt::DisplayRole);

				case Layer::Column::ColorMap:
					return colorMap(Qt::DisplayRole);

				case Layer::Column::Image:
					return image(Qt::DisplayRole);

				case Layer::Column::ImageRange:
					return imageRange(Qt::DisplayRole);

				case Layer::Column::DisplayRange:
					return displayRange(Qt::DisplayRole);

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (static_cast<Layer::Column>(column)) {
				case Layer::Column::Enabled:
					return flag(Layer::Flag::Enabled, Qt::EditRole);

				case Layer::Column::Type:
					return type(Qt::EditRole);

				case Layer::Column::Locked:
					return flag(Layer::Flag::Frozen, Qt::EditRole);

				case Layer::Column::ID:
					return id(Qt::EditRole);

				case Layer::Column::Name:
					return name(Qt::EditRole);

				case Layer::Column::Dataset:
					return dataset(Qt::EditRole);

				case Layer::Column::Flags:
					return flags(Qt::EditRole);

				case Layer::Column::Frozen:
					return flag(Layer::Flag::Frozen, Qt::EditRole);

				case Layer::Column::Removable:
					return flag(Layer::Flag::Removable, Qt::EditRole);

				case Layer::Column::Mask:
					return flag(Layer::Flag::Mask, Qt::EditRole);

				case Layer::Column::Renamable:
					return flag(Layer::Flag::Renamable, Qt::EditRole);

				case Layer::Column::Order:
					return order(Qt::EditRole);

				case Layer::Column::Opacity:
					return opacity(Qt::EditRole);

				case Layer::Column::WindowNormalized:
					return windowNormalized(Qt::EditRole);

				case Layer::Column::LevelNormalized:
					return levelNormalized(Qt::EditRole);

				case Layer::Column::ColorMap:
					return colorMap(Qt::EditRole);

				case Layer::Column::Image:
					return image(Qt::EditRole);

				case Layer::Column::ImageRange:
					return imageRange(Qt::EditRole);

				case Layer::Column::DisplayRange:
					return displayRange(Qt::EditRole);

				default:
					break;
			}

			break;
		}

		case Qt::ToolTipRole:
		{
			switch (static_cast<Layer::Column>(column)) {
				case Layer::Column::Enabled:
					return flag(Layer::Flag::Enabled, Qt::ToolTipRole);

				case Layer::Column::Type:
					return type(Qt::ToolTipRole);

				case Layer::Column::Locked:
					return flag(Layer::Flag::Frozen, Qt::ToolTipRole);

				case Layer::Column::ID:
					return name(Qt::ToolTipRole);

				case Layer::Column::Name:
					return name(Qt::ToolTipRole);

				case Layer::Column::Dataset:
					return dataset(Qt::ToolTipRole);

				case Layer::Column::Flags:
					return flags(Qt::ToolTipRole);

				case Layer::Column::Frozen:
					return flag(Layer::Flag::Frozen, Qt::ToolTipRole);

				case Layer::Column::Removable:
					return flag(Layer::Flag::Removable, Qt::ToolTipRole);

				case Layer::Column::Mask:
					return flag(Layer::Flag::Mask, Qt::ToolTipRole);

				case Layer::Column::Renamable:
					return flag(Layer::Flag::Renamable, Qt::ToolTipRole);

				case Layer::Column::Order:
					return order(Qt::ToolTipRole);

				case Layer::Column::Opacity:
					return opacity(Qt::ToolTipRole);

				case Layer::Column::WindowNormalized:
					return windowNormalized(Qt::ToolTipRole);

				case Layer::Column::LevelNormalized:
					return levelNormalized(Qt::ToolTipRole);

				case Layer::Column::ColorMap:
					return colorMap(Qt::ToolTipRole);

				case Layer::Column::Image:
					return image(Qt::ToolTipRole);

				case Layer::Column::ImageRange:
					return imageRange(Qt::ToolTipRole);

				case Layer::Column::DisplayRange:
					return displayRange(Qt::ToolTipRole);

				default:
					break;
			}

			break;
		}

		case Qt::TextAlignmentRole:
		{
			switch (static_cast<Layer::Column>(column)) {
				case Layer::Column::Enabled:
				case Layer::Column::Type:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case Layer::Column::Locked:
				case Layer::Column::ID:
				case Layer::Column::Name:
				case Layer::Column::Dataset:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case Layer::Column::Flags:
				case Layer::Column::Frozen:
				case Layer::Column::Removable:
				case Layer::Column::Mask:
				case Layer::Column::Renamable:
				case Layer::Column::Order:
				case Layer::Column::Opacity:
				case Layer::Column::WindowNormalized:
				case Layer::Column::LevelNormalized:
				case Layer::Column::ColorMap:
				case Layer::Column::Image:
					return Qt::AlignRight + Qt::AlignVCenter;

				case Layer::Column::ImageRange:
				case Layer::Column::DisplayRange:
					return Qt::AlignRight + Qt::AlignVCenter;

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

void Layer::setData(const Column& column, const QVariant& value, const int& role)
{
	if (role == Qt::CheckStateRole) {
		switch (static_cast<Layer::Column>(column)) {
			case Layer::Column::Enabled:
				setFlag(Layer::Flag::Enabled, value == Qt::Checked ? true : false);
				break;

			default:
				break;
		}
	}

	if (role == Qt::DisplayRole) {
		switch (static_cast<Layer::Column>(column)) {
			case Layer::Column::Enabled:
				setFlag(Layer::Flag::Enabled, value.toBool());
				break;

			case Layer::Column::Type:
				setType(static_cast<Layer::Type>(value.toInt()));
				break;

			case Layer::Column::Locked:
				break;

			case Layer::Column::ID:
				setId(value.toString());
				break;

			case Layer::Column::Name:
				setName(value.toString());
				break;

			case Layer::Column::Dataset:
				break;

			case Layer::Column::Flags:
				setFlags(value.toInt());
				break;

			case Layer::Column::Frozen:
				setFlag(Layer::Flag::Frozen, value.toBool());
				break;

			case Layer::Column::Removable:
				setFlag(Layer::Flag::Removable, value.toBool());
				break;

			case Layer::Column::Mask:
				setFlag(Layer::Flag::Mask, value.toBool());
				break;

			case Layer::Column::Renamable:
				setFlag(Layer::Flag::Renamable, value.toBool());
				break;

			case Layer::Column::Order:
				setOrder(value.toInt());
				break;

			case Layer::Column::Opacity:
				setOpacity(value.toFloat());
				break;

			case Layer::Column::WindowNormalized:
				setWindowNormalized(value.toFloat());
				break;

			case Layer::Column::LevelNormalized:
				setLevelNormalized(value.toFloat());
				break;

			case Layer::Column::ColorMap:
				setColorMap(value.value<QImage>());
				break;

			case Layer::Column::Image:
				setImage(value.value<QImage>());
				break;

			case Layer::Column::ImageRange:
			case Layer::Column::DisplayRange:
				break;

			default:
				break;
		}
	}
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

		case Qt::DisplayRole:
			return typeName;

		case Qt::EditRole:
			return static_cast<int>(_type);

		case Qt::ToolTipRole:
			return QString("Type: %1").arg(typeName);

		case Roles::FontIconText:
		{
			switch (_type) {
				case Layer::Type::Images:
					return u8"\uf03e";

				case Layer::Type::Selection:
					return u8"\uf065";

				case Layer::Type::Clusters:
					return u8"\uf141";

				case Layer::Type::Points:
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

void Layer::setType(const Layer::Type& type)
{
	_type = type;
}

QVariant Layer::flags(const int& role) const
{
	const auto flagsString = QString::number(_flags);

	switch (role)
	{
		case Qt::DisplayRole:
			return flagsString;

		case Qt::EditRole:
			return _flags;

		case Qt::ToolTipRole:
			return QString("Flags: %1").arg(flagsString);

		default:
			break;
	}

	return QVariant();
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

void Layer::setFlags(const std::uint32_t& flags)
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


