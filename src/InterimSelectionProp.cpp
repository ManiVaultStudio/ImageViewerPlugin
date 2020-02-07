#include "InterimSelectionProp.h"
#include "QuadShape.h"
#include "SelectionPickerActor.h"
#include "Renderer.h"

#include <QDebug>

const std::string quadVertexShaderSource =
#include "InterimSelectionQuadVertex.glsl"
;

const std::string quadFragmentShaderSource =
#include "InterimSelectionQuadFragment.glsl"
;

const std::string offscreenBufferVertexShaderSource =
#include "InterimSelectionOffscreenBufferVertex.glsl"
;

const std::string offscreenBufferFragmentShaderSource =
#include "InterimSelectionOffscreenBufferFragment.glsl"
;

InterimSelectionProp::InterimSelectionProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_fbo()
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addShaderProgram("OffscreenBuffer");
}

void InterimSelectionProp::initialize()
{
	try
	{
		Prop::initialize();

		auto shape = shapeByName<QuadShape>("Quad");

		const auto stride = 5 * sizeof(GLfloat);

		const auto quadShaderProgram = shaderProgramByName("Quad");

		if (!quadShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, quadVertexShaderSource.c_str()))
			throw std::exception("Unable to compile quad vertex shader");

		if (!quadShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, quadFragmentShaderSource.c_str()))
			throw std::exception("Unable to compile quad fragment shader");

		if (!quadShaderProgram->link())
			throw std::exception("Unable to link quad shader program");

		if (quadShaderProgram->bind()) {
			shape->vao().bind();
			shape->vbo().bind();

			quadShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
			quadShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
			quadShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
			quadShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

			shape->vao().release();
			shape->vbo().release();

			quadShaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		const auto offscreenBufferShaderProgram = shaderProgramByName("OffscreenBuffer");

		if (!offscreenBufferShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, offscreenBufferVertexShaderSource.c_str()))
			throw std::exception("Unable to compile off screen buffer vertex shader");

		if (!offscreenBufferShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, offscreenBufferFragmentShaderSource.c_str()))
			throw std::exception("Unable to compile off screen buffer fragment shader");

		if (!offscreenBufferShaderProgram->link())
			throw std::exception("Unable to link off screen buffer shader program");

		if (offscreenBufferShaderProgram->bind()) {
			shape->vao().bind();
			shape->vbo().bind();

			offscreenBufferShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
			offscreenBufferShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
			offscreenBufferShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
			offscreenBufferShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

			shape->vao().release();
			shape->vbo().release();

			offscreenBufferShaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind off screen buffer shader program");
		}

		_initialized = true;
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "initialization failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "initialization failed due to unhandled exception";
	}
}

bool InterimSelectionProp::canRender() const
{
	if (!Prop::canRender())
		return false;

	if (_fbo.isNull())
		return false;

	return _fbo->isValid();
}

void InterimSelectionProp::render()
{
	try {
		if (!canRender())
			return;

		Prop::render();

		const auto shape			= shapeByName<QuadShape>("Quad");
		const auto shaderProgram	= shaderProgramByName("Quad");

		if (shaderProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, _fbo->texture());

			shaderProgram->setUniformValue("offscreenBufferTexture", 0);
			shaderProgram->setUniformValue("color", QColor(255, 0, 0, 120));
			shaderProgram->setUniformValue("transform", modelViewProjectionMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

void InterimSelectionProp::setImageSize(const QSize& imageSize)
{
	if (!_fbo.isNull() && imageSize == _fbo->size())
		return;

	qDebug() << "Set image size to" << imageSize;

	const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(imageSize.width()), static_cast<float>(imageSize.height())));

	shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

	_fbo.reset(new QOpenGLFramebufferObject(imageSize.width(), imageSize.height()));

	QMatrix4x4 modelMatrix;

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 1.0f);

	setModelMatrix(modelMatrix);
}

