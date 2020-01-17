#include "ImageQuadRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

ImageQuadRenderer::ImageQuadRenderer(const float& depth) :
	QuadRenderer(depth),
	_imageMin(),
	_imageMax(),
	_windowNormalized(),
	_levelNormalized(),
	_window(1.0f),
	_level(0.5f)
{
}

void ImageQuadRenderer::init()
{
	QuadRenderer::init();

	auto quadProgram = shaderProgram("Quad");
	
	if (quadProgram->bind()) {
		auto quadVBO = vbo("Quad");
		auto quadVAO = vao("Quad");

		quadVAO->bind();
		quadVBO->bind();

		const auto stride = 5 * sizeof(GLfloat);

		quadProgram->enableAttributeArray(QuadRenderer::_quadVertexAttribute);
		quadProgram->enableAttributeArray(QuadRenderer::_quadTextureAttribute);
		quadProgram->setAttributeBuffer(QuadRenderer::_quadVertexAttribute, GL_FLOAT, 0, 3, stride);
		quadProgram->setAttributeBuffer(QuadRenderer::_quadTextureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVAO->release();
		quadVBO->release();

		quadProgram->release();
	}
}

void ImageQuadRenderer::render()
{
	if (!isInitialized())
		return;

	const auto quadProgram = shaderProgram("Quad");

	if (quadProgram->bind()) {
		quadProgram->setUniformValue("imageTexture", 0);
		quadProgram->setUniformValue("transform", _modelViewProjection);

		const auto imageMin			= static_cast<float>(_imageMin);
		const auto imageMax			= static_cast<float>(_imageMax);
		const auto minPixelValue	= std::clamp(_level - (_window / 2.0f), imageMin, imageMax);
		const auto maxPixelValue	= std::clamp(_level + (_window / 2.0f), imageMin, imageMax);

		quadProgram->setUniformValue("minPixelValue", minPixelValue);
		quadProgram->setUniformValue("maxPixelValue", maxPixelValue);

		auto quadTexture = texture("Quad");

		quadTexture->bind();
		{
			QuadRenderer::render();
		}
		quadTexture->release();

		quadProgram->release();
	}
}

void ImageQuadRenderer::setImage(std::shared_ptr<QImage> image)
{
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

	auto quadTexture = QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D);

	quadTexture->create();
	quadTexture->setSize(image->size().width(), image->size().height());
	quadTexture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	quadTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
	quadTexture->allocateStorage();
	quadTexture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image->bits());

	_textures["Quad"] = quadTexture;

	setSize(image->size());

	resetWindowLevel();
}

std::uint16_t ImageQuadRenderer::imageMin() const
{
	return _imageMin;
}

std::uint16_t ImageQuadRenderer::imageMax() const
{
	return _imageMax;
}

void ImageQuadRenderer::setImageMinMax(const std::uint16_t& imageMin, const std::uint16_t& imageMax)
{
	if (imageMin == _imageMin && imageMax == _imageMax)
		return;

	_imageMin = imageMin;
	_imageMax = imageMax;

	qDebug() << "Set image min/max" << _imageMin << _imageMax;

	emit imageMinMaxChanged(_imageMin, _imageMax);
}

float ImageQuadRenderer::windowNormalized() const
{
	return _windowNormalized;
}

float ImageQuadRenderer::window() const
{
	return _window;
}

float ImageQuadRenderer::levelNormalized() const
{
	return _levelNormalized;
}

float ImageQuadRenderer::level() const
{
	return _level;
}

void ImageQuadRenderer::setWindowLevel(const float& window, const float& level)
{
	if (window == _windowNormalized && level == _levelNormalized)
		return;

	_windowNormalized	= std::clamp(window, 0.01f, 1.0f);
	_levelNormalized	= std::clamp(level, 0.01f, 1.0f);
	
	const auto maxWindow = static_cast<float>(_imageMax - _imageMin);

	_level	= std::clamp(_imageMin + (_levelNormalized * maxWindow), static_cast<float>(_imageMin), static_cast<float>(_imageMax));
	_window	= std::clamp(_windowNormalized * maxWindow, static_cast<float>(_imageMin), static_cast<float>(_imageMax));

	qDebug() << "Set window/level" << _windowNormalized << _levelNormalized;

	emit windowLevelChanged(_window, _level);
}

void ImageQuadRenderer::resetWindowLevel()
{
	setWindowLevel(1.0f, 0.5f);
}

bool ImageQuadRenderer::isInitialized() const
{
	const auto quadTexture = texture("Quad");
	
	return quadTexture.get() != nullptr && quadTexture->isCreated();
}

void ImageQuadRenderer::createShaderPrograms()
{
	auto quadProgram = QSharedPointer<QOpenGLShaderProgram>::create();

	quadProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, imageVertexShaderSource.c_str());
	quadProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, imageFragmentShaderSource.c_str());
	quadProgram->link();

	_shaderPrograms.insert("Quad", quadProgram);
}

void ImageQuadRenderer::createTextures()
{
	_textures.insert("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));
}