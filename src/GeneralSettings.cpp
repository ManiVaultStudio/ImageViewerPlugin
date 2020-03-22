#include "GeneralSettings.h"

#include <QFont>
#include <QDebug>

GeneralSettings::GeneralSettings(Dataset* dataset, const QString& id, const QString& name, const LayerType& type, const std::uint32_t& flags) :
	Settings(dataset),
	_id(id),
	_name(name),
	_type(type),
	_flags(flags),
	_order(0),
	_opacity(0.0f),
	_colorMap(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(0.5f)
{
}

Qt::ItemFlags GeneralSettings::itemFlags(const LayerColumn& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	const auto type = static_cast<LayerType>(_type);

	switch (static_cast<LayerColumn>(column)) {
		case LayerColumn::Enabled:
			flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
			break;

		case LayerColumn::Type:
		case LayerColumn::Locked:
		case LayerColumn::ID:
			break;

		case LayerColumn::Name:
		{
			if (flag(LayerFlag::Renamable, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case LayerColumn::Dataset:
		case LayerColumn::Flags:
		case LayerColumn::Frozen:
		case LayerColumn::Removable:
			break;

		case LayerColumn::Mask:
		{
			if (type == LayerType::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case LayerColumn::Order:
			break;

		case LayerColumn::Opacity:
			flags |= Qt::ItemIsEditable;
			break;

		case LayerColumn::WindowNormalized:
		{
			if (type == LayerType::Images)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case LayerColumn::LevelNormalized:
		{
			if (type == LayerType::Images)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case LayerColumn::ColorMap:
		{
			if (type == LayerType::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case LayerColumn::Image:
			break;

		case LayerColumn::ImageRange:
		case LayerColumn::DisplayRange:
			break;

		default:
			break;
	}

	return flags;
}

QVariant GeneralSettings::data(const LayerColumn& column, int role) const
{
	switch (role)
	{
		case Qt::FontRole:
		{
			switch (static_cast<LayerColumn>(column)) {
				case LayerColumn::Type:
					return type(Qt::FontRole).toString();

				default:
					break;
			}

			break;
		}

		case Qt::CheckStateRole:
		{
			switch (static_cast<LayerColumn>(column)) {
				case LayerColumn::Enabled:
					return flag(LayerFlag::Enabled, Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;
			}

			break;
		}

		case Qt::DisplayRole:
		{
			switch (static_cast<LayerColumn>(column)) {
				case LayerColumn::Enabled:
					break;

				case LayerColumn::Type:
					return type(Roles::FontIconText).toString();

				case LayerColumn::Locked:
					return flag(LayerFlag::Frozen, Qt::EditRole).toBool() ? u8"\uf023" : u8"\uf09c";

				case LayerColumn::ID:
					return id(Qt::DisplayRole);

				case LayerColumn::Name:
					return name(Qt::DisplayRole);

				case LayerColumn::Dataset:
					return dataset(Qt::DisplayRole);

				case LayerColumn::Flags:
					return flags(Qt::DisplayRole);

				case LayerColumn::Frozen:
					return flag(LayerFlag::Frozen, Qt::DisplayRole);

				case LayerColumn::Removable:
					return flag(LayerFlag::Removable, Qt::DisplayRole);

				case LayerColumn::Mask:
					return flag(LayerFlag::Mask, Qt::DisplayRole);

				case LayerColumn::Renamable:
					return flag(LayerFlag::Renamable, Qt::DisplayRole);

				case LayerColumn::Order:
					return order(Qt::DisplayRole);

				case LayerColumn::Opacity:
					return opacity(Qt::DisplayRole);

				case LayerColumn::WindowNormalized:
					return windowNormalized(Qt::DisplayRole);

				case LayerColumn::LevelNormalized:
					return levelNormalized(Qt::DisplayRole);

				case LayerColumn::ColorMap:
					return colorMap(Qt::DisplayRole);

				case LayerColumn::Image:
					return image(Qt::DisplayRole);

				case LayerColumn::ImageRange:
					return imageRange(Qt::DisplayRole);

				case LayerColumn::DisplayRange:
					return displayRange(Qt::DisplayRole);

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (static_cast<LayerColumn>(column)) {
				case LayerColumn::Enabled:
					return flag(LayerFlag::Enabled, Qt::EditRole);

				case LayerColumn::Type:
					return type(Qt::EditRole);

				case LayerColumn::Locked:
					return flag(LayerFlag::Frozen, Qt::EditRole);

				case LayerColumn::ID:
					return id(Qt::EditRole);

				case LayerColumn::Name:
					return name(Qt::EditRole);

				case LayerColumn::Dataset:
					return dataset(Qt::EditRole);

				case LayerColumn::Flags:
					return flags(Qt::EditRole);

				case LayerColumn::Frozen:
					return flag(LayerFlag::Frozen, Qt::EditRole);

				case LayerColumn::Removable:
					return flag(LayerFlag::Removable, Qt::EditRole);

				case LayerColumn::Mask:
					return flag(LayerFlag::Mask, Qt::EditRole);

				case LayerColumn::Renamable:
					return flag(LayerFlag::Renamable, Qt::EditRole);

				case LayerColumn::Order:
					return order(Qt::EditRole);

				case LayerColumn::Opacity:
					return opacity(Qt::EditRole);

				case LayerColumn::WindowNormalized:
					return windowNormalized(Qt::EditRole);

				case LayerColumn::LevelNormalized:
					return levelNormalized(Qt::EditRole);

				case LayerColumn::ColorMap:
					return colorMap(Qt::EditRole);

				case LayerColumn::Image:
					return image(Qt::EditRole);

				case LayerColumn::ImageRange:
					return imageRange(Qt::EditRole);

				case LayerColumn::DisplayRange:
					return displayRange(Qt::EditRole);

				default:
					break;
			}

			break;
		}

		case Qt::ToolTipRole:
		{
			switch (static_cast<LayerColumn>(column)) {
				case LayerColumn::Enabled:
					return flag(LayerFlag::Enabled, Qt::ToolTipRole);

				case LayerColumn::Type:
					return type(Qt::ToolTipRole);

				case LayerColumn::Locked:
					return flag(LayerFlag::Frozen, Qt::ToolTipRole);

				case LayerColumn::ID:
					return name(Qt::ToolTipRole);

				case LayerColumn::Name:
					return name(Qt::ToolTipRole);

				case LayerColumn::Dataset:
					return dataset(Qt::ToolTipRole);

				case LayerColumn::Flags:
					return flags(Qt::ToolTipRole);

				case LayerColumn::Frozen:
					return flag(LayerFlag::Frozen, Qt::ToolTipRole);

				case LayerColumn::Removable:
					return flag(LayerFlag::Removable, Qt::ToolTipRole);

				case LayerColumn::Mask:
					return flag(LayerFlag::Mask, Qt::ToolTipRole);

				case LayerColumn::Renamable:
					return flag(LayerFlag::Renamable, Qt::ToolTipRole);

				case LayerColumn::Order:
					return order(Qt::ToolTipRole);

				case LayerColumn::Opacity:
					return opacity(Qt::ToolTipRole);

				case LayerColumn::WindowNormalized:
					return windowNormalized(Qt::ToolTipRole);

				case LayerColumn::LevelNormalized:
					return levelNormalized(Qt::ToolTipRole);

				case LayerColumn::ColorMap:
					return colorMap(Qt::ToolTipRole);

				case LayerColumn::Image:
					return image(Qt::ToolTipRole);

				case LayerColumn::ImageRange:
					return imageRange(Qt::ToolTipRole);

				case LayerColumn::DisplayRange:
					return displayRange(Qt::ToolTipRole);

				default:
					break;
			}

			break;
		}

		case Qt::TextAlignmentRole:
		{
			switch (static_cast<LayerColumn>(column)) {
				case LayerColumn::Enabled:
				case LayerColumn::Type:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case LayerColumn::Locked:
				case LayerColumn::ID:
				case LayerColumn::Name:
				case LayerColumn::Dataset:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case LayerColumn::Flags:
				case LayerColumn::Frozen:
				case LayerColumn::Removable:
				case LayerColumn::Mask:
				case LayerColumn::Renamable:
				case LayerColumn::Order:
				case LayerColumn::Opacity:
				case LayerColumn::WindowNormalized:
				case LayerColumn::LevelNormalized:
				case LayerColumn::ColorMap:
				case LayerColumn::Image:
					return Qt::AlignRight + Qt::AlignVCenter;

				case LayerColumn::ImageRange:
				case LayerColumn::DisplayRange:
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

void GeneralSettings::setData(const LayerColumn& column, const QVariant& value, const int& role)
{
	if (role == Qt::CheckStateRole) {
		switch (static_cast<LayerColumn>(column)) {
			case LayerColumn::Enabled:
				setFlag(LayerFlag::Enabled, value == Qt::Checked ? true : false);
				break;

			default:
				break;
		}
	}

	if (role == Qt::DisplayRole) {
		switch (static_cast<LayerColumn>(column)) {
			case LayerColumn::Enabled:
				setFlag(LayerFlag::Enabled, value.toBool());
				break;

			case LayerColumn::Type:
				setType(static_cast<LayerType>(value.toInt()));
				break;

			case LayerColumn::Locked:
				break;

			case LayerColumn::ID:
				setId(value.toString());
				break;

			case LayerColumn::Name:
				setName(value.toString());
				break;

			case LayerColumn::Dataset:
				break;

			case LayerColumn::Flags:
				setFlags(value.toInt());
				break;

			case LayerColumn::Frozen:
				setFlag(LayerFlag::Frozen, value.toBool());
				break;

			case LayerColumn::Removable:
				setFlag(LayerFlag::Removable, value.toBool());
				break;

			case LayerColumn::Mask:
				setFlag(LayerFlag::Mask, value.toBool());
				break;

			case LayerColumn::Renamable:
				setFlag(LayerFlag::Renamable, value.toBool());
				break;

			case LayerColumn::Order:
				setOrder(value.toInt());
				break;

			case LayerColumn::Opacity:
				setOpacity(value.toFloat());
				break;

			case LayerColumn::WindowNormalized:
				setWindowNormalized(value.toFloat());
				break;

			case LayerColumn::LevelNormalized:
				setLevelNormalized(value.toFloat());
				break;

			case LayerColumn::ColorMap:
				setColorMap(value.value<QImage>());
				break;

			case LayerColumn::Image:
				setImage(value.value<QImage>());
				break;

			case LayerColumn::ImageRange:
			case LayerColumn::DisplayRange:
				break;

			default:
				break;
		}
	}
}

QVariant GeneralSettings::id(const int& role) const
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

void GeneralSettings::setId(const QString& id)
{
	_id = id;
}

QVariant GeneralSettings::name(const int& role) const
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

void GeneralSettings::setName(const QString& name)
{
	_name = name;
}

QVariant GeneralSettings::dataset(const int& role) const
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

QVariant GeneralSettings::type(const int& role) const
{
	const auto typeName = layerTypeName(_type);

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
				case LayerType::Images:
					return u8"\uf03e";

				case LayerType::Selection:
					return u8"\uf065";

				case LayerType::Clusters:
					return u8"\uf141";

				case LayerType::Points:
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

void GeneralSettings::setType(const LayerType& type)
{
	_type = type;
}

QVariant GeneralSettings::flags(const int& role) const
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

QVariant GeneralSettings::flag(const LayerFlag& flag, const int& role) const
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
				case LayerFlag::Enabled:
					return QString("Enabled: %1").arg(flagString);

				case LayerFlag::Frozen:
					return QString("Frozen: %1").arg(flagString);

				case LayerFlag::Removable:
					return QString("Removable: %1").arg(flagString);

				case LayerFlag::Mask:
					return QString("Mask: %1").arg(flagString);

				case LayerFlag::Renamable:
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

void GeneralSettings::setFlag(const LayerFlag& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= static_cast<int>(flag);
	else
		_flags = _flags & ~static_cast<int>(flag);
}

void GeneralSettings::setFlags(const std::uint32_t& flags)
{
	_flags = flags;
}

QVariant GeneralSettings::order(const int& role) const
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

void GeneralSettings::setOrder(const std::uint32_t& order)
{
	_order = order;
}

QVariant GeneralSettings::opacity(const int& role) const
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

void GeneralSettings::setOpacity(const float& opacity)
{
	_opacity = opacity;
}

QVariant GeneralSettings::colorMap(const int& role) const
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

void GeneralSettings::setColorMap(const QImage& colorMap)
{
	_colorMap = colorMap;
}

QVariant GeneralSettings::image(const int& role) const
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

void GeneralSettings::setImage(const QImage& image)
{
	_image = image;
	_windowNormalized = 1.0f;
	_levelNormalized = 0.5f;

	computeImageRange();
	computeDisplayRange();
}

QVariant GeneralSettings::imageRange(const int& role) const
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

QVariant GeneralSettings::displayRange(const int& role) const
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

QVariant GeneralSettings::windowNormalized(const int& role) const
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

void GeneralSettings::setWindowNormalized(const float& windowNormalized)
{
	_windowNormalized = windowNormalized;

	computeDisplayRange();
}

QVariant GeneralSettings::levelNormalized(const int& role) const
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

void GeneralSettings::setLevelNormalized(const float& levelNormalized)
{
	_levelNormalized = levelNormalized;

	computeDisplayRange();
}

QVariant GeneralSettings::window(const int& role) const
{
	return _window;
}

void GeneralSettings::setWindow(const float& window)
{
	_window = window;
}

QVariant GeneralSettings::level(const int& role) const
{
	return _level;
}

void GeneralSettings::setLevel(const float& level)
{
	_level = level;
}

void GeneralSettings::computeImageRange()
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

void GeneralSettings::computeDisplayRange()
{
	//qDebug() << "Compute display range";

	const auto maxWindow = _imageRange.length();

	_level = std::clamp(_imageRange.min() + (_levelNormalized * maxWindow), _imageRange.min(), _imageRange.max());
	_window = std::clamp(_windowNormalized * maxWindow, _imageRange.min(), _imageRange.max());

	_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _imageRange.min(), _imageRange.max()));
	_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _imageRange.min(), _imageRange.max()));
}