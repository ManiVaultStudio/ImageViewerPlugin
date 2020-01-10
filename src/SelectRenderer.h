#pragma once

#include "Common.h"

#include "QuadRenderer.h"

#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QRect>
#include <QOpenGLFramebufferObject>

class SelectRenderer : public QuadRenderer
{
public:
	SelectRenderer(const std::uint32_t& zIndex);

public:
	void render() override;

protected:
	void initializePrograms();

public:
	void setImageSize(const QSize& size);
	void updatePixelSelection(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions);
	void resetPixelSelection();
	float brushRadius() const;
	void setBrushRadius(const float& brushRadius);
	float brushRadiusDelta() const;
	void setBrushRadiusDelta(const float& brushRadiusDelta);
	void brushSizeIncrease();
	void brushSizeDecrease();

protected:
	bool initialized() const;

protected:
	std::unique_ptr<QOpenGLTexture>				_texture;
	std::unique_ptr<QOpenGLFramebufferObject>	_fbo;
	QVector4D									_color;
	float										_brushRadius;
	float										_brushRadiusDelta;
	std::unique_ptr<QOpenGLShaderProgram>		_pixelSelectionProgram;
};