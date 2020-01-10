#include "ImageQuadRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

ImageQuadRenderer::ImageQuadRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_imageMin(),
	_imageMax(),
	_window(),
	_level()
{
}

void ImageQuadRenderer::render()
{
	if (!initialized())
		return;

	_program->bind();
	{
		_program->setUniformValue("imageTexture", 0);
		_program->setUniformValue("transform", _modelViewProjection);

		const auto imageMin = static_cast<float>(_imageMin);
		const auto imageMax = static_cast<float>(_imageMax);
		const auto maxWindow = static_cast<float>(imageMax - imageMin);
		const auto level = std::clamp(imageMin + (_level * maxWindow), imageMin, imageMax);
		const auto window = std::clamp(_window * maxWindow, imageMin, imageMax);
		const auto minPixelValue = std::clamp(level - (window / 2.0f), imageMin, imageMax);
		const auto maxPixelValue = std::clamp(level + (window / 2.0f), imageMin, imageMax);

		_program->setUniformValue("minPixelValue", minPixelValue);
		_program->setUniformValue("maxPixelValue", maxPixelValue);

		QuadRenderer::render();
	}
	_program->release();
}

void ImageQuadRenderer::initializeProgram()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, imageVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, imageFragmentShaderSource.c_str());
	_program->link();
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

	_texture.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));

	_texture->setSize(image->size().width(), image->size().height());
	_texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	_texture->setWrapMode(QOpenGLTexture::ClampToEdge);
	_texture->allocateStorage();
	_texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image->bits());

	createQuad();
	resetWindowLevel();
}

float ImageQuadRenderer::window() const
{
	return _window;
}

float ImageQuadRenderer::level() const
{
	return _level;
}

void ImageQuadRenderer::setWindowLevel(const float& window, const float& level)
{
	_window	= std::clamp(window, 0.01f, 1.0f);
	_level	= std::clamp(level, 0.01f, 1.0f);

	qDebug() << "Set window/level" << _window << _level;
}

void ImageQuadRenderer::resetWindowLevel()
{
	_window = 1.0;
	_level	= 0.5;
}