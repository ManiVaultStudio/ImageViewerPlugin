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

class QuadRenderer : public hdps::Renderer
{
public:
	QuadRenderer();

public:
	void init() override;
	void resize(QSize renderSize) override;
	void render() override;
	void destroy() override;

public:
	void setImage(std::shared_ptr<QImage> image);
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);
	QSize size() const;

protected:
	bool initialized() const;
	void createQuad();

protected:
	std::unique_ptr<QOpenGLTexture>			_texture;
	QVector<GLfloat>						_vertexData;
	QOpenGLBuffer							_vbo;
	QOpenGLVertexArrayObject				_vao;
	std::unique_ptr<QOpenGLShaderProgram>	_program;
	std::shared_ptr<QImage>					_image;
	QMatrix4x4								_modelViewProjection;
};