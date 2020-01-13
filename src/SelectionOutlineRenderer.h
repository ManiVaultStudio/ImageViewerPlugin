#pragma once

#include "Common.h"

#include "StackedRenderer.h"

class ImageViewerWidget;

class SelectionOutlineRenderer : public StackedRenderer
{
public:
	SelectionOutlineRenderer(const std::uint32_t& zIndex, ImageViewerWidget* imageViewerWidget);

public:
	void init() override;
	void resize(QSize renderSize) override;
	void render() override;
	void destroy() override;

protected:
	bool initialized() const;

private:
	ImageViewerWidget*						_imageViewerWidget;
	std::unique_ptr<QOpenGLShaderProgram>	_program;
	QVector4D								_color;
};