#include "SelectionBoundsRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

SelectionBoundsRenderer::SelectionBoundsRenderer(const std::uint32_t& zIndex) :
	StackedRenderer(zIndex),
	_vertexData(),
	_vbo(),
	_vao(),
	_color(1.0f, 0.6f, 0.f, 0.5f)
{
	_vertexData.resize(12);
}

void SelectionBoundsRenderer::init()
{
	StackedRenderer::init();

	_vbo.create();
	_vbo.bind();
	_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_vbo.release();

	_vao.create();

	auto boundsOutlineProgram = shaderProgram("Outline");

	if (boundsOutlineProgram->bind()) {
		_vao.bind();
		_vbo.bind();

		boundsOutlineProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		boundsOutlineProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3);

		_vao.release();
		_vbo.release();

		boundsOutlineProgram->release();
	}
}

void SelectionBoundsRenderer::resize(QSize renderSize)
{
}

void SelectionBoundsRenderer::render()
{
	if (!isInitialized())
		return;

	auto boundsOutlineProgram = shaderProgram("Outline");

	if (boundsOutlineProgram->bind()) {
		boundsOutlineProgram->setUniformValue("transform", _modelViewProjection);
		boundsOutlineProgram->setUniformValue("color", _color);

		_vao.bind();
		{
			glLineWidth(2.f);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		}
		_vao.release();

		boundsOutlineProgram->release();
	}
}

void SelectionBoundsRenderer::destroy()
{
	_vbo.destroy();
	_vao.destroy();
}

bool SelectionBoundsRenderer::isInitialized() const
{
	return true;// _selectionBounds.isValid();
}

void SelectionBoundsRenderer::createShaderPrograms()
{
	auto boundsOutlineProgram = std::make_shared<QOpenGLShaderProgram>();

	boundsOutlineProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	boundsOutlineProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	boundsOutlineProgram->link();

	_shaderPrograms.insert("Outline", boundsOutlineProgram);
}

void SelectionBoundsRenderer::createTextures()
{
}