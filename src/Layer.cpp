#include "Layer.h"

#include <QFont>
#include <QDebug>

Layer::Layer(QObject* parent) :
	QObject(parent),
	_id(),
	_name(),
	_type(Type::Image),
	_flags(0),
	_order(0),
	_opacity(1.0f),
	_color(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(0.5f)
{
}

Layer::Layer(QObject* parent, const QString& id, const QString& name, const Type& type, const std::uint32_t& flags, const std::uint32_t& order, const float& opacity /*= 1.0f*/, const float& window /*= 1.0f*/, const float& level /*= 0.5f*/) :
	QObject(parent),
	_id(id),
	_name(name),
	_type(type),
	_flags(flags),
	_order(order),
	_opacity(opacity),
	_color(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(0.5f)
{
}

QVariant Layer::id(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setId(const QString& id)
{
	_id = id;
}

QVariant Layer::name(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setName(const QString& name)
{
	_name = name;
}

QVariant Layer::type(const int& role /*= Qt::DisplayRole*/) const
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
				case Layer::Type::Image:
					return u8"\uf03e";

				case Layer::Type::Selection:
					return u8"\uf065";

				case Layer::Type::MetaData:
					return u8"\uf141";

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QString();
}

void Layer::setType(const Type& type)
{
	_type = type;
}

QVariant Layer::flag(const std::uint32_t& flag, const int& role /*= Qt::DisplayRole*/) const
{
	const auto isFlagSet	= _flags & flag;
	const auto flagString	= isFlagSet ? "true" : "false";

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
				case Enabled:
					return QString("Enabled: %1").arg(flagString);

				case Frozen:
					return QString("Frozen: %1").arg(flagString);

				case Removable:
					return QString("Removable: %1").arg(flagString);

				case Mask:
					return QString("Mask: %1").arg(flagString);

				case Renamable:
					return QString("Renamable: %1").arg(flagString);

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QString();
}

void Layer::setFlag(const std::uint32_t& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= flag;
	else
		_flags = _flags & ~flag;
}

QVariant Layer::order(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setOrder(const std::uint32_t& order)
{
	_order = order;
}

QVariant Layer::opacity(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setOpacity(const float& opacity)
{
	_opacity = opacity;
}

QVariant Layer::color(const int& role /*= Qt::DisplayRole*/) const
{
	const auto colorString = _color.name();

	switch (role)
	{
		case Qt::DisplayRole:
			return colorString;

		case Qt::EditRole:
			return _color;

		case Qt::ToolTipRole:
			return QString("Color: %1").arg(colorString);

		default:
			break;
	}

	return QString();
}

void Layer::setColor(const QColor& color)
{
	_color = color;
}

QVariant Layer::image(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setImage(const QImage& image)
{
	_image				= image;
	_windowNormalized	= 1.0f;
	_levelNormalized	= 0.5f;

	computeImageRange();
	computeDisplayRange();
}

QVariant Layer::imageRange(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

QVariant Layer::displayRange(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

QVariant Layer::windowNormalized(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setWindowNormalized(const float& windowNormalized)
{
	_windowNormalized = windowNormalized;

	computeDisplayRange();
}

QVariant Layer::levelNormalized(const int& role /*= Qt::DisplayRole*/) const
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

	return QString();
}

void Layer::setLevelNormalized(const float& levelNormalized)
{
	_levelNormalized = levelNormalized;

	computeDisplayRange();
}

QVariant Layer::window(const int& role /*= Qt::DisplayRole*/) const
{
	return _window;
}

void Layer::setWindow(const float& window)
{
	_window = window;
}

QVariant Layer::level(const int& role /*= Qt::DisplayRole*/) const
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

	qDebug() << "Compute image range";

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
	const auto maxWindow = _imageRange.length();

	_level = std::clamp(_imageRange.min() + (_levelNormalized * maxWindow), _imageRange.min(), _imageRange.max());
	_window = std::clamp(_windowNormalized * maxWindow, _imageRange.min(), _imageRange.max());

	_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _imageRange.min(), _imageRange.max()));
	_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _imageRange.min(), _imageRange.max()));
}