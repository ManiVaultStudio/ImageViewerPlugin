#include "SelectionBoundsRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

SelectionBoundsRenderer::SelectionBoundsRenderer() :
	Renderer(),
	_vertexData(),
	_vbo(),
	_vao(),
	_program(std::make_unique<QOpenGLShaderProgram>()),
	_modelViewProjection(),
	_selectionBoundsColor(1.0f, 0.6f, 0.f, 0.5f),
	_selectionBounds()
{
	_vertexData.resize(20);
}

void SelectionBoundsRenderer::init()
{
	initializeOpenGLFunctions();
	initializeProgram();

	/*
	_program = std::make_unique<QOpenGLShaderProgram>();

	// Shader program
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, imageVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, imageFragmentShaderSource.c_str());
	_program->link();

	// Vertex buffer object
	_vbo.create();
	_vbo.bind();
	_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_vbo.release();

	// Vertex array object
	_vao.create();

	_program->bind();

	_vao.bind();
	{
		_vbo.bind();

		_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
	}
	_vao.release();
	_vbo.release();

	_program->release();
	*/
}

void SelectionBoundsRenderer::resize(QSize renderSize)
{

}

void SelectionBoundsRenderer::render()
{
	if (!initialized())
		return;

	//qDebug() << "Draw selection bounds" << _selectionBounds;

	/*
	const GLfloat boxScreen[4] = {
		_selectionBounds.left(), _selectionBounds.right(),
		_displayImage->height() - _selectionBounds.top(), _displayImage->height() - _selectionBounds.bottom()
	};

	const GLfloat vertexCoordinates[] = {
		boxScreen[0],		boxScreen[3] - 1.f, 0.0f,
		boxScreen[1] + 1.f, boxScreen[3] - 1.f, 0.0f,
		boxScreen[1] + 1.f,	boxScreen[2],		0.0f,
		boxScreen[0],		boxScreen[2],		0.0f
	};

	const auto vertexLocation = _program->attributeLocation("vertex");

	_program->setAttributeArray(vertexLocation, vertexCoordinates, 3);

	glLineWidth(2.f);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	_program->disableAttributeArray(vertexLocation);
	*/
}

void SelectionBoundsRenderer::destroy()
{
	//_texture->destroy();
	_vbo.destroy();
	_vao.destroy();
}

void SelectionBoundsRenderer::initializeProgram()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	_program->link();
}

void SelectionBoundsRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;
}

void SelectionBoundsRenderer::setSelectionBounds(const QRect& selectionBounds)
{
	_selectionBounds = selectionBounds;
}

bool SelectionBoundsRenderer::initialized() const
{
	return _selectionBounds.isValid();
}