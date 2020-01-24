#pragma once

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
	QMap<QString, QSharedPointer<Shape>>	_shapes;				/** Shapes map */
};