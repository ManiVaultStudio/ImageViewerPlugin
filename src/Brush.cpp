#include "Brush.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QDebug>

#include <QtMath>

#include "Renderer.h"

Brush::Brush(Actor* actor, const QString& name /*= "Brush"*/, const float& z /*= 0.f*/, const QColor& color /*= QColor(255, 153, 0, 150)*/) :
	Polyline2D(actor, name, z, true, 0.05f),
	_mousePositions()
{
	_color = color;

	connect(renderer(), &Renderer::brushRadiusChanged, this, &Brush::update);
}

void Brush::update()
{
	Polyline2D::update();

	QVector<PolylinePoint2D> polylinePoints;

	auto* outlineVBO = vbo("Polyline").get();
	auto* outlineVAO = vao("Polyline").get();

	QVector<QVector3D> positions;

	if (_mousePositions.size() >= 1) {
		const auto brushCenter = _mousePositions.back();
		const auto noSegments = 128u;

		std::vector<GLfloat> vertexCoordinates;

		vertexCoordinates.resize(noSegments * 3);

		const auto brushRadius = renderer()->brushRadius() *renderer()->zoom();

		for (std::uint32_t s = 0; s < noSegments; s++) {
			const auto theta = 2.0f * M_PI * float(s) / float(noSegments);
			const auto x = brushRadius * cosf(theta);
			const auto y = brushRadius * sinf(theta);

			positions.append(QVector3D(brushCenter.x() + x, brushCenter.y() + y, 0.f));
		}
	}

	for (auto position : positions) {
		polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _lineWidth));
	}

	setPoints(polylinePoints);

	emit changed(this);
}

void Brush::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), _color);

	setTexture("Polyline", QSharedPointer<QOpenGLTexture>::create(textureImage));

	auto polylineTexture = texture("Polyline");

	polylineTexture->setWrapMode(QOpenGLTexture::Repeat);
	polylineTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

void Brush::configureShaderProgram(const QString& name)
{
	Polyline2D::configureShaderProgram(name);

	if (name == "Polyline") {
		shaderProgram("Polyline")->setUniformValue("lineTexture", 0);
		shaderProgram("Polyline")->setUniformValue("transform", _actor->modelViewProjectionMatrix());
		shaderProgram("Polyline")->setUniformValue("lineWidth", _lineWidth);
	}
}