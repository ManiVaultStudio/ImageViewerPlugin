#include "SelectionOutlineRenderer.h"
#include "ImageViewerWidget.h"

#include "Shaders.h"

SelectionOutlineRenderer::SelectionOutlineRenderer(const std::uint32_t& zIndex, ImageViewerWidget* imageViewerWidget) :
	StackedRenderer(zIndex),
	_imageViewerWidget(imageViewerWidget),
	_color(1.f, 0.f, 0.f, 0.6f),
	_program(std::make_unique<QOpenGLShaderProgram>())
{
}

void SelectionOutlineRenderer::init()
{
	initializeOpenGLFunctions();

	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOutlineVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOutlineFragmentShaderSource.c_str());
	_program->link();
}

void SelectionOutlineRenderer::resize(QSize renderSize)
{
}

void SelectionOutlineRenderer::render()
{
	if (!initialized())
		return;

	

	
}

void SelectionOutlineRenderer::destroy()
{
}

bool SelectionOutlineRenderer::initialized() const
{
	return true;// _texture.get() != nullptr && _texture->isCreated();
}



