#include "LayerImage.h"

#include <QDebug>

LayerImage::LayerImage(QObject* parent, const float& window /*= 1.0f*/, const float& level /*= 0.5f*/) :
	QObject(parent),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(window),
	_level(level)
{
}

QImage LayerImage::image() const
{
	return _image;
}

void LayerImage::setImage(const QImage& image)
{
	_image = image;

	_windowNormalized	= 1.0f;
	_levelNormalized	= 0.5f;

	computeImageRange();
	computeDisplayRange();
}

LayerImage::Range LayerImage::imageRange() const
{
	return _imageRange;
}

LayerImage::Range LayerImage::displayRange() const
{
	return _displayRange;
}

float LayerImage::windowNormalized() const
{
	return _windowNormalized;
}

void LayerImage::setWindowNormalized(const float& windowNormalized)
{
	_windowNormalized = windowNormalized;

	computeDisplayRange();
}

float LayerImage::levelNormalized() const
{
	return _levelNormalized;
}

void LayerImage::setLevelNormalized(const float& levelNormalized)
{
	_levelNormalized = levelNormalized;

	computeDisplayRange();
}

float LayerImage::window() const
{
	return _window;
}

void LayerImage::setWindow(const float& window)
{
	_window = window;
}

float LayerImage::level() const
{
	return _level;
}

void LayerImage::setLevel(const float& level)
{
	_level = level;
}

void LayerImage::computeImageRange()
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

void LayerImage::computeDisplayRange()
{
	const auto maxWindow = _imageRange.length();

	_level		= std::clamp(_imageRange.min() + (_levelNormalized * maxWindow), _imageRange.min(), _imageRange.max());
	_window		= std::clamp(_windowNormalized * maxWindow, _imageRange.min(), _imageRange.max());
	
	_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _imageRange.min(), _imageRange.max()));
	_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _imageRange.min(), _imageRange.max()));
}