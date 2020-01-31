#pragma once

#include "Quad.h"

class Actor;

/**
 * Color image quad class
 * @author Thomas Kroes
 */
class ColorImageQuad : public Quad
{
	Q_OBJECT

public:
	/** Constructor */
	ColorImageQuad(Actor* actor, const QString& name, const float& z = 0.f);

	/** Returns the color image size */
	QSize imageSize() const;

	/**
	 * Sets the window/level based minimum pixel value
	 * @param minPixelValue Minimum pixel value
	 */
	void setMinPixelValue(const float& minPixelValue);

	/**
	 * Sets the window/level based maximum pixel value
	 * @param maxPixelValue Maximum pixel value
	 */
	void setMaxPixelValue(const float& maxPixelValue);

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
	
	float				_minPixelValue;			/** Window minimum pixel value */
	float				_maxPixelValue;			/** Window maximum pixel value */
	
};