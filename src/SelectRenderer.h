#pragma once

#include "Common.h"

#include "QuadRenderer.h"

#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QRect>

class ImageViewerWidget;

class SelectRenderer : public QuadRenderer
{
public:
	SelectRenderer(const std::uint32_t& zIndex, ImageViewerWidget* imageViewerWidget);

public:
	void render() override;
	void init();

public:
	void setImageSize(const QSize& size);
	void update(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions);
	void resetSelectionBuffer();
	void setSelectionImage(std::shared_ptr<QImage> image);
	void setOpacity(const float& opacity);
	float brushRadius() const;
	void setBrushRadius(const float& brushRadius);
	float brushRadiusDelta() const;
	void setBrushRadiusDelta(const float& brushRadiusDelta);
	void brushSizeIncrease();
	void brushSizeDecrease();
	std::shared_ptr<QImage> selectionImage() const;

protected:
	bool isInitialized() const override;
	void createShaderPrograms() override;
	void createTextures() override;
	void createVBOs() override;
	void createVAOs() override;

private:
	void renderOverlay();
	void renderSelection();
	void renderOutline();
	void drawPolyline(const QVector<QVector2D>& points);

protected:
	ImageViewerWidget*			_imageViewerWidget;
	QVector4D					_color;
	QVector4D					_selectionColor;
	float						_brushRadius;
	float						_brushRadiusDelta;
	QVector4D					_outlineColor;
	QOpenGLBuffer				_outlineVBO;
	QOpenGLVertexArrayObject	_outlineVAO;
};