#pragma once

#include "Common.h"

#include "renderers/Renderer.h"

#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QImage>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLDebugLogger>

class ImageQuadRenderer : public hdps::Renderer
{
public:
	ImageQuadRenderer();
    ~ImageQuadRenderer();

public:
	void init() override;
	void resize(QSize renderSize) override;
	void render() override;
	void destroy() override;

public:
	void setImage(std::shared_ptr<QImage> image);
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);
	float window() const;
	float level() const;
	void setWindowLevel(const float& window, const float& level);
	void resetWindowLevel();

	QSize size() const;

private:
	bool initialized() const;
	void createQuad();

private:
	std::unique_ptr<QOpenGLTexture>			_texture;
	QVector<GLfloat>						_vertexData;
	QOpenGLBuffer							_vbo;
	QOpenGLVertexArrayObject				_vao;
	std::unique_ptr<QOpenGLShaderProgram>	_program;
	std::shared_ptr<QImage>					_image;
	std::uint16_t							_imageMin;
	std::uint16_t							_imageMax;
	float									_window;
	float									_level;
	
	QMatrix4x4								_modelViewProjection;
};