#include "ImageQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QDebug>

#include "Shaders.h"

ImageQuad::ImageQuad(Renderer* renderer, const QString& name, const float& z /*= 0.f*/) :
	Quad(renderer, name, z),
	_imageMin(),
	_imageMax(),
	_minPixelValue(),
	_maxPixelValue(),
	_windowNormalized(),
	_levelNormalized(),
	_window(1.0f),
	_level(0.5f),
	_mousePositions()
{
	_mouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);
}

void ImageQuad::setImage(std::shared_ptr<QImage> image)
{
	qDebug() << "Set image for" << _name;

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

	texture("Quad")->destroy();
	texture("Quad")->create();
	texture("Quad")->setSize(image->size().width(), image->size().height());
	texture("Quad")->setFormat(QOpenGLTexture::RGBA16_UNorm);
	texture("Quad")->setWrapMode(QOpenGLTexture::ClampToEdge);
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	texture("Quad")->allocateStorage();
	texture("Quad")->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image->bits());

	setRectangle(QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	resetWindowLevel();

	emit changed(this);
}

std::uint16_t ImageQuad::imageMin() const
{
	return _imageMin;
}

std::uint16_t ImageQuad::imageMax() const
{
	return _imageMax;
}

void ImageQuad::setImageMinMax(const std::uint16_t& imageMin, const std::uint16_t& imageMax)
{
	if (imageMin == _imageMin && imageMax == _imageMax)
		return;

	_imageMin = imageMin;
	_imageMax = imageMax;

	qDebug() << "Set image min/max" << _imageMin << _imageMax;

	emit imageMinMaxChanged(_imageMin, _imageMax);
}

float ImageQuad::windowNormalized() const
{
	return _windowNormalized;
}

float ImageQuad::window() const
{
	return _window;
}

float ImageQuad::levelNormalized() const
{
	return _levelNormalized;
}

float ImageQuad::level() const
{
	return _level;
}

void ImageQuad::setWindowLevel(const float& window, const float& level)
{
	if (window == _windowNormalized && level == _levelNormalized)
		return;

	_windowNormalized	= std::clamp(window, 0.01f, 1.0f);
	_levelNormalized	= std::clamp(level, 0.01f, 1.0f);

	const auto maxWindow = static_cast<float>(_imageMax - _imageMin);

	_level			= std::clamp(_imageMin + (_levelNormalized * maxWindow), static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	_window			= std::clamp(_windowNormalized * maxWindow, static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	_minPixelValue	= std::clamp(_level - (_window / 2.0f), static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	_maxPixelValue	= std::clamp(_level + (_window / 2.0f), static_cast<float>(_imageMin), static_cast<float>(_imageMax));

	qDebug() << "Set window/level" << _windowNormalized << _levelNormalized;

	emit windowLevelChanged(_window, _level);

	emit changed(this);
}

void ImageQuad::resetWindowLevel()
{
	setWindowLevel(1.0f, 0.5f);
}

void ImageQuad::onMousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event for" << _name;

	_mousePositions.clear();
	_mousePositions.push_back(mouseEvent->pos());
}

void ImageQuad::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event for" << _name;
}

void ImageQuad::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse move event for" << _name;

	_mousePositions.push_back(mouseEvent->pos());

	if (_mousePositions.size() > 1)
		update();
}

QVector<QPoint> ImageQuad::mousePositions() const
{
	return _mousePositions;
}

void ImageQuad::update()
{
	Quad::update();

	const auto mousePosition0	= _mousePositions[_mousePositions.size() - 2];
	const auto mousePosition1	= _mousePositions.back();
	const auto deltaWindow		= (mousePosition1.x() - mousePosition0.x()) / 150.f;
	const auto deltaLevel		= -(mousePosition1.y() - mousePosition0.y()) / 150.f;
	const auto window			= std::clamp(windowNormalized() + deltaWindow, 0.0f, 1.0f);
	const auto level			= std::clamp(levelNormalized() + deltaLevel, 0.0f, 1.0f);

	setWindowLevel(window, level);
}

void ImageQuad::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Vertex, imageQuadVertexShaderSource.c_str());
	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Fragment, imageQuadFragmentShaderSource.c_str());
	shaderProgram("Quad")->link();
}

void ImageQuad::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	addTexture("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));

	texture("Quad")->setWrapMode(QOpenGLTexture::Repeat);
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

void ImageQuad::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("imageTexture", 0);
		quadProgram->setUniformValue("minPixelValue", _minPixelValue);
		quadProgram->setUniformValue("maxPixelValue", _maxPixelValue);
	}
}