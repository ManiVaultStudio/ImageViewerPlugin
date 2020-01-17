#pragma once

#include "Common.h"

#include "QuadRenderer.h"

/**
 * Image quad renderer class
 * This renderer class is used to display textured quads using OpenGL
 */
class ImageQuadRenderer : public QuadRenderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param depth Depth (layer) to render content at
	 */
	ImageQuadRenderer(const float& depth);

public:
	/** Initializes the renderer */
	void init() override;

	/** Renders the content */
	void render() override;

	/** Return whether the renderer is initialized */
	bool isInitialized() const override;

public:
	/**
	 * Set the image
	 * @param image Image to display
	 */
	void setImage(std::shared_ptr<QImage> image);

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

protected:
	/** Create the necessary OpenGL shader programs */
	void createShaderPrograms() override;

	/** Create the necessary OpenGL texture */
	void createTextures() override;

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

private:
	std::uint16_t	_imageMin;				/*! Minimum pixel value in the image */
	std::uint16_t	_imageMax;				/*! Maximum pixel value in the image  */
	float			_windowNormalized;		/*! Normalized window */
	float			_levelNormalized;		/*! Normalized level */
	float			_window;				/*! Window */
	float			_level;					/*! Level */
};