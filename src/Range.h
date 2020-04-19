#pragma once

#include "Common.h"

#include <QVector2D>

/**
 * Range class
 *
 * Layer that contains child layers
 *
 * @author Thomas Kroes
 */
class Range
{
public:

	/**
	 * Constructor
	 * @param min Range minimum
	 * @param max Range maximum
	 */
	Range(const float& min = 0.0f, const float& max = 0.0f);

	/** Returns the range minimum */
	float min() const;

	/**
	 * Sets the range minimum
	 * @param min Range minimum
	 */
	void setMin(const float& min);

	/** Returns the range maximum */
	float max() const;

	/**
	 * Sets the range maximum
	 * @param max Range maximum
	 */
	void setMax(const float& max);

	/**
	 * Make the range include the specified value
	 * @param value Value to include
	 */
	void include(const float& value);

	/** Set the range minimum and maximum to the float minimum and maximum */
	void setFullRange();

	/** Returns the length of the range */
	float length() const;

	/** Converts the range to a two-dimensional vector where the first item is the minimum and the second item the maximum */
	QVector2D toVector2D() const;

private:
	float	_min;	/** Range minimum */
	float	_max;	/** Range maximum */
};

Q_DECLARE_METATYPE(Range);