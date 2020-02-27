#include "ImageLayer.h"
#include "ImageDataset.h"

#include <QDebug>

ImageLayer::ImageLayer(ImageDataset* imageDataset, const QString& name) :
	QObject(imageDataset),
	_name(name),
	_order(0),
	_opacity(1.0f),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(0.5f)
{
}

ImageLayer::~ImageLayer() = default;

const ImageDataset* ImageLayer::imageDataset() const
{
	return dynamic_cast<ImageDataset*>(parent());
}

ImageDataset* ImageLayer::imageDataset()
{
	const auto constThis = const_cast<const ImageLayer*>(this);
	return const_cast<ImageDataset*>(constThis->imageDataset());
}

QString ImageLayer::name() const
{
	return _name;
}

void ImageLayer::setName(const QString& name)
{
	if (name == _name)
		return;

	const auto oldName = fullName();

	_name = name;

	qDebug() << oldName << "rename to" << fullName();

	emit nameChanged(_name);
}

QString ImageLayer::fullName() const
{
	return QString("%1::%2").arg(imageDataset()->name(), _name);
}

std::uint32_t ImageLayer::order() const
{
	return _order;
}

void ImageLayer::setOrder(const std::uint32_t& order)
{
	if (order == _order)
		return;

	_order = order;

	qDebug() << fullName() << "set order to" << _order;

	emit orderChanged(_order);
}

float ImageLayer::opacity() const
{
	return _opacity;
}

void ImageLayer::setOpacity(const float& opacity)
{
	if (opacity == _opacity)
		return;

	_opacity = opacity;

	qDebug() << fullName() << "set opacity to" << QString::number(_opacity, 'f', 2);

	emit opacityChanged(_opacity);
}

const QImage ImageLayer::image() const
{
	return _image;
}

void ImageLayer::setImage(const QImage& image)
{
	if (image == _image)
		return;

	_image = image;

	qDebug() << fullName() << "set image";

	computeImageRange();
	computeDisplayRange();

	emit imageChanged(_image);
}

QPair<float, float> ImageLayer::imageRange() const
{
	return _imageRange;
}

QPair<float, float> ImageLayer::displayRange() const
{
	return _displayRange;
}

float ImageLayer::windowNormalized() const
{
	return _windowNormalized;
}

float ImageLayer::window() const
{
	return _window;
}

float ImageLayer::levelNormalized() const
{
	return _levelNormalized;
}

float ImageLayer::level() const
{
	return _level;
}

void ImageLayer::setWindowLevel(const float& window, const float& level)
{
	if (window == _windowNormalized && level == _levelNormalized)
		return;

	qDebug() << fullName() << "set window/level" << QString::number(window, 'f', 2) << QString::number(level, 'f', 2);

	_windowNormalized	= std::clamp(window, 0.01f, 1.0f);
	_levelNormalized	= std::clamp(level, 0.01f, 1.0f);

	computeDisplayRange();
}

void ImageLayer::computeImageRange()
{
	if (_image.isNull())
		return;

	qDebug() << fullName() << "compute image range";

	auto imageBits = reinterpret_cast<ushort*>(const_cast<uchar*>(_image.bits()));

	const auto noPixels = _image.width() * _image.height();

	_imageRange.first = std::numeric_limits<float>::max();
	_imageRange.second = std::numeric_limits<float>::min();

	for (std::int32_t y = 0; y < _image.height(); y++)
	{
		for (std::int32_t x = 0; x < _image.width(); x++)
		{
			const auto pixelId = y * _image.width() + x;

			for (int c = 0; c < 3; c++)
			{
				const auto channel = static_cast<float>(imageBits[pixelId * 4 + c]);

				if (channel < _imageRange.first)
					_imageRange.first = channel;

				if (channel > _imageRange.second)
					_imageRange.second = channel;
			}
		}
	}

	emit imageRangeChanged(_displayRange.first, _displayRange.second);
}

void ImageLayer::computeDisplayRange()
{
	const auto maxWindow = _imageRange.second - _imageRange.first;

	_level					= std::clamp(_imageRange.first + (_levelNormalized * maxWindow), _imageRange.first, _imageRange.second);
	_window					= std::clamp(_windowNormalized * maxWindow, _imageRange.first, _imageRange.second);
	_displayRange.first		= std::clamp(_level - (_window / 2.0f), _imageRange.first, _imageRange.second);
	_displayRange.second	= std::clamp(_level + (_window / 2.0f), _imageRange.first, _imageRange.second);

	emit displayRangeChanged(_displayRange.first, _displayRange.second);
}