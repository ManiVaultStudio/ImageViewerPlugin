#pragma once

#include "Common.h"

#include <QVector2D>

/**
 * Group layer class
 *
 * Layer that contains child layers
 *
 * @author Thomas Kroes
 */
class Range
{
public:

	/** TODO */
	Range(const float& min = 0.0f, const float& max = 0.0f) :
		_min(std::min(min, max)),
		_max(std::max(min, max))
	{
	}

	/** TODO */
	float min() const { return _min; }

	/** TODO */
	void setMin(const float& min) { _min = std::min(min, _max); }

	/** TODO */
	float max() const { return _max; }

	/** TODO */
	void setMax(const float& max) { _max = std::max(_min, max); }

	/** TODO */
	void include(const float& value) {
		_min = std::min(_min, value);
		_max = std::max(_max, value);
	}

	/** TODO */
	void setFullRange() {
		_max = std::numeric_limits<float>::min();
		_min = std::numeric_limits<float>::max();
	}

	/** TODO */
	float length() const { return _max - _min; }

	QVector2D toVector2D() const {
		return QVector2D(_min, _max);
	}

private:
	float	_min;	/** TODO */
	float	_max;	/** TODO */
};

Q_DECLARE_METATYPE(Range);