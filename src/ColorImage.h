#pragma once

#include "Prop.h"

class Actor;

/**
 * Color image prop class
 * @author Thomas Kroes
 */
class ColorImage : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	ColorImage(Actor* actor, const QString& name);

public:
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
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

private:
	float	_minPixelValue;			/** Window minimum pixel value */
	float	_maxPixelValue;			/** Window maximum pixel value */
};