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
	_mousePositions()
{
	_color = color;

	_receiveMouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);

	connect(renderer, &Renderer::brushRadiusChanged, this, &SelectionOutline::update);
}

void SelectionOutline::update()
{
	Polyline2D::update();

	QVector<PolylinePoint2D> polylinePoints;

	auto* outlineVBO = vbo("Polyline").get();
	auto* outlineVAO = vao("Polyline").get();

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

	for (auto position : positions) {
		polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _lineWidth));
	}

	setPoints(polylinePoints);

	emit changed(this);
}

void SelectionOutline::onMousePressEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMousePressEvent())
		return;

	Polyline2D::onMousePressEvent(mouseEvent);

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			switch (_renderer->selectionType())
			{
				case SelectionType::Rectangle:
				case SelectionType::Brush:
				case SelectionType::Lasso:
				{
					_mousePositions.clear();
					_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos()));
					break;
				}

				default:
					break;
			}
			
			break;
		}

		default:
			break;
	}
}

void SelectionOutline::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseReleaseEvent())
		return;

	Polyline2D::onMouseReleaseEvent(mouseEvent);

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			switch (_renderer->selectionType())
			{
				case SelectionType::Rectangle:
				{
					_mousePositions.clear();
					update();
					break;
				}

				default:
					break;
			}

			break;
		}

		case Qt::RightButton: 
		{
			switch (_renderer->selectionType())
			{
				case SelectionType::Polygon:
				{
					_mousePositions.clear();
					update();
					break;
				}

				default:
					break;
			}

			break;
		}

		default:
			break;
	}
}

void SelectionOutline::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseMoveEvent())
		return;

	Polyline2D::onMouseMoveEvent(mouseEvent);

	switch (_renderer->selectionType())
	{
		case SelectionType::Rectangle:
		{
			_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos()));
			update();
			break;
		}

		case SelectionType::Lasso:
		{
			if (mouseEvent->buttons()  & Qt::LeftButton) {
				
				_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos()));
				update();
			}
			
			break;
		}

		case SelectionType::Brush:
		{
			//_mousePositions.back() = _renderer->screenToWorld(modelViewMatrix(), mouseEvent->pos());
			update();
			break;
		}

		default:
			break;
	}
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

	if (name == "Polyline") {
		shaderProgram("Polyline")->setUniformValue("lineTexture", 0);
		shaderProgram("Polyline")->setUniformValue("transform", modelViewProjectionMatrix());
		shaderProgram("Polyline")->setUniformValue("lineWidth", _lineWidth);
	}
}