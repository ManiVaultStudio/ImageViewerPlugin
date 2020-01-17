#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class ImageViewerWidget;

class SelectionRenderer : public QuadRenderer
{
	Q_OBJECT

public:
	SelectionRenderer(const float& depth, ImageViewerWidget* imageViewerWidget);

public:
	void render() override;
	void init();

	bool isInitialized() const override;

public:
	void setImageSize(const QSize& size);
	void updateSelectionBuffer(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions);
	void resetSelectionBuffer();
	void setSelectionImage(std::shared_ptr<QImage> image);
	void setSelectionBounds(const QRect& selectionBounds);
	void setOpacity(const float& opacity);
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