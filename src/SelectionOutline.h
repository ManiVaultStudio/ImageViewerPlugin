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
	SelectionOutline(const QString& name = "SelectionOutline", const float& z = 0.f, const QColor& color = QColor(255, 153, 0, 150));

	/** Returns the color */
	QColor color() const;

	/** Set rendering color
	 * @param color Color
	 */
	void setColor(const QColor& color);

	/** Updates the selection buffer
	 * @param mousePositions Mouse positions in world coordinates
	 * @param selectionType Selection type
	 */
	void update(std::vector<QVector3D> mousePositions, const SelectionType& selectionType);

	/** Returns the view rectangle */
	QRect viewRectangle() const;

	/** Set view size
	 * @param viewSize View size
	 */
	void setViewRectangle(const QRect& viewRectangle);

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
	QColor		_color;				/** Color */
	QRect		_viewRectangle;		/** View rectangle */
};