#pragma once

#include "Range.h"

#include <QObject>
#include <QColor>
#include <QImage>
#include <QVector2D>

/**
* Channel class
*
* TODO
*
* @author Thomas Kroes
*/
class Channel : public QObject
{
	Q_OBJECT

public:

	using Elements = QVector<float>;

public:
	/**
	 * (Default) constructor
	 * @param parent Parent object
	 * @param id Channel identifier
	 * @param name Channel name
	 * @param dimensionId Channel dimension identifier
	 */
	Channel(QObject* parent, const std::uint32_t& id, const QString& name, const std::uint32_t& dimensionId = -1);

	/** Returns whether the channel is enabled */
	bool enabled() const;

	/**
	 * Sets  whether the channel is enabled
	 * @param enabled Whether the channel is enabled
	 */
	void setEnabled(const bool& enabled);

	/** Returns whether the channel is inverted */
	bool inverted() const;

	/**
	 * Sets  whether the channel is inverted
	 * @param enabled Whether the channel is inverted
	 */
	void setInverted(const bool& inverted);

	/** Returns the channel image size */
	QSize imageSize() const;

	/**
	 * Sets the channel image size
	 * @param imageSize Channel image size
	 */
	void setImageSize(const QSize& imageSize);

	/** Returns the channel identifier */
	const std::int32_t& id() const;

	/** Returns the channel name */
	QString name() const;

	/**
	 * Sets the channel name
	 * @param name Channel name
	 */
	void setName(const QString& name);

	/** Returns the channel dimension identifier */
	const std::int32_t& dimensionId() const;

	/**
	 * Sets the channel dimension identifier
	 * @param dimensionId Channel dimension identifier
	 */
	void setDimensionId(const std::int32_t& dimensionId);

	/** Returns the channel elements */
	const Elements& elements() const;

	/**
	 * Sets the channel elements
	 * @param elements Elements
	 */
	void setElements(const Elements& elements);

	/** Returns the channel elements range */
	Range range() const;

	/** Returns the display range */
	Range displayRange() const;

	/** Returns the display range in vector format */
	QVector2D displayRangeVector() const;

	/** Returns the normalized window */
	float windowNormalized() const;

	/**
	 * Sets the normalized window
	 * @param windowNormalized Normalized window
	 */
	void setWindowNormalized(const float& windowNormalized);

	/** Returns the normalized level */
	float levelNormalized() const;

	/**
	 * Sets the normalized level
	 * @param levelNormalized Normalized level
	 */
	void setLevelNormalized(const float& levelNormalized);

	/** Returns the window */
	float window() const;

	/**
	 * Sets the window
	 * @param window Window
	 */
	void setWindow(const float& window);

	/** Returns the level */
	float level() const;

	/**
	* Sets the level
	* @param level Level
	*/
	void setLevel(const float& level);

	/** Compute channel elements range */
	void computeRange();

	/** Compute display range */
	void computeDisplayRange();

	/**
	 * Returns a reference to the i-th element
	 * @param i I-th elements to fetch
	 */
	float& operator [] (const std::uint32_t& i) {
		return _elements[i];
	}

	/**
	 * Determines whether the channel is valid (dimension identifier >= 0)
	 * @return Whether the channel is valid
	 */
	bool isValid() const;

	/** Indicates that the elements have changed */
	void setChanged();

signals:
	
	/** Signals that the elements have been modified */
	void changed(Channel*);

private:
	bool			_enabled;				/** Whether the channel is enabled */
	bool			_inverted;				/** Whether the channel is inverted */
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