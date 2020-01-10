#pragma once

#include "Common.h"

#include "QuadRenderer.h"

#include <QOpenGLTexture>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QVector4D>
#include <QRect>
#include <QOpenGLFramebufferObject>

class SelectRenderer : public QuadRenderer
{
public:
	SelectRenderer(const std::uint32_t& zIndex);

public:
	void init() override;
	void resize(QSize renderSize) override;
	void render() override;
	void destroy() override;

	void initializeProgram();

public:
	void setSize(const QSize& size);
	void updatePixelSelection();
	void resetPixelSelection();

protected:

protected:
	std::unique_ptr<QOpenGLFramebufferObject>	_pixelSelectionFBO;
	QVector4D									_pixelSelectionColor;
};