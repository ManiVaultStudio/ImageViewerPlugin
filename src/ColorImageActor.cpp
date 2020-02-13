#include "ColorImageActor.h"
#include "ColorImageProp.h"
#include "Renderer.h"

#include <QMouseEvent>
#include <QOpenGLTexture>
#include <QDebug>

ColorImageActor::ColorImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_imageMin(),
	_imageMax(),
	_windowNormalized(),
	_levelNormalized(),
	_window(1.0f),
	_level(0.5f)
{
	_registeredEvents |= static_cast<int>(ActorEvent::MousePress);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseRelease);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseMove);

	addProp<ColorImageProp>("ColorImageProp");

	connect(propByName<ColorImageProp>("ColorImageProp"), &ColorImageProp::imageSizeChanged, this, &ColorImageActor::imageSizeChanged);
}

void ColorImageActor::setImage(QSharedPointer<QImage> image)
{
	qDebug() << "Set image for" << name();

	bindOpenGLContext();

	std::uint16_t* pixels = (std::uint16_t*)image->bits();

	const auto noPixels = image->width() * image->height();

	auto test = std::vector<std::uint16_t>(pixels, pixels + noPixels * 4);

	_imageMin = std::numeric_limits<std::uint16_t>::max();
	_imageMax = std::numeric_limits<std::uint16_t>::min();

	for (std::int32_t y = 0; y < image->height(); y++)
	{
		for (std::int32_t x = 0; x < image->width(); x++)
		{
			const auto pixelId = y * image->width() + x;

			for (int c = 0; c < 3; c++)
			{
				const auto channel = reinterpret_cast<std::uint16_t*>(image->bits())[pixelId * 4 + c];

				if (channel < _imageMin)
					_imageMin = channel;

				if (channel > _imageMax)
					_imageMax = channel;
			}
		}
	}

	const auto imageMin = static_cast<float>(_imageMin);
	const auto imageMax = static_cast<float>(_imageMax);

	propByName<ColorImageProp>("ColorImageProp")->setImage(image);

	resetWindowLevel();

	emit changed(this);
}

QSize ColorImageActor::imageSize() const
{
	return propByName<ColorImageProp>("ColorImageProp")->imageSize();
}

float ColorImageActor::windowNormalized() const
{
	return _windowNormalized;
}

float ColorImageActor::levelNormalized() const
{
	return _levelNormalized;
}

void ColorImageActor::setWindowLevel(const float& window, const float& level)
{
	if (window == _windowNormalized && level == _levelNormalized)
		return;

	_windowNormalized	= std::clamp(window, 0.01f, 1.0f);
	_levelNormalized	= std::clamp(level, 0.01f, 1.0f);

	const auto maxWindow = static_cast<float>(_imageMax - _imageMin);

	_level		= std::clamp(_imageMin + (_levelNormalized * maxWindow), static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	_window		= std::clamp(_windowNormalized * maxWindow, static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	
	const auto minPixelValue	= std::clamp(_level - (_window / 2.0f), static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	const auto maxPixelValue	= std::clamp(_level + (_window / 2.0f), static_cast<float>(_imageMin), static_cast<float>(_imageMax));

	auto quad = propByName<ColorImageProp>("ColorImageProp");

	quad->setMinPixelValue(minPixelValue);
	quad->setMaxPixelValue(maxPixelValue);

	qDebug() << "Set window/level" << _windowNormalized << _levelNormalized;

	emit windowLevelChanged(_window, _level);

	emit changed(this);
}

void ColorImageActor::resetWindowLevel()
{
	setWindowLevel(1.0f, 0.5f);
}

void ColorImageActor::onMousePressEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMousePressEvent())
		return;

	Actor::onMousePressEvent(mouseEvent);
	
	_mouseEvents.clear();

	addMouseEvent(mouseEvent);

	emit startWindowLevel();
}

void ColorImageActor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseReleaseEvent())
		return;

	Actor::onMouseReleaseEvent(mouseEvent);

	emit endWindowLevel();
}

void ColorImageActor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseMoveEvent())
		return;

	Actor::onMouseMoveEvent(mouseEvent);

	addMouseEvent(mouseEvent);

	if (_mouseEvents.size() > 1 && mouseEvent->buttons() & Qt::RightButton) {
		const auto pScreen0		= _mouseEvents[_mouseEvents.size() - 2].screenPoint();
		const auto pScreen1		= _mouseEvents.last().screenPoint();
		const auto deltaWindow	= (pScreen1.x() - pScreen0.x()) / 150.f;
		const auto deltaLevel	= -(pScreen1.y() - pScreen0.y()) / 150.f;
		const auto window		= std::clamp(_windowNormalized + deltaWindow, 0.0f, 1.0f);
		const auto level		= std::clamp(_levelNormalized + deltaLevel, 0.0f, 1.0f);
		
		setWindowLevel(window, level);
	}
}