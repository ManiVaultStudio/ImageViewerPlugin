#include "SelectionOutline.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QDebug>

#include "Shaders.h"

#include <QtMath>

SelectionOutline::SelectionOutline(Renderer* renderer, const QString& name /*= "SelectionOutline"*/, const float& z /*= 0.f*/, const QColor& color /*= QColor(255, 153, 0, 150)*/) :
	Polyline2D(renderer, name, z, true, 0.5f),
	_color(color),
	_mousePositions()
{
	_mouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);
}

QColor SelectionOutline::color() const
{
	return _color;
}

void SelectionOutline::setColor(const QColor& color)
{
	if (color == _color)
		return;

	qDebug() << "Set selection bounds shape color";

	_color = color;

	emit colorChanged(_color);
}

QRect SelectionOutline::viewRectangle() const
{
	return _viewRectangle;
}

void SelectionOutline::setViewRectangle(const QRect& viewRectangle)
{
	if (viewRectangle == _viewRectangle)
		return;

	qDebug() << "Set view size for" << _name;

	_viewRectangle = viewRectangle;

	emit viewRectangleChanged(_viewRectangle);
}

void SelectionOutline::update(std::vector<QVector3D> mousePositions, const SelectionType& selectionType, const float& brushRadius)
{
	auto polylineShaderProgram = shaderProgram("Polyline");

	if (polylineShaderProgram->bind()) {
		QMatrix4x4 transform;

		transform.ortho(_viewRectangle);

		polylineShaderProgram->setUniformValue("stippleTexture", 0);
		polylineShaderProgram->setUniformValue("transform", modelViewProjectionMatrix());
		polylineShaderProgram->setUniformValue("color", _color);

		auto* outlineVBO = vbo("Polyline").get();
		auto* outlineVAO = vao("Polyline").get();

		auto outlineStippleTexture = texture("Polyline");

		outlineStippleTexture->bind();
		{
			QVector<QVector2D> points;

			switch (selectionType)
			{
				case SelectionType::Rectangle:
				{
					if (mousePositions.size() >= 2) {
						const auto start = mousePositions.front();
						const auto end = mousePositions.back();

						points.append(QVector2D(start.x(), start.y()));
						points.append(QVector2D(end.x(), start.y()));
						points.append(QVector2D(end.x(), end.y()));
						points.append(QVector2D(start.x(), end.y()));

						setPoints(points);
					}
					break;
				}

				case SelectionType::Brush:
				{
					if (mousePositions.size() >= 1) {
						const auto brushCenter = mousePositions.back();
						const auto noSegments = 128u;

						std::vector<GLfloat> vertexCoordinates;

						vertexCoordinates.resize(noSegments * 3);

						for (std::uint32_t s = 0; s < noSegments; s++) {
							const auto theta = 2.0f * M_PI * float(s) / float(noSegments);
							const auto x = brushRadius * cosf(theta);
							const auto y = brushRadius * sinf(theta);

							points.append(QVector2D(brushCenter.x() + x, brushCenter.y() + y));
						}

						setPoints(points);
					}

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					if (mousePositions.size() >= 2) {
						for (const auto& mousePosition : mousePositions) {
							points.append(QVector2D(mousePosition.x(), mousePosition.y()));
						}

						setPoints(points);
					}
					break;
				}

				default:
					break;
			}
		}
		outlineStippleTexture->release();

		polylineShaderProgram->release();
	}
}

void SelectionOutline::reset()
{
	qDebug() << "Reset" << _name;
}

void SelectionOutline::onMousePressEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse press event for" << _name;

	_mousePositions.clear();
	_mousePositions.push_back(mouseEvent->pos());
}

void SelectionOutline::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse release event for" << _name;
}

void SelectionOutline::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse move event for" << _name;
}

void SelectionOutline::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Polyline", QSharedPointer<QOpenGLShaderProgram>::create());

	auto polylineShaderProgram = shaderProgram("Polyline");

	polylineShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOutlineVertexShaderSource.c_str());
	polylineShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOutlineFragmentShaderSource.c_str());
	polylineShaderProgram->link();
}

void SelectionOutline::addVAOs()
{
	qDebug() << "Add OpenGL VAO's to" << _name << "shape";

	addVAO("Polyline", QSharedPointer<QOpenGLVertexArrayObject>::create());

	vao("Polyline")->create();
}

void SelectionOutline::addVBOs()
{
	qDebug() << "Add OpenGL VBO's to" << _name << "shape";

	addVBO("Polyline", QSharedPointer<QOpenGLBuffer>::create());

	vbo("Polyline")->create();
}

void SelectionOutline::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), _color);

	addTexture("Polyline", QSharedPointer<QOpenGLTexture>::create(textureImage));

	auto polylineTexture = texture("Polyline");

	polylineTexture->setWrapMode(QOpenGLTexture::Repeat);
	polylineTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

void SelectionOutline::configureShaderProgram(const QString& name)
{
	Polyline2D::configureShaderProgram(name);

	auto polylineProgram = shaderProgram("Polyline");

	if (name == "Polyline") {
		QMatrix4x4 translate;

		translate.translate(0.f, 0.f, _z);

		polylineProgram->setUniformValue("transform", modelViewProjectionMatrix() * translate);
	}
}