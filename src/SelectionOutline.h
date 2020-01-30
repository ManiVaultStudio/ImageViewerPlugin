#pragma once

#include "Polyline2D.h"

#include <QRectF>
#include <QColor>

/**
 * OpenGL selection outline class
 * @author Thomas Kroes
 */
class SelectionOutline : public Polyline2D
{
	Q_OBJECT

public:
	/** Default constructor
	 * @param renderer Renderer
	 * @param name Name of the bounds polyline
	 * @param z Depth at which to draw the shape
	 * @param color Line color
	 */
	SelectionOutline(Actor* actor, const QString& name = "SelectionOutline", const float& z = 0.f, const QColor& color = QColor(255, 153, 0, 150));

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

protected:
	QVector<QVector3D>		_mousePositions;	/** Recorded mouse positions in world coordinates */
};