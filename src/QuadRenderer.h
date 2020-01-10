#pragma once

#include "Common.h"

#include "StackedRenderer.h"

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

	virtual void initializeProgram() = 0;

public:
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
	QMatrix4x4								_modelViewProjection;
};