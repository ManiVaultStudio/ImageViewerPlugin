#include "ColorImageActor.h"

#include <QMouseEvent>
#include <QOpenGLTexture>
#include <QDebug>

#include "Renderer.h"

#include "ColorImage.h"

ColorImageActor::ColorImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_imageMin(),
	_imageMax(),
	_mousePositions(),
	_windowNormalized(),
	_levelNormalized(),
	_window(1.0f),
	_level(0.5f)
{
	_registeredEvents |= static_cast<int>(ActorEvent::MousePress);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseRelease);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseMove);

	addProp<ColorImage>("Quad");

	connect(prop<ColorImage>("Quad"), &ColorImage::sizeChanged, this, [&](const QSizeF& imageSize) {
		emit imageSizeChanged(QSizeF(imageSize.width(), imageSize.height()));
	});
}

void ColorImageActor::setImage(std::shared_ptr<QImage> image)
{
	qDebug() << "Set image for" << _name;

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

	auto* colorImageQuad = shape<ColorImageQuad>("Quad");

	auto texture = QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D);

	texture->create();
	texture->setSize(image->size().width(), image->size().height());
	texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	texture->setWrapMode(QOpenGLTexture::ClampToEdge);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	texture->allocateStorage();
	texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image->bits());

	colorImageQuad->setTexture("Quad", texture);
	colorImageQuad->setRectangle(QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	resetWindowLevel();

	emit changed(this);
}

QSize ColorImageActor::imageSize()
{
	return dynamic_cast<ColorImageQuad*>(_props["Quad"].get())->imageSize();
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

	auto* colorImageQuad = shape<ColorImageQuad>("Quad");

	colorImageQuad->setMinPixelValue(minPixelValue);
	colorImageQuad->setMaxPixelValue(maxPixelValue);

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

	//qDebug() << "Mouse press event for" << _name;

	_mousePositions.clear();
	_mousePositions.push_back(mouseEvent->pos());
}

void ColorImageActor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseReleaseEvent())
		return;

	//qDebug() << "Mouse release event for" << _name;
}

void ColorImageActor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseMoveEvent())
		return;

	//qDebug() << "Mouse move event for" << _name;

	_mousePositions.push_back(mouseEvent->pos());

	if (_mousePositions.size() > 1 && mouseEvent->buttons() & Qt::RightButton) {
		auto* imageQuad = shape<ColorImageQuad>("Quad");

		const auto mousePosition0	= _mousePositions[_mousePositions.size() - 2];
		const auto mousePosition1	= _mousePositions.back();
		const auto deltaWindow		= (mousePosition1.x() - mousePosition0.x()) / 150.f;
		const auto deltaLevel		= -(mousePosition1.y() - mousePosition0.y()) / 150.f;
		const auto window			= std::clamp(_windowNormalized + deltaWindow, 0.0f, 1.0f);
		const auto level			= std::clamp(_levelNormalized + deltaLevel, 0.0f, 1.0f);

		setWindowLevel(window, level);
	}
}