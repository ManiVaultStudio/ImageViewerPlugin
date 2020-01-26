#pragma once

#include "Polyline2D.h"

#include <QRectF>
#include <QColor>

/**
 * OpenGL bounds class
 * @author Thomas Kroes
 */
class SelectionBounds : public Polyline2D
{
	Q_OBJECT

public:
	/** Default constructor
	 * @param name Name of the bounds polyline
	 */
	SelectionBounds(Renderer* renderer, const QString& name, const float& z = 0.f, const QColor& color = QColor(255, 153, 0, 150));

	/** Set bounds
	 * @param bounds Bounds
	 */
	void setBounds(const QRectF& bounds);

	/** Set rendering color
	 * @param color Color
	 */
	void setColor(const QColor& color);

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL vertex array objects that the shape needs */
	void addVAOs();

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	void addVBOs();

	/** Adds the OpenGL textures that the shape needs */
	void addTextures();

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

signals:
	/**
	 * Signals that the bounds changed
	 * @param bounds Bounds
	 */
	void boundsChanged(const QRectF& bounds);
	
	/**
	 * Signals that the color changed
	 * @param color Color
	 */
	void colorChanged(const QColor& color);

protected:
	QRectF		_bounds;		/** Bounds */
	QColor		_color;			/** Color */
};