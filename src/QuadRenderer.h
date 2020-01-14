#pragma once

#include "Common.h"

#include "StackedRenderer.h"

#include <QSize>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QImage>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLDebugLogger>

class QuadRenderer : public StackedRenderer
{
public:
	QuadRenderer(const std::uint32_t& zIndex);

public:
	void init() override;
	void resize(QSize renderSize) override;
	void render() override;
	void destroy() override;

public:
	QSize size() const;
	void setSize(const QSize& size);

protected:
	void createQuad();

protected:
	QSize						_size;
	QVector<GLfloat>			_vertexData;
	QOpenGLBuffer				_quadVBO;
	QOpenGLVertexArrayObject	_quadVAO;
	static std::uint32_t		_quadVertexAttribute;
	static std::uint32_t		_quadTextureAttribute;
};