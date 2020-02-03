#pragma once

#include "Shape.h"

#include <QRect>
#include <QVector>

/**
 * Quad shape class
 * @author Thomas Kroes
 */
class Quad : public Shape
{
	Q_OBJECT

public:
	/** Constructor
	 * @param prop Parent prop
	 * @param name Name of the shape
	 */
	Quad(Prop* prop, const QString& name);

	/** Returns the quad rectangle */
	QRectF rectangle() const;

	/**
	 * Set quad rectangle
	 * @param rectangle Rectangle in world space
	 */
	void setRectangle(const QRectF& rectangle);

	/** Returns the quad size */
	QSizeF imageSize() const;

protected:
	/** Initializes the shape */
	void initialize() override;

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

protected:
	QRectF					_rectangle;				/** Rectangle (in world space) that defines the quad */
	QVector<GLfloat>		_vertexData;			/** Quad vertex data */
	static std::uint32_t	_vertexAttribute;		/** Quad vertex attribute location */
	static std::uint32_t	_textureAttribute;		/** Quad texture attribute location */
};