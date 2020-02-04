#include "SelectionBrushProp.h"
#include "Actor.h"
#include "Renderer.h"
#include "PolylineShape.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QtMath>
#include <QDebug>

SelectionBrushProp::SelectionBrushProp(Actor* actor, const QString& name) :
	PolylineProp(actor, name),
	_brushCenter(),
	_brushRadius(1.f)
{
}

void SelectionBrushProp::setBrushCenter(const QPoint& brushCenter)
{
	if (brushCenter == _brushCenter)
		return;

	_brushCenter = brushCenter;

	updateShapes();
}

void SelectionBrushProp::setBrushRadius(const float& brushRadius)
{
	if (brushRadius == _brushRadius)
		return;

	_brushRadius = brushRadius;

	updateShapes();
}

void SelectionBrushProp::initialize()
{
	Prop::initialize();

	/*
	const auto quadShapeShaderProgram = _shaderPrograms["QuadShape"];

	quadShapeShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	quadShapeShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!quadShapeShaderProgram->link()) {
		throw std::exception("Unable to link color image quad shader program");
	}

	const auto stride = 5 * sizeof(GLfloat);

	auto quadShape = shape<QuadShape>("QuadShape");

	if (quadShapeShaderProgram->bind()) {
		quadShape->vao().bind();
		quadShape->vbo().bind();

		quadShapeShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
		quadShapeShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
		quadShapeShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		quadShapeShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
		quadShapeShaderProgram->release();

		quadShape->vao().release();
		quadShape->vbo().release();
	}
	else {
		throw std::exception("Unable to bind color image quad shader program");
	}

	const auto quadShapeTexture = _textures["QuadShape"];

	quadShapeTexture->setWrapMode(QOpenGLTexture::Repeat);
	quadShapeTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	*/

	_initialized = true;
}

void SelectionBrushProp::render()
{
	if (!canRender())
		return;

	PolylineProp::render();
/*
	const auto quadShape = _shapes["QuadShape"];
	const auto quadShapeShaderProgram = _shaderPrograms["QuadShape"];
	const auto quadShapeTexture = _textures["QuadShape"];

	quadShapeTexture->bind();

	if (quadShapeShaderProgram->bind()) {
		auto color = _color;

		color.setAlphaF(actor()->opacity());

		quadShapeShaderProgram->setUniformValue("imageTexture", 0);
		quadShapeShaderProgram->setUniformValue("color", color);
		quadShapeShaderProgram->setUniformValue("transform", actor()->modelViewProjectionMatrix() * _matrix);

		quadShape->render();

		quadShapeShaderProgram->release();
	}

	quadShapeTexture->release();
	*/
}

void SelectionBrushProp::updateShapes()
{
	PolylineProp::updateShapes();

	QVector<QVector3D> points;

	const auto brushCenter	= renderer()->screenToWorld(actor()->modelViewMatrix(), _brushCenter);
	const auto noSegments	= 32u;

	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(noSegments * 3);

	const auto brushRadius = _brushRadius * renderer()->zoom();

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta = 2.0f * M_PI * float(s) / float(noSegments);
		const auto x = brushRadius * cosf(theta);
		const auto y = brushRadius * sinf(theta);

		points.append(QVector3D(brushCenter.x() + x, brushCenter.y() + y, 0.f));
	}

	setPoints(points);
}

void SelectionBrushProp::updateTextures()
{
	PolylineProp::updateTextures();
}