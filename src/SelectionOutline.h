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
	 * @param name Name of the bounds polyline
	 */
	SelectionOutline(Renderer* renderer, const QString& name = "SelectionOutline", const float& z = 0.f, const QColor& color = QColor(255, 153, 0, 150));

	/** Returns the color */
	QColor color() const;

	/** Set rendering color
	 * @param color Color
	 */
	void setColor(const QColor& color);

	/** Returns the view rectangle */
	QRect viewRectangle() const;

	/** Set view size
	 * @param viewSize View size
	 */
	void setViewRectangle(const QRect& viewRectangle);

	/** Updates the selection buffer
	 * @param mousePositions Mouse positions in world coordinates
	 * @param selectionType Selection type
	 */
	void update(std::vector<QVector3D> mousePositions, const SelectionType& selectionType, const float& brushRadius);

	/** Reset the outline */
	void reset();

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

	/** Activate the shape */
	void activate() override;

	/** Deactivate the shape */
	void deactivate() override;

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
	 * Signals that the color changed
	 * @param color Color
	 */
	void colorChanged(const QColor& color);

	/**
	 * Signals that the view rectangle changed
	 * @param viewRectangle View rectangle
	 */
	void viewRectangleChanged(const QRect& viewRectangle);

protected:
	QColor					_color;				/** Color */
	QRect					_viewRectangle;		/** View rectangle */
	QVector<QVector3D>		_mousePositions;	/** Recorded mouse positions in world coordinates */
};