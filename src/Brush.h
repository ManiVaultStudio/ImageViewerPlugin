#pragma once

#include "Polyline2D.h"

#include <QRectF>
#include <QColor>

/**
 * Brush class
 * @author Thomas Kroes
 */
class Brush : public Polyline2D
{
	Q_OBJECT

public:
	/** Default constructor
	 * @param renderer Renderer
	 * @param name Name of the bounds polyline
	 * @param z Depth at which to draw the shape
	 * @param color Line color
	 */
	Brush(Renderer* renderer, const QString& name = "Brush", const float& z = 0.f, const QColor& color = QColor(255, 153, 0, 150));

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

	/** Activate the shape */
	virtual void activate();

	/** Deactivate the shape */
	virtual void deactivate();

protected:
	/** Updates the internals of the shape */
	void update() override;

	/** Adds the OpenGL textures that the shape needs */
	void addTextures();

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

signals:
	/**
	 * Signals that the color changed
	 * @param color Color
	 */
	void colorChanged(const QColor& color);

protected:
	QColor					_color;				/** Color */
	QVector<QVector3D>		_mousePositions;	/** Recorded mouse positions in world coordinates */
};