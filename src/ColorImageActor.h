#pragma once

#include "Common.h"

#include <QObject>
#include <QColor>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

#include "Actor.h"

class QMouseEvent;
class QWheelEvent;

class Renderer;

/**
 * Color image actor class
 * @author Thomas Kroes
 */
class ColorImageActor : public Actor
{
	Q_OBJECT

public:
	ColorImageActor(Renderer* renderer, const QString& name);

	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

	/** Returns the image size */
	QSize imageSize();

	/** Returns the color image size */
	QSize size() const;

	/** Returns the minimum image pixel value */
	std::uint16_t imageMin() const;

	/** Returns the maximum image pixel value */
	std::uint16_t imageMax() const;

	/**
	 * Sets the image minimum and maximum pixel values
	 * @param imageMin Minimum image pixel value
	 * @param imageMax Maximum image pixel value
	 */
	void setImageMinMax(const std::uint16_t& imageMin, const std::uint16_t& imageMax);

	/** Returns the normalized display window */
	float windowNormalized() const;

	/** Returns the display window */
	float window() const;

	/** Returns the normalized display level */
	float levelNormalized() const;

	/** Returns the display level */
	float level() const;

	/**
	 * Set the display window and level
	 * @param window Display window
	 * @param level Display level
	 */
	void setWindowLevel(const float& window, const float& level);

	/** Resets the display window and level */
	void resetWindowLevel();

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

signals:
	/**
	 * Invoked when the image minimum and/or maximum pixel values change
	 * @param imageMin Minimum image pixel value
	 * @param imageMax Maximum image pixel value
	 */
	void imageMinMaxChanged(const std::uint16_t& imageMin, const std::uint16_t& imageMax);

	/**
	 * Invoked when the display window and/or level change
	 * @param window Display window
	 * @param level Display level
	 */
	void windowLevelChanged(const float& window, const float& level);

	/**
	 * Invoked when the image size changed
	 * @param imageSize Image size
	 */
	void imageSizeChanged(const QSizeF& imageSize);

	/** Invoked when window/level adjustment starts */
	void startWindowLevel();

	/** Invoked when window/level adjustment end */
	void endWindowLevel();

private:
	std::uint16_t		_imageMin;				/** Minimum pixel value in the image */
	std::uint16_t		_imageMax;				/** Maximum pixel value in the image */
	float				_windowNormalized;		/** Normalized display window */
	float				_levelNormalized;		/** Normalized display level */
	float				_window;				/** Display window */
	float				_level;					/** Display level */
};