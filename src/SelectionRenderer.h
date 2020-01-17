#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class ImageViewerWidget;

/**
 * Selection renderer class
 * This renderer class is used to display selections of high-dimensional image data using OpenGL
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
	void setImageSize(const QSize& size);
	void updateSelectionBuffer(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions);
	void resetSelectionBuffer();
	void setSelectionImage(std::shared_ptr<QImage> image);
	void setSelectionBounds(const QRect& selectionBounds);
	float selectionOpacity() const;
	void setSelectionOpacity(const float& selectionOpacity);
	float brushRadius() const;
	void setBrushRadius(const float& brushRadius);
	float brushRadiusDelta() const;
	void setBrushRadiusDelta(const float& brushRadiusDelta);
	void brushSizeIncrease();
	void brushSizeDecrease();

	std::shared_ptr<QImage> selectionImage() const;

protected:
	void createShaderPrograms() override;
	void createTextures() override;
	void createVBOs() override;
	void createVAOs() override;

private:
	void renderOverlay();
	void renderSelection();
	void renderOutline();
	void renderBounds();
	void drawPolyline(QVector<QVector2D> points, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, const bool& closed = true, const float& lineWidth = 1.f, const float& textureScale = 0.05f);

signals:
	void selectionOpacityChanged(const float& selectionOpacity);

protected:
	ImageViewerWidget*	_imageViewerWidget;		/*! Pointer to image viewer widget */
	QColor				_bufferColor;			/*! Selected pixel color during selection */
	QColor				_selectionColor;		/*! Pixel color of selected data points */
	QColor				_boundsColor;			/*! Line color of selection bounds */
	float				_boundsLineWidth;		/*! Line width of selection bounds */
	QColor				_outlineColor;			/*! Outline color of selection geometry */
	float				_outlineLineWidth;		/*! Outline line width of selection geometry */
	QRect				_bounds;				/*! Selection bounds */
	float				_brushRadius;			/*! Selection brush radius */
	float				_brushRadiusDelta;		/*! Selection brush size increase/decrease delta */
};