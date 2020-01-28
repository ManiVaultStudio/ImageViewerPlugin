#pragma once

#include "Quad.h"

/**
 * OpenGL Image quad class
 * @author Thomas Kroes
 */
class ImageQuad : public Quad
{
	Q_OBJECT

public:
	/** Constructor */
	ImageQuad(Renderer* renderer, const QString& name, const float& z = 0.f);

	/**
	 * Sets the image
	 * @param image Image
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

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

	/** Returns the mouse positions in screen coordinates */
	QVector<QPoint> mousePositions() const;

protected:
	/** Updates the internals of the shape */
	void update() override;

	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL textures that the shape needs */
	void addTextures();

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

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

protected:
	std::uint16_t		_imageMin;				/** Minimum pixel value in the image */
	std::uint16_t		_imageMax;				/** Maximum pixel value in the image  */
	float				_minPixelValue;			/** Window minimum pixel value */
	float				_maxPixelValue;			/** Window maximum pixel value */
	float				_windowNormalized;		/** Normalized display window */
	float				_levelNormalized;		/** Normalized display level */
	float				_window;				/** Display window */
	float				_level;					/** Display level */
	QVector<QPoint>		_mousePositions;		/** Recorded mouse positions in screen coordinates */
};