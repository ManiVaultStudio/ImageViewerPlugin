#include "Channel.h"

#include <QDebug>

Channel::Channel(QObject* parent, const std::uint32_t& id, const QString& name, const std::uint32_t& dimensionId /*= -1*/) :
	QObject(parent),
	_enabled(false),
	_imageSize(),
	_id(id),
	_name(name),
	_dimensionId(dimensionId),
	_elements(),
	_range(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(),
	_level()
{
}

bool Channel::enabled() const
{
	return _enabled;
}

void Channel::setEnabled(const bool& enabled)
{
	_enabled = enabled;
}

bool Channel::inverted() const
{
	return _inverted;
}

void Channel::setInverted(const bool& inverted)
{
	_inverted = inverted;
}

QSize Channel::imageSize() const
{
	return _imageSize;
}

void Channel::setImageSize(const QSize& imageSize)
{
	_imageSize = imageSize;

	const auto noPixels = imageSize.width() * imageSize.height();

	if (_elements.count() == noPixels)
		return;

	auto elements = Elements();

	elements.resize(noPixels);
	elements.fill(0, noPixels);

	setElements(elements);
}

const std::int32_t& Channel::id() const
{
	return _id;
}

QString Channel::name() const
{
	return _name;
}

void Channel::setName(const QString& name)
{
	_name = name;
}

const std::int32_t& Channel::dimensionId() const
{
	return _dimensionId;
}

void Channel::setDimensionId(const std::int32_t& dimensionId)
{
	_dimensionId = dimensionId;
}

const Channel::Elements& Channel::elements() const
{
	return _elements;
}

void Channel::setElements(const Channel::Elements& elements)
{
	_elements			= elements;
	_windowNormalized	= 1.0f;
	_levelNormalized	= 0.5f;

	setChanged();
}

Range Channel::range() const
{
	return _range;
}

Range Channel::displayRange() const
{
	return _displayRange;
}
QVector2D Channel::displayRangeVector() const
{
	return QVector2D(displayRange().min(), displayRange().max());
}

float Channel::windowNormalized() const
{
	return _windowNormalized;
}

void Channel::setWindowNormalized(const float& windowNormalized)
{
	_windowNormalized = windowNormalized;

	computeDisplayRange();
}

float Channel::levelNormalized() const
{
	return _levelNormalized;
}

void Channel::setLevelNormalized(const float& levelNormalized)
{
	_levelNormalized = levelNormalized;

	computeDisplayRange();
}

float Channel::window() const
{
	return _window;
}

void Channel::setWindow(const float& window)
{
	_window = window;
}

float Channel::level() const
{
	return _level;
}

void Channel::setLevel(const float& level)
{
	_level = level;
}

void Channel::computeRange()
{
	if (_elements.isEmpty())
		return;

	//qDebug() << "Compute image range";

	const auto[min, max] = std::minmax_element(_elements.begin(), _elements.end());

	_range.setMin(*min);
	_range.setMax(*max);
}

void Channel::computeDisplayRange()
{
	//qDebug() << "Compute display range";

	const auto maxWindow = _range.length();

	_level	= std::clamp(_range.min() + (_levelNormalized * maxWindow), _range.min(), _range.max());
	_window	= std::clamp(_windowNormalized * maxWindow, _range.min(), _range.max());

	_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _range.min(), _range.max()));
	_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _range.min(), _range.max()));
}

bool Channel::isValid() const
{
	return _dimensionId >= 0;
}

void Channel::setChanged()
{
	computeRange();
	computeDisplayRange();

	emit changed(this);
}