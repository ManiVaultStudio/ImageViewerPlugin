#pragma once

#include "Common.h"

#include <QSharedPointer>

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class Polyline2D
{
public:
	Polyline2D(QSharedPointer<QOpenGLShaderProgram> shaderProgram, const bool& closed = true, const float& lineWidth = 1.f, const float& textureScale = 0.05f);

public:
	/** Initialized the shape (must be called in appropriate OpenGL context) */
	void initialize();

	/**
	 * Set polyline points
	 * @param points Points in world coordinates
	 */
	void setPoints(QVector<QVector2D> points);

public:
	/** Renders the polyline */
	void render();

signals:

protected:
	QSharedPointer<QOpenGLShaderProgram>		_shaderProgram;		/** Shader program */
	QSharedPointer<QOpenGLVertexArrayObject>	_vao;				/** Vertex Array Object (VAO) */
	QSharedPointer<QOpenGLBuffer>				_vbo;				/** Vertex Buffer Object (VBO) */
	bool										_closed;			/** Whether to close the polyline or not */
	float										_lineWidth;			/** Line width in world space */
	float										_textureScale;		/** Texture scale */
};