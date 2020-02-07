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

		qDebug() << "Render" << name();

		const auto shape			= shapeByName<QuadShape>("Quad");
		const auto shaderProgram	= shaderProgramByName("Quad");

		if (shaderProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, _fbo->texture());

			shaderProgram->setUniformValue("offscreenBufferTexture", 0);
			shaderProgram->setUniformValue("color", renderer()->colorByName("InterimSelectionOverlayColor"));
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

	qDebug() << "Update" << _name;

	try {
		renderer()->bindOpenGLContext();

		if (_fbo->bind()) {
			glViewport(0, 0, _fbo->width(), _fbo->height());

			QMatrix4x4 transform;

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

					const auto fboSize			= QSizeF(static_cast<float>(_fbo->size().width()), static_cast<float>(_fbo->size().height()));
					const auto mouseEvents		= selectionPickerActor->mouseEvents();
					const auto noMouseEvents	= mouseEvents.size();

					offscreenBufferShaderProgram->setUniformValue("imageSize", fboSize.width(), fboSize.height());

					switch (selectionPickerActor->selectionType())
					{
						case SelectionType::Rectangle:
						{
							if (noMouseEvents < 2)
								break;

							const auto pFirst					= mouseEvents.first().screenPoint();
							const auto pLast					= mouseEvents.last().screenPoint();
							const auto pRectangleTopLeft		= renderer()->screenPointToWorldPosition(modelViewMatrix(), pFirst);
							const auto pRectangleBottomRight	= renderer()->screenPointToWorldPosition(modelViewMatrix(), pLast);
							const auto pRectangleTopLeftUV		= QVector2D(pRectangleTopLeft.x() / fboSize.width(), pRectangleTopLeft.y() / fboSize.height());
							const auto pRectangleBottomRightUV	= QVector2D(pRectangleBottomRight.x() / fboSize.width(), pRectangleBottomRight.y() / fboSize.height());
							const auto rectangle				= QRectF(QPointF(pRectangleTopLeftUV.x(), pRectangleTopLeftUV.y()), QPointF(pRectangleBottomRightUV.x(), pRectangleBottomRightUV.y())).normalized();
							
							offscreenBufferShaderProgram->setUniformValue("rectangleTopLeft", rectangle.topLeft());
							offscreenBufferShaderProgram->setUniformValue("rectangleBottomRight", rectangle.bottomRight());
							break;
						}

						case SelectionType::Brush:
						{
							if (noMouseEvents == 0)
								break;
							
							offscreenBufferShaderProgram->setUniformValue("brushRadius", selectionPickerActor->brushRadius());

							if (noMouseEvents == 1) {
								const auto pMouseLast	= mouseEvents.last().screenPoint();
								const auto pBrush		= renderer()->screenPointToWorldPosition(modelViewMatrix(), pMouseLast).toVector2D();

								offscreenBufferShaderProgram->setUniformValue("previousBrushCenter", pBrush);
								offscreenBufferShaderProgram->setUniformValue("currentBrushCenter", pBrush);
							}

							if (noMouseEvents > 1) {
								const auto pMousePrevious	= mouseEvents[noMouseEvents - 2].screenPoint();
								const auto pMouseCurrent	= mouseEvents.last().screenPoint();
								const auto pBrushPrevious	= renderer()->screenPointToWorldPosition(modelViewMatrix(), pMousePrevious).toVector2D();
								const auto pBrushCurrent	= renderer()->screenPointToWorldPosition(modelViewMatrix(), pMouseCurrent).toVector2D();

								offscreenBufferShaderProgram->setUniformValue("previousBrushCenter", pBrushPrevious);
								offscreenBufferShaderProgram->setUniformValue("currentBrushCenter", pBrushCurrent);
							}
							break;
						}

						case SelectionType::Lasso:
						case SelectionType::Polygon:
						{
							if (noMouseEvents < 2)
								break;

							QList<QVector2D> points;

							points.reserve(static_cast<std::int32_t>(noMouseEvents));

							for (const auto& mouseEvent : mouseEvents) {
								points.push_back(renderer()->screenPointToWorldPosition(modelViewMatrix(), mouseEvent.screenPoint()).toVector2D());
							}

							//qDebug() << points;

							offscreenBufferShaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
							offscreenBufferShaderProgram->setUniformValue("noPoints", static_cast<int>(points.size()));
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

void InterimSelectionProp::reset()
{
	qDebug() << "Reset" << _name;

	try {
		renderer()->bindOpenGLContext();

		if (_fbo->bind()) {
			glViewport(0, 0, _fbo->width(), _fbo->height());
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();

			_fbo->release();
		}
		else
		{
			throw std::exception("Unable to bind frame buffer object");
		}
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "reset failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "reset failed due to unhandled exception";
	}
}