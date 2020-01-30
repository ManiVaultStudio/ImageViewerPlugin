#include "SelectionOutline.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QDebug>

#include <QtMath>

#include "Renderer.h"

SelectionOutline::SelectionOutline(Renderer* renderer, const QString& name /*= "SelectionOutline"*/, const float& z /*= 0.f*/, const QColor& color /*= QColor(255, 153, 0, 150)*/) :
	Polyline2D(renderer, name, z, true, 0.005f),
	_color(color),
	_viewRectangle(),
	_mousePositions()
{
	_handleMouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);

	connect(renderer, &Renderer::brushRadiusChanged, this, &SelectionOutline::update);
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

void SelectionOutline::update()
{
	Polyline2D::update();

	auto polylineShaderProgram = shaderProgram("Polyline");

	if (polylineShaderProgram->bind()) {
		QMatrix4x4 transform;

		transform.ortho(_viewRectangle);

		polylineShaderProgram->setUniformValue("stippleTexture", 0);
		polylineShaderProgram->setUniformValue("color", _color);

		auto* outlineVBO = vbo("Polyline").get();
		auto* outlineVAO = vao("Polyline").get();

		auto outlineStippleTexture = texture("Polyline");

		outlineStippleTexture->bind();
		{
			QVector<QVector3D> positions;

			switch (_renderer->selectionType())
			{
				case SelectionType::Rectangle:
				{
					if (_mousePositions.size() >= 2) {
						const auto start	= _mousePositions.front();
						const auto end		= _mousePositions.back();

						positions.append(QVector3D(start.x(),	start.y(),	0.f));
						positions.append(QVector3D(start.x(),	start.y(),	0.f));
						positions.append(QVector3D(end.x(),		start.y(),	0.f));
						positions.append(QVector3D(end.x(),		end.y(),	0.f));
						positions.append(QVector3D(start.x(),	end.y(),	0.f));
						positions.append(QVector3D(start.x(),	start.y(),	0.f));
					}
					break;
				}

				case SelectionType::Brush:
				{
					if (_mousePositions.size() >= 1) {
						const auto brushCenter	= _mousePositions.back();
						const auto noSegments	= 128u;

						std::vector<GLfloat> vertexCoordinates;

						vertexCoordinates.resize(noSegments * 3);

						const auto brushRadius = _renderer->brushRadius() *_renderer->zoom();

						for (std::uint32_t s = 0; s < noSegments; s++) {
							const auto theta	= 2.0f * M_PI * float(s) / float(noSegments);
							const auto x		= brushRadius * cosf(theta);
							const auto y		= brushRadius * sinf(theta);

							positions.append(QVector3D(brushCenter.x() + x, brushCenter.y() + y, 0.f));
						}
					}

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					if (_mousePositions.size() >= 2) {
						for (const auto& mousePosition : _mousePositions) {
							positions.append(QVector3D(mousePosition.x(), mousePosition.y(), 0.f));
						}
					}
					break;
				}

				default:
					break;
			}

			QVector<PolylinePoint2D> polylinePoints;

			for (auto position : positions) {
				polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _lineWidth));
			}

			setPoints(polylinePoints);
		}
		outlineStippleTexture->release();

		polylineShaderProgram->release();
	}

	emit changed(this);
}

void SelectionOutline::reset()
{
	qDebug() << "Reset" << _name;
}

void SelectionOutline::onMousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event for" << _name;

	if (_renderer->selectionType() != SelectionType::Polygon) {
		_mousePositions.clear();
	}
	
	if (mouseEvent->button() == Qt::LeftButton) {
		_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos()));
	}

	update();
}

void SelectionOutline::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event for" << _name;

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_renderer->selectionType() != SelectionType::Polygon) {
				deactivate();
			}

			break;
		}

		case Qt::RightButton: 
		{
			if (_renderer->selectionType() == SelectionType::Polygon) {
				deactivate();
			}

			break;
		}

		default:
			break;
	}

	update();
}

void SelectionOutline::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse move event for" << _name;
	
	if (_renderer->selectionType() != SelectionType::Polygon) {
		_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos()));
		update();
	}
	else {
		_mousePositions.back() = _renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos());
	}
}

void SelectionOutline::activate()
{
	if (isActive())
		return;

	Polyline2D::activate();

	reset();
}

void SelectionOutline::deactivate()
{
	if (!isActive())
		return;

	Polyline2D::deactivate();

	reset();
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
}