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

public:
	void setSelectionBounds(const QRect& selectionBounds);

protected:
	virtual bool isInitialized() const;
	virtual void createShaderPrograms();
	virtual void createTextures();

protected:
	QVector<GLfloat>						_vertexData;
	QOpenGLBuffer							_vbo;
	QOpenGLVertexArrayObject				_vao;
	QVector4D								_color;
	QRect									_selectionBounds;
};