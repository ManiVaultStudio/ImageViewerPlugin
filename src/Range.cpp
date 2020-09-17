#include "Range.h"

Range::Range(const float& min /*= 0.0f*/, const float& max /*= 0.0f*/) :
	_min(std::min(min, max)),
	_max(std::max(min, max))
{

}

float Range::getMin() const
{
	return _min;
}

void Range::setMin(const float& min)
{
	_min = std::min(min, _max);
}

float Range::getMax() const
{
	return _max;
}

void Range::setMax(const float& max)
{
	_max = std::max(_min, max);
}

void Range::include(const float& value)
{
	_min = std::min(_min, value);
	_max = std::max(_max, value);
}

void Range::setFullRange()
{
	_max = std::numeric_limits<float>::min();
	_min = std::numeric_limits<float>::max();
}

float Range::getLength() const
{
	return _max - _min;
}