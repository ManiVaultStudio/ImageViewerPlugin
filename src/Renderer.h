#pragma once

#include "ImageData/ImageData.h"

#include "Common.h"

#include "StackedRenderer.h"
#include "Shape.h"

class ImageQuad;
class SelectionBufferQuad;
class SelectionOutline;

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

	/** Returns the selection opacity */
	float selectionOpacity();

	/**
	 * Sets the selection  opacity
	 * @param selectionOpacity Selection opacity
	 */
	void setSelectionOpacity(const float& selectionOpacity);

	/** Returns the image quad */
	ImageQuad* imageQuad();

	/** Returns the selection buffer quad */
	SelectionBufferQuad* selectionBufferQuad();

	/** Returns the selection outline */
	SelectionOutline* selectionOutline();

	/**
	 * Get shape by name
	 * @param name Name of the shape
	 */
	template<typename T>
	T* shape(const QString& name);

	/** Returns the selection type */
	SelectionType selectionType() const;

	/**
	 * Sets the selection type
	 * @param selectionType Selection type
	 */
	void setSelectionType(const SelectionType& selectionType);

	/** Returns the selection modifier */
	SelectionModifier selectionModifier() const;

	/**
	 * Sets the selection modifier
	 * @param selectionModifier Selection modifier
	 */
	void setSelectionModifier(const SelectionModifier& selectionModifier);

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

private:
	/** Create shapes */
	void createShapes();

	/** Initialize shapes */
	void initializeShapes();

	/** Render shapes */
	void renderShapes();

	/** Destroy shapes */
	void destroyShapes();

signals:
	/**
	 * Invoked when the selection type changed
	 * @param selectionType Selection type
	 */
	void selectionTypeChanged(const SelectionType& selectionType);

	/**
	 * Invoked when the selection modifier changed
	 * @param selectionModifier Selection modifier
	 */
	void selectionModifierChanged(const SelectionModifier& selectionModifier);

	/**
	 * Invoked when the brush radius changed
	 * @param brushRadius Brush radius
	 */
	void brushRadiusChanged(const float& brushRadius);

	/**
	 * Invoked when the brush radius delta changed
	 * @param brushRadiusDelta Brush radius delta
	 */
	void brushRadiusDeltaChanged(const float& brushRadiusDelta);

protected:
	ImageViewerWidget*						_imageViewerWidget;		/** Pointer to image viewer widget */
	QMap<QString, QSharedPointer<Shape>>	_shapes;				/** Shapes map */
	SelectionType							_selectionType;			/** Type of selection e.g. rectangle, brush */
	SelectionModifier						_selectionModifier;		/** The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	float									_brushRadius;			/** Brush radius */
	float									_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
};