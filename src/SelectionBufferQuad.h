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
	SelectionBufferQuad(const QString& name = "SelectionBufferQuad", const float& z = 0.f);

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

	/**
	 * Update the pixel selection buffer during interaction
	 */
	void update();

	/** Reset the (temporary) selection buffer */
	void reset();

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL textures that the shape needs */
	void addTextures();

	/** Adds the OpenGL frame buffer objects that the shape needs */
	void addFBOs();

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

signals:
	/**
	 * Invoked when the quad size changed
	 * @param size Quad size
	 */
	void sizeChanged(const QSize& size);

	/**
	 * Invoked when the opacity has changed
	 * @param opacity Selection opacity
	 */
	void opacityChanged(const float& opacity);

protected:
	QSize		_size;		/** Size of the quad */
	QColor		_color;		/** Selection color */
};