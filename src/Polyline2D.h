#pragma once

#include "Shape.h"

/**
 * OpenGL polyline 2D class
 * @author Thomas Kroes
 */
class Polyline2D : public Shape
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param closed Whether to close the polyline or not
	 * @param lineWidth Line width
	 * @param textureScale Scale of the texture in the U direction
	 */
	Polyline2D(const QString& name, const bool& closed = true, const float& lineWidth = 1.f, const float& textureScale = 0.05f);

	/** Initialized the shape (must be called in appropriate OpenGL context)
	 * @param shaderProgram Shared pointer to the OpenGL shader program
	 * @param texture Shared pointer to the OpenGL texture
	 */
	void initialize() override;

	/** Renders the polyline */
	void render() override;

	/** Returns the shader program for this polyline */
	const QSharedPointer<QOpenGLShaderProgram> shaderProgram() const;

	/** Returns the OpenGL Vertex Array Object (VAO) for this polyline */
	const QSharedPointer<QOpenGLVertexArrayObject> vao() const;

	/** Returns the OpenGL Vertex Buffer Object (VBO) for this polyline */
	const QSharedPointer<QOpenGLBuffer> vbo() const;

	/** Returns the OpenGL texture for this polyline */
	const QSharedPointer<QOpenGLTexture> texture() const;

	/** Returns the line width in world space */
	float lineWidth() const;

	/**
	 * Sets the line width
	 * @param lineWidth Line width in world space
	 */
	void setLineWidth(const float& lineWidth);

protected:
	/**
	 * Set polyline points
	 * @param points Points in world coordinates
	 */
	void setPoints(QVector<QVector2D> points);

	/** Returns the shader program for this polyline */
	QSharedPointer<QOpenGLShaderProgram> shaderProgram();

	/** Returns the OpenGL Vertex Array Object (VAO) for this polyline */
	QSharedPointer<QOpenGLVertexArrayObject> vao();

	/** Returns the OpenGL Vertex Buffer Object (VBO) for this polyline */
	QSharedPointer<QOpenGLBuffer> vbo();

	/** Returns the OpenGL texture for this polyline */
	QSharedPointer<QOpenGLTexture> texture();

	/**
	 * Determines whether the polyline is textured
	 * @return Whether the polyline is textured
	 */
	bool isTextured() const;

signals:
	/**
	 * Signals that the line width changed
	 * @param lineWidth Line width in world space
	 */
	void lineWidthChanged(const float& lineWidth);

protected:
	bool				_closed;			/** Whether to close the polyline or not */
	float				_lineWidth;			/** Line width in world space */
	float				_textureScale;		/** Texture scale */
	std::uint32_t		_noPoints;			/** Number of points */
};