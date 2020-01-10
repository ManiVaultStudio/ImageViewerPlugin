#pragma once

#include "Common.h"

#include "StackedRenderer.h"

#include <QOpenGLTexture>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QVector4D>
#include <QRect>

class SelectionBoundsRenderer : public StackedRenderer
{
public:
	SelectionBoundsRenderer(const std::uint32_t& zIndex);

public:
	void init() override;
	void resize(QSize renderSize);
	void render() override;
	void destroy() override;

	void initializePrograms();

public:
	void setSelectionBounds(const QRect& selectionBounds);

protected:
	bool initialized() const;

protected:
	std::unique_ptr<QOpenGLTexture>			_texture;
	QVector<GLfloat>						_vertexData;
	QOpenGLBuffer							_vbo;
	QOpenGLVertexArrayObject				_vao;
	std::unique_ptr<QOpenGLShaderProgram>	_program;
	QVector4D								_color;
	QRect									_selectionBounds;
};