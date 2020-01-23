#pragma once

#include "Common.h"

#include "QuadRenderer.h"
#include "Bounds.h"

class ImageViewerWidget;

/**
 * Selection renderer class
 * This renderer class is used to display selections of high-dimensional image data using OpenGL
 * @author Thomas Kroes
 */
class SelectionRenderer : public QuadRenderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param depth Depth (layer) to render content at
	 * @param imageViewerWidget Pointer to image viewer widget
	 */
	SelectionRenderer(const float& depth, ImageViewerWidget* imageViewerWidget);

public:
	/** Renders the content */
	void render() override;

	/** Initialize the renderer */
	void init() override;

	/** Return whether the renderer is initialized */
	bool isInitialized() const override;

public:
	/**
	 * Set image size
	 * param size Size of the image
	 */
	void setImageSize(const QSize& size);

	/**
	 * Update the pixel selection buffer during interaction
	 */
	void updateSelectionBuffer();

	/** Reset the (temporary) selection buffer */
	void resetSelectionBuffer();

	/**
	 * Set the selection image
	 * @param selectionImage Selection image
	 */
	void setSelectionImage(std::shared_ptr<QImage> selectionImage);

	/**
	 * Set selection bounds
	 * @param selectionBounds Bounds of the selected pixels in the selection image
	 */
	void setSelectionBounds(const QRect& selectionBounds);

	/** Returns the selection opacity */
	float selectionOpacity() const;

	/**
	 * Sets the selection opacity
	 * @param selectionOpacity Selection opacity
	 */
	void setSelectionOpacity(const float& selectionOpacity);

	/** Returns the brush radius */
	float brushRadius() const;

	/**
	 * Sets the brush radius
	 * @param brushRadius Brush radius
	 */
	void setBrushRadius(const float& brushRadius);

	/** Returns the brush radius delta (amount to increasing/decreasing) */
	float brushRadiusDelta() const;

	/**
	 * Sets the brush radius delta (amount to increasing/decreasing)
	 * @param brushRadiusDelta Amount to add/remove 
	 */
	void setBrushRadiusDelta(const float& brushRadiusDelta);

	/** Increase the brush size by _brushRadiusDelta */
	void brushSizeIncrease();

	/** Decrease the brush size by _brushRadiusDelta */
	void brushSizeDecrease();

	/** Returns the current selection image */
	std::shared_ptr<QImage> selectionImage() const;

protected:
	void createShaderPrograms() override;
	void createTextures() override;
	void createVBOs() override;
	void createVAOs() override;

private:
	/** Renders the selection overlay */
	void renderOverlay();

	/** Renders the pixel selection */
	void renderSelection();

	/** Renders the selection geometry outline */
	void renderOutline();

signals:
	/**
	 * Invoked when the selection opacity has changed
	 * @param selectionOpacity Selection opacity
	 */
	void selectionOpacityChanged(const float& selectionOpacity);

protected:
	ImageViewerWidget*	_imageViewerWidget;		/** Pointer to image viewer widget */
	QColor				_bufferColor;			/** Selected pixel color during selection */
	QColor				_selectionColor;		/** Pixel color of selected data points */
	QColor				_boundsColor;			/** Line color of selection bounds */
	float				_boundsLineWidth;		/** Line width of selection bounds */
	QColor				_outlineColor;			/** Outline color of selection geometry */
	float				_outlineLineWidth;		/** Outline line width of selection geometry */
	//QRect				_bounds;				/** Selection bounds */
	float				_brushRadius;			/** Selection brush radius */
	float				_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
	//Polyline2D			_outlinePolyline;		/** Outline polyline */
	Bounds				_bounds;				/** Bounds shape */
};