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
	/**
	 * Constructor
	 * @param renderer Renderer
	 * @param name Name of the shape
	 * @param z Depth at which to draw the shape
	 */
	SelectionBufferQuad(Actor* actor, const QString& name, const float& z = 0.f);

	/** Renders the selection buffer quad */
	void render();

	/** Returns the quad size */
	QSize size() const;

	/**
	 * Sets the quad size
	 * @param size Quad size
	 */
	void setSize(const QSize& size);

	/** Returns the opacity */
	float opacity() const;

	/**
	 * Sets the opacity
	 * @param opacity Opacity
	 */
	void setOpacity(const float& opacity);

	/** Reset the (temporary) selection buffer */
	void reset();

	/** Returns the current selection buffer image */
	QSharedPointer<QImage> selectionBufferImage() const;

protected:
	/** Updates the internals of the shape */
	void update() override;
	
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
	QSize				_size;				/** Size of the quad */
	QColor				_color;				/** Selection color */
	QVector<QVector3D>	_mousePositions;	/** Recorded mouse positions in world coordinates */
};