#pragma once

#include "Quad.h"

#include <QColor>

/**
 * OpenGL Selection image quad class
 * @author Thomas Kroes
 */
class SelectionImageQuad : public Quad
{
	Q_OBJECT

public:
	/** Constructor */
	SelectionImageQuad(Actor* actor, const QString& name, const float& z = 0.f);

	/** Returns the selection image size */
	QSize size() const;

	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL textures that the shape needs */
	void addTextures();

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

protected:
	QColor		_color;		/** Selection color */
};