void InterimSelectionProp::update()
{
	if (_fbo.isNull())
		return;

	renderer()->bindOpenGLContext();

	try {
		if (_fbo->bind()) {
			glViewport(0, 0, _fbo->width(), _fbo->height());

			QMatrix4x4 transform;

			auto width = _fbo->width();

			transform.ortho(0.0f, _fbo->width(), 0.0f, _fbo->height(), -1.0f, +1.0f);

			auto shape = shapeByName<QuadShape>("Quad");

			const auto offscreenBufferShaderProgram = shaderProgramByName("OffscreenBuffer");

			auto selectionPickerActor = dynamic_cast<SelectionPickerActor*>(actor());

			shape->vao().bind();
			{
				if (offscreenBufferShaderProgram->bind()) {
					glBindTexture(GL_TEXTURE_2D, _fbo->texture());

					offscreenBufferShaderProgram->setUniformValue("pixelSelectionTexture", 0);
					offscreenBufferShaderProgram->setUniformValue("transform", transform);
					offscreenBufferShaderProgram->setUniformValue("selectionType", static_cast<int>(selectionPickerActor->selectionType()));

					auto imageWidth = static_cast<float>(_fbo->size().width());
					auto imageHeight = static_cast<float>(_fbo->size().height());
					auto mouseEvents = selectionPickerActor->mouseEvents();

					offscreenBufferShaderProgram->setUniformValue("imageSize", imageWidth, imageHeight);

					const auto noMouseEvents = mouseEvents.size();

					switch (selectionPickerActor->selectionType())
					{
						case SelectionType::Rectangle:
						{
							if (noMouseEvents < 2)
								break;

							const auto firstPoint				= mouseEvents.first().screenPoint();
							const auto lastPoint				= mouseEvents.last().screenPoint();
							const auto rectangleTopLeft			= renderer()->screenPointToWorldPosition(modelViewMatrix(), QPointF(firstPoint.x(), firstPoint.y()));
							const auto rectangleBottomRight		= renderer()->screenPointToWorldPosition(modelViewMatrix(), QPointF(lastPoint.x(), lastPoint.y()));
							const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(_fbo->width()), rectangleTopLeft.y() / static_cast<float>(_fbo->height()));
							const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(_fbo->width()), rectangleBottomRight.y() / static_cast<float>(_fbo->height()));

							auto rectangle = QRectF(QPointF(rectangleTopLeftUV.x(), rectangleTopLeftUV.y()), QPointF(rectangleBottomRightUV.x(), rectangleBottomRightUV.y())).normalized();
							
							offscreenBufferShaderProgram->setUniformValue("rectangleTopLeft", rectangle.topLeft());
							offscreenBufferShaderProgram->setUniformValue("rectangleBottomRight", rectangle.bottomRight());
							break;
						}

						case SelectionType::Brush:
						{
							if (noMouseEvents == 0)
								break;
							
							if (noMouseEvents == 1) {
								const auto pMouseLast	= mouseEvents.last().screenPoint();
								const auto brushCenter	= renderer()->screenPointToWorldPosition(modelViewMatrix(), QPointF(pMouseLast.x(), pMouseLast.y()));
							}
							
							const auto previousBrushCenter	= mouseEvents.size() > 1 ? renderer()->screenPointToWorldPosition(modelViewMatrix(), QPointF(mouseEvents[mouseEvents.size() - 2].screenPoint().x(), mouseEvents[mouseEvents.size() - 2].screenPoint().y())) : brushCenter;

							offscreenBufferShaderProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
							offscreenBufferShaderProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
							offscreenBufferShaderProgram->setUniformValue("brushRadius", selectionPickerActor->brushRadius());
							
							break;
						}

						case SelectionType::Lasso:
						case SelectionType::Polygon:
						{
							/*
							QList<QVector2D> points;

							points.reserve(static_cast<std::int32_t>(_mousePositions.size()));

							for (const auto& mousePosition : _mousePositions) {
								points.push_back(QVector2D(mousePosition.x(), mousePosition.y()));
							}

							offscreenBufferShaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
							offscreenBufferShaderProgram->setUniformValue("noPoints", static_cast<int>(points.size()));
							*/
							break;
						}

						default:
							break;
					}

					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

					offscreenBufferShaderProgram->release();
				}
				else
				{
					throw std::exception("Unable to bind off screen shader program");
				}
			}

			shape->vao().release();

			_fbo->release();
		}
		else
		{
			throw std::exception("Unable to bind frame buffer object");
		}
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "update failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "update failed due to unhandled exception";
	}
}