#pragma once

#include "Common.h"

#include "StackedRenderer.h"
#include "Shape.h"

class SelectionBufferQuad;

class ImageViewerWidget;

/**
 * Selection renderer class
 * This renderer class is used to display selections of high-dimensional image data using OpenGL
 * @author Thomas Kroes
 */
class Renderer : public StackedRenderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param depth Depth (layer) to render content at
	 * @param imageViewerWidget Pointer to image viewer widget
	 */
	Renderer(const float& depth, ImageViewerWidget* imageViewerWidget);

public:
	/** Renders the content */
	void render() override;

	/** Resizes the renderer */
	void resize(QSize renderSize) override;

	/** Initialize the renderer */
	void init() override;

	/** Destroys the renderer */
	void destroy() override;

	/** Return whether the renderer is initialized */
	bool isInitialized() const override;

public:
	/**
	 * Sets the color image
	 * @param colorImage Color image
	 */
	void setColorImage(std::shared_ptr<QImage> colorImage);

	/**
	 * Sets the selection image
	 * @param selectionImage Selection image
	 */
	void setSelectionImage(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);

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

	/** Returns the selection opacity */
	float selectionOpacity();

	/**
	 * Sets the selection  opacity
	 * @param selectionOpacity Selection opacity
	 */
	void setSelectionOpacity(const float& selectionOpacity);

	/** Returns the current selection image */
	std::shared_ptr<QImage> selectionImage() const;

	/** Updates the selection buffer quad */
	void updateSelectionBufferQuad();

	/** Returns the selection buffer quad */
	void resetSelectionBufferQuad();

	/**
	 * Get shape by name
	 * @param name Name of the shape
	 */
	template<typename T>
	T* shape(const QString& name);

private:
	/** Create shapes */
	void createShapes();

	/** Initialize shapes */
	void initializeShapes();

	/** Render shapes */
	void renderShapes();

	/** Destroy shapes */
	void destroyShapes();

protected:
	ImageViewerWidget*						_imageViewerWidget;		/** Pointer to image viewer widget */
	QColor									_bufferColor;			/** Selected pixel color during selection */
	QColor									_selectionColor;		/** Pixel color of selected data points */
	float									_brushRadius;			/** Selection brush radius */
	float									_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
	QMap<QString, QSharedPointer<Shape>>	_shapes;				/** Shapes map */
	
};