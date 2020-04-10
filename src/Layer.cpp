#include "Layer.h"
#include "Dataset.h"
#include "LayerActor.h"

#include <QFont>
#include <QDebug>

Layer::Layer(Dataset* dataset, const Type& type, const QString& id, const QString& name, const int& flags) :
	TreeItem(id, name, flags),
	_dataset(dataset),
	_type(type),
	_opacity(1.0f),
	_colorMap(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(),
	_level(),
	_modelMatrix(),
	_props()
{
//	_actor = new LayerActor()
}

Layer::~Layer() = default;

int Layer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags Layer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

	const auto type = static_cast<Type>(_type);

	switch (static_cast<Column>(index.column())) {
		case Column::Name:
		{
			flags |= Qt::ItemIsUserCheckable;

			if (flag(Layer::Flag::Renamable, Qt::EditRole).toBool())
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
		case Column::Flags:
			break;

		default:
			break;
	}

	return flags;
}

QVariant Layer::data(const QModelIndex& index, const int& role) const
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

		case Column::Flags:
			return TreeItem::flags(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList Layer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIndices;

	affectedIndices.append(index);

	const auto column = static_cast<Column>(index.column());

	switch (role)
	{
		case Qt::CheckStateRole:
		{
			switch (column) {
				case Column::Name:
				{
					setFlag(Layer::Flag::Enabled, value.toBool());

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

bool Layer::isBaseLayerIndex(const QModelIndex& index) const
{
	return index.column() <= ult(Layer::Column::End);
}

QVariant Layer::dataset(const int& role) const
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