#include "ImageQuadRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

ImageQuadRenderer::ImageQuadRenderer() :
	QuadRenderer(),
	_imageMin(),
	_imageMax(),
	_window(),
	_level()
{
}

ImageQuadRenderer::~ImageQuadRenderer()
{
}

void ImageQuadRenderer::init()
{
	initializeOpenGLFunctions();

	_program = std::make_unique<QOpenGLShaderProgram>();

	// Shader program
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, imageVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, imageFragmentShaderSource.c_str());
	
	_program->link();
	
	// Vertex buffer object
	_vbo.create();
	_vbo.bind();
	_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_vbo.release();

	// Vertex array object
	_vao.create();

	_program->bind();

	_vao.bind();
	{
		_vbo.bind();

		_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
	}
	_vao.release();
	_vbo.release();
	_program->release();
}

void ImageQuadRenderer::render()
{
	//qDebug() << "ImageQuadRenderer::render" << _modelViewProjection;

	if (!initialized())
		return;

	_program->bind();
	{
		_program->setUniformValue("imageTexture", 0);
		_program->setUniformValue("matrix", _modelViewProjection);

		const auto imageMin = static_cast<float>(_imageMin);
		const auto imageMax = static_cast<float>(_imageMax);
		const auto maxWindow = static_cast<float>(imageMax - imageMin);
		const auto level = std::clamp(imageMin + (_level * maxWindow), imageMin, imageMax);
		const auto window = std::clamp(_window * maxWindow, imageMin, imageMax);
		const auto minPixelValue = std::clamp(level - (window / 2.0f), imageMin, imageMax);
		const auto maxPixelValue = std::clamp(level + (window / 2.0f), imageMin, imageMax);

		_program->setUniformValue("minPixelValue", minPixelValue);
		_program->setUniformValue("maxPixelValue", maxPixelValue);

		_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

		_texture->bind();
		_vao.bind();

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		_vao.release();
		_texture->release();
	}
	_program->release();
}

void ImageQuadRenderer::setImage(std::shared_ptr<QImage> image)
{
	_image = image;

	std::uint16_t* pixels = (std::uint16_t*)_image->bits();

	const auto noPixels = _image->width() * _image->height();

	auto test = std::vector<std::uint16_t>(pixels, pixels + noPixels * 4);

	_imageMin = std::numeric_limits<std::uint16_t>::max();
	_imageMax = std::numeric_limits<std::uint16_t>::min();

	for (std::int32_t y = 0; y < _image->height(); y++)
	{
		for (std::int32_t x = 0; x < _image->width(); x++)
		{
			const auto pixelId = y * _image->width() + x;

			for (int c = 0; c < 3; c++)
			{
				const auto channel = reinterpret_cast<std::uint16_t*>(_image->bits())[pixelId * 4 + c];

				if (channel < _imageMin)
					_imageMin = channel;

				if (channel > _imageMax)
					_imageMax = channel;
			}
		}
	}

	_texture.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));

	_texture->setSize(_image->size().width(), _image->size().height());
	_texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	_texture->setWrapMode(QOpenGLTexture::ClampToEdge);
	_texture->allocateStorage();
	_texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, _image->bits());

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