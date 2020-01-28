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
	Quad(Renderer* renderer, const QString& name, const float& z = 0.f);

	/** Initialized the shape (must be called in appropriate OpenGL context) */
	void initialize() override;

	/** Renders the quad */
	void render();

	/** Returns the quad rectangle */
	QRectF rectangle() const;

	/** Returns the quad size */
	QSizeF size() const;

	/** Return position along z-axis */
	float z() const;

	/**
	 * Set position along z-axis
	 * @param z Position along z-axis
	 */
	void setZ(const float& z);

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

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

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

	/**
	 * Invoked when the quad size changed
	 * @param size Quad size
	 */
	void sizeChanged(const QSizeF& size);

	/**
	 * Signals that z changed
	 * @param z Position along z-axis
	 */
	void zChanged(const float& z);

protected:
	QRectF					_rectangle;				/** Rectangle (in world space) that defines the quad */
	float					_z;						/** Position along z-axis */
	QVector<GLfloat>		_vertexData;			/** Quad vertex data */
	static std::uint32_t	_vertexAttribute;		/** Quad vertex attribute location */
	static std::uint32_t	_textureAttribute;		/** Quad texture attribute location */
};