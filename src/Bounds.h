#pragma once

#include "Polyline2D.h"

#include <QRectF>

/**
 * OpenGL bounds class
 * @author Thomas Kroes
 */
class Bounds : public Polyline2D
{
public:
	/** Default constructor
	 * @param name Name of the bounds polyline
	 */
	Bounds(const QString& name = "Bounds");

	/** Set bounds
	 * @param bounds Bounds
	 */
	void setBounds(const QRectF& bounds);

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL vertex array objects that the shape needs */
	void addVAOs();

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	void addVBOs();

	/** Adds the OpenGL textures that the shape needs */
	void addTextures();

signals:

protected:
	QRectF		_bounds;			/** Bounds */
};