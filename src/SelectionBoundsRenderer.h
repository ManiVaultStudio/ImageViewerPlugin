#pragma once

#include "Common.h"

#include "renderers/Renderer.h"

#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QVector4D>
#include <QRect>

class SelectionBoundsRenderer : public hdps::Renderer
{
public:
	SelectionBoundsRenderer();

public:
	void init() override;
	void render() override;
	void destroy() override;

public:
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:

protected:
	QVector<GLfloat>						_vertexData;
	QOpenGLBuffer							_vbo;
	QOpenGLVertexArrayObject				_vao;
	std::unique_ptr<QOpenGLShaderProgram>	_program;
	QMatrix4x4								_modelViewProjection;
	QVector4D								_selectionBoundsColor;
	QRect									_selectionBounds;
};