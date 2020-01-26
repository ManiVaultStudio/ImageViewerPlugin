#pragma once

#include "Quad.h"

#include <QColor>

/**
 * OpenGL Selection image quad class
 * @author Thomas Kroes
 */
class SelectionBufferQuad : public Quad
{
	Q_OBJECT

public:
	/** Constructor */
	SelectionBufferQuad(Renderer* renderer, const QString& name, const float& z = 0.f);

	/** Renders the selection buffer quad */
	void render();

	/** Returns the quad size */
	QSize size() const;

	/**
	 * Sets the quad size
	 * @param size Quad size
	 */
	void setSize(const QSize& size);

	/** Returns the color */
	QColor color() const;

	/**
	 * Sets the color
	 * @param size Quad size
	 */
	void setColor(const QColor& color);

	/** Returns the opacity */
	float opacity() const;

	/**
	 * Sets the opacity
	 * @param opacity Opacity
	 */
	void setOpacity(const float& opacity);

	/** Updates the selection buffer
	 * @param mousePositions Mouse positions in world coordinates
	 * @param selectionType Selection type
	 */
	void update();

	/** Reset the (temporary) selection buffer */
	void reset();

	/** Returns the current selection buffer image */
	QSharedPointer<QImage> selectionBufferImage() const;

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

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

signals:
	/**
	 * Invoked when the size changed
	 * @param size Quad size
	 */
	void sizeChanged(const QSize& size);

	/**
	 * Invoked when the color changed
	 * @param color Color
	 */
	void colorChanged(const QColor& color);

	/**
	 * Invoked when the opacity changed
	 * @param opacity Selection opacity
	 */
	void opacityChanged(const float& opacity);

	/**
	 * Invoked when the selection process has ended */
	void selectionEnded();

protected:
	QSize					_size;				/** Size of the quad */
	QColor					_color;				/** Selection color */
	QVector<QVector3D>		_mousePositions;	/** Recorded mouse positions during active phase */
};