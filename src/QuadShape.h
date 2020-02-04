#pragma once

#include "Shape.h"

#include <QRect>
#include <QVector>

/**
 * Quad shape class
 * @author Thomas Kroes
 */
class QuadShape : public Shape
{
	Q_OBJECT

public:
	/** Constructor
	 * @param prop Parent prop
	 * @param name Name of the shape
	 */
	QuadShape(Prop* prop, const QString& name);

// Inherited public members
public:
	/** Initializes the shape */
	void initialize() override;

	/** Returns if the shape can be rendered */
	bool canRender() const override;

	/** Renders the shape */
	void render() override;

public:
	/** Returns the quad rectangle */
	QRectF rectangle() const;

	/**
	 * Set quad rectangle
	 * @param rectangle Rectangle in world space
	 */
	void setRectangle(const QRectF& rectangle);

	/** Returns the quad size */
	QSizeF imageSize() const;

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

public:
	static std::uint32_t	_vertexAttribute;		/** Quad vertex attribute location */
	static std::uint32_t	_textureAttribute;		/** Quad texture attribute location */

protected:
	QRectF					_rectangle;				/** Rectangle (in world space) that defines the quad */
	QVector<GLfloat>		_vertexData;			/** Quad vertex data */
};