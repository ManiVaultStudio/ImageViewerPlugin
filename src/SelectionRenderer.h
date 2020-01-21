#pragma once

#include "Common.h"

#include "QuadRenderer.h"

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
	 * Update the (temporary) selection buffer during interaction
	 * @param selectionType Type of selection
	 * @param mousePositions Mouse positions in world space
	 */
	void updateSelectionBuffer(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions);

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

	/** Renders the selection bounds */
	void renderBounds();

	/**
	 * Draws a polyline with OpenGL
	 * @param points Polyline points in world space
	 * @param vbo Pointer to polyline Vertex Buffer Object (VBO)
	 * @param vbo Pointer to polyline Vertex Array Object (VAO)
	 * @param closed Whether to close the polyline
	 * @param lineWidth Width of the polyline in screen space
	 * @param textureScale Scale of the texture coordinate (U)
	 */
	void drawPolyline(QVector<QVector2D> points, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, const bool& closed = true, const float& lineWidth = 1.f, const float& textureScale = 0.05f);

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
	QRect				_bounds;				/** Selection bounds */
	float				_brushRadius;			/** Selection brush radius */
	float				_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
};