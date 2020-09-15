#pragma once

#include "Range.h"

#include <QObject>
#include <QSize>
#include <QVector2D>

#include <vector>
#include <memory>

/**
* Channel class
*
* Templated image channel class with window/level functionality
*
* @author Thomas Kroes
*/
template<typename ChannelType>
class Channel
{
public:
	using Elements = std::vector<ChannelType>;

public:

	/**
	 * Constructor
	 * @param id Channel identifier
	 * @param name Channel name
	 * @param dimensionId Channel dimension identifier
	 */
	Channel(const std::uint32_t& id, const QString& name, const std::uint32_t& dimensionId = -1) :
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

	/** Returns whether the channel is enabled */
	bool enabled() const
	{
		return _enabled;
	}

	/**
	 * Sets whether the channel is enabled
	 * @param enabled Whether the channel is enabled
	 */
	void setEnabled(const bool& enabled)
	{
		_enabled = enabled;
	}

	/** Returns the channel image size */
	QSize imageSize() const
	{
		return _imageSize;
	}

	/**
	 * Sets the channel image size
	 * @param imageSize Channel image size
	 */
	void setImageSize(const QSize& imageSize)
	{
		_imageSize = imageSize;

		const auto noPixels = imageSize.width() * imageSize.height();

		if (_elements.size() == noPixels)
			return;

		auto elements = Elements();

		elements.resize(noPixels);

		setElements(elements);
	}

	/** Returns the channel identifier */
	const std::int32_t& id() const
	{
		return _id;
	}

	/** Returns the channel name */
	QString name() const
	{
		return _name;
	}

	/**
	 * Sets the channel name
	 * @param name Channel name
	 */
	void setName(const QString& name)
	{
		_name = name;
	}

	/** Returns the channel dimension identifier */
	const std::int32_t& dimensionId() const
	{
		return _dimensionId;
	}

	/**
	 * Sets the channel dimension identifier
	 * @param dimensionId Channel dimension identifier
	 */
	void setDimensionId(const std::int32_t& dimensionId)
	{
		_dimensionId = dimensionId;
	}

	/** Returns the channel elements */
	const Elements& elements() const
	{
		return _elements;
	}

	/**
	 * Sets the channel elements
	 * @param elements Elements
	 */
	void setElements(const Elements& elements)
	{
		_elements = elements;
		_windowNormalized = 1.0f;
		_levelNormalized = 0.5f;

		setChanged();
	}

	/** Returns the channel elements range */
	Range range() const
	{
		return _range;
	}

	/** Returns the display range */
	Range displayRange() const
	{
		return _displayRange;
	}

	/** Returns the display range in vector format */
	QVector2D displayRangeVector() const
	{
		return QVector2D(displayRange().min(), displayRange().max());
	}

	/** Returns the normalized window */
	float windowNormalized() const
	{
		return _windowNormalized;
	}

	/**
	 * Sets the normalized window
	 * @param windowNormalized Normalized window
	 */
	void setWindowNormalized(const float& windowNormalized)
	{
		_windowNormalized = windowNormalized;

		computeDisplayRange();
	}

	/** Returns the normalized level */
	float levelNormalized() const
	{
		return _levelNormalized;
	}

	/**
	 * Sets the normalized level
	 * @param levelNormalized Normalized level
	 */
	void setLevelNormalized(const float& levelNormalized)
	{
		_levelNormalized = levelNormalized;

		computeDisplayRange();
	}

	/** Returns the window */
	float window() const
	{
		return _window;
	}

	/**
	 * Sets the window
	 * @param window Window
	 */
	void setWindow(const float& window)
	{
		_window = window;
	}

	/** Returns the level */
	float level() const
	{
		return _level;
	}

	/**
	* Sets the level
	* @param level Level
	*/
	void setLevel(const float& level)
	{
		_level = level;
	}

	/** Compute channel elements range */
	void computeRange()
	{
		if (_elements.empty())
			return;

		_range.setFullRange();

		//qDebug() << "Compute image range";

		const auto[min, max] = std::minmax_element(_elements.begin(), _elements.end());

		_range.setMin(*min);
		_range.setMax(*max);
	}

	/** Compute display range */
	void computeDisplayRange()
	{
		//qDebug() << "Compute display range";

		const auto maxWindow = _range.length();

		_level		= std::clamp(_range.min() + (_levelNormalized * maxWindow), _range.min(), _range.max());
		_window		= std::clamp(_windowNormalized * maxWindow, _range.min(), _range.max());

		_displayRange.setMin(std::clamp(_level - (_window / 2.0f), _range.min(), _range.max()));
		_displayRange.setMax(std::clamp(_level + (_window / 2.0f), _range.min(), _range.max()));
	}

	/**
	 * Returns a reference to the i-th element
	 * @param i I-th elements to fetch
	 */
	ChannelType& operator [] (const std::uint32_t& i) {
		return _elements[i];
	}

	/**
	 * Determines whether the channel is valid (dimension identifier >= 0)
	 * @return Whether the channel is valid
	 */
	bool isValid() const
	{
		return _dimensionId >= 0;
	}

	/** Indicates that the elements have changed */
	void setChanged()
	{
		computeRange();
		computeDisplayRange();
	}

	/**
	 * Fill all elements with the supplied value
	 * @param value Value to fill
	 */
	void fill(const ChannelType& value)
	{
		std::fill(_elements.begin(), _elements.end(), value);

		setChanged();
	}

	/** Returns an HTML tooltip representation of the channel */
	QString htmlTooltip() const {
		
		QString rows;

		const auto description = QString("<table>%1</table>").arg(rows);

		return QString("<html><head / ><body><p><span style='font-weight:600;'>%1<br/></span>%2</p></body></html>").arg(_name, description);
	}

private:
	bool			_enabled;				/** Whether the channel is enabled */
	QSize			_imageSize;				/** Image size */
	std::int32_t	_id;					/** Identifier (for internal use) */
	QString			_name;					/** Name (for use in the GUI) */
	std::int32_t	_dimensionId;			/** Dimension identifier */
	Elements		_elements;				/** Elements */
	Range			_range;					/** Elements range */
	Range			_displayRange;			/** Elements display range (based on window/level) */
	float			_windowNormalized;		/** Normalized window */
	float			_levelNormalized;		/** Normalized level */
	float			_window;				/** Window */
	float			_level;					/** Level */
};

/**
* Channels class
*
* Container class for channels
*
* @author Thomas Kroes
*/
template<typename ChannelType>
class Channels
{
	//using SharedChannel = std::shared_ptr<Channel<ChannelType>>;

public: // Construction

	/**
	 * Constructor
	 * @param noChannels Number of channels to allocate
	 */
	Channels(const std::uint32_t& noChannels = 0)
	{
		for (std::uint32_t channelId = 0; channelId < noChannels; channelId++)
			_channels.push_back(std::make_shared<Channel<ChannelType>>(channelId, QString("Channel %1").arg(channelId + 1)));
	}

public: // Miscellaneous

	/**
	 * Get channel by identifier
	 * @param channelId
	 */
	Channel<ChannelType>* channel(const std::uint32_t& channelId) const {
		return _channels.at(channelId).get();
	}

	/** Return the number of channels */
	std::uint32_t noChannels() const {
		return _channels.size();
	}

private:
	std::vector<std::shared_ptr<Channel<ChannelType>>>		_channels;		/** Channels */
};