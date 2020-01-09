#pragma once

#include "Common.h"

#include "QuadRenderer.h"

#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QImage>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLDebugLogger>

class ImageQuadRenderer : public QuadRenderer
{
public:
	ImageQuadRenderer();
    ~ImageQuadRenderer();

public:
	void init() override;
	void render() override;

public:
	void setImage(std::shared_ptr<QImage> image);
	float window() const;
	float level() const;
	void setWindowLevel(const float& window, const float& level);
	void resetWindowLevel();

private:
	std::uint16_t	_imageMin;
	std::uint16_t	_imageMax;
	float			_window;
	float			_level;
};