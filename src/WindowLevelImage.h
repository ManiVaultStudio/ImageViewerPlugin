#pragma once

#include "Range.h"

#include <QColor>
#include <QImage>

/**
* Range image class
*
* Wraps QT image class and adds window/level support
*
* @author Thomas Kroes
*/
class WindowLevelImage
{

public:
	/** Default constructor */
	WindowLevelImage();

	/**
		* Get image
		* @param role Data role
		*/
	QVariant image(const int& role) const;

	/**
		* Set image
		* @param image Image
		*/
	void setImage(const QImage& image);

	/**
		* Get image range
		* @param role Data role
		*/
	QVariant imageRange(const int& role) const;

	/**
		* Get display range
		* @param role Data role
		*/
	QVariant displayRange(const int& role) const;

	/**
		* Get normalized window
		* @param role Data role
		*/
	QVariant windowNormalized(const int& role) const;

	/**
		* Set normalized window
		* @param windowNormalized Normalized window
		*/
	void setWindowNormalized(const float& windowNormalized);

	/**
		* Get normalized level
		* @param role Data role
		*/
	QVariant levelNormalized(const int& role) const;

	/**
		* Set normalized level
		* @param levelNormalized Normalized level
		*/
	void setLevelNormalized(const float& levelNormalized);

	/**
		* Get window
		* @param role Data role
		*/
	QVariant window(const int& role) const;

	/**
		* Set window
		* @param window Window
		*/
	void setWindow(const float& window);

	/**
		* Get level
		* @param role Data role
		*/
	QVariant level(const int& role) const;

	/**
		* Set level
		* @param level Level
		*/
	void setLevel(const float& level);

	/** Compute image range */
	void computeImageRange();

	/** Compute display range */
	void computeDisplayRange();

private:
	QImage	_image;					/** Input image */
	Range	_imageRange;			/** Pixel range of input image */
	Range	_displayRange;			/** Pixel range based on window/level */
	float	_windowNormalized;		/** Normalized window */
	float	_levelNormalized;		/** Normalized level */
	float	_window;				/** Window */
	float	_level;					/** Level */
};