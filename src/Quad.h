#pragma once

#include "Shape.h"

#include <QVector>
#include <QOpenGLBuffer>

/**
 * OpenGL Quad class
 * @author Thomas Kroes
 */
class Quad : public Shape
{
	Q_OBJECT

public:
	/** Constructor */
	Quad(const QString& name = "Quad");

	/** Initialized the shape (must be called in appropriate OpenGL context) */
	void initialize() override;

	/** Renders the quad */
	void render();

	/** Returns the quad rectangle */
	QRectF rectangle() const;

	/** Returns the quad size */
	QSizeF size() const;

protected:
	/**
	 * Set quad rectangle
	 * @param rectangle Rectangle in world space
	 */
	void setRectangle(const QRectF& rectangle);

	/**
	 * Determines whether the quad is textured
	 * @return Whether the quad is textured
	 */
	bool isTextured() const;

	/** Adds the OpenGL vertex array objects that the shape needs */
	void addVAOs();

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	void addVBOs();

	/** Returns whether the shape can be rendered */
	virtual bool canRender() const;

private:
	/**
	 * Creates the OpenGL quad buffers
	 * @param rectangle Quad rectangle
	 */
	void createQuad();

signals:
	/**
	 * Signals that the rectangle changed
	 * @param rectangle Rectangle in world space
	 */
	void rectangleChanged(const QRectF& rectangle);

protected:
	QRectF					_rectangle;				/** Rectangle (in world space) that defines the quad */
	QVector<GLfloat>		_vertexData;			/** Quad vertex data */
	static std::uint32_t	_vertexAttribute;		/** Quad vertex attribute location */
	static std::uint32_t	_textureAttribute;		/** Quad texture attribute location */
};