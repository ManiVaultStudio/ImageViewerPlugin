#pragma once

#include "Common.h"

#include "QuadRenderer.h"

#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QRect>
#include <QOpenGLFramebufferObject>

class ImageViewerWidget;

class SelectRenderer : public QuadRenderer
{
public:
	SelectRenderer(const std::uint32_t& zIndex, ImageViewerWidget* imageViewerWidget);

public:
	void render() override;
	void init();

protected:
	void initializePrograms();

public:
	void setImageSize(const QSize& size);
	void update(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions);
	void reset();
	float brushRadius() const;
	void setBrushRadius(const float& brushRadius);
	float brushRadiusDelta() const;
	void setBrushRadiusDelta(const float& brushRadiusDelta);
	void brushSizeIncrease();
	void brushSizeDecrease();
	std::shared_ptr<QImage> selectionImage() const;

protected:
	bool initialized() const;

private:
	void renderOverlay();
	void renderOutline();
	void drawSelectionOutlineRectangle(const QVector3D& start, const QVector3D& end);
	void drawSelectionOutlineBrush();
	void drawSelectionOutlineLasso();
	void drawSelectionOutlinePolygon();

protected:
	ImageViewerWidget*							_imageViewerWidget;
	std::unique_ptr<QOpenGLTexture>				_texture;
	std::unique_ptr<QOpenGLFramebufferObject>	_fbo;
	QVector4D									_color;
	float										_brushRadius;
	float										_brushRadiusDelta;
	std::unique_ptr<QOpenGLShaderProgram>		_pixelSelectionProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_outlineProgram;
};