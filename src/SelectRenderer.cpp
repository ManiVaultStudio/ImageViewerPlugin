#include "SelectRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

SelectRenderer::SelectRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_texture(),
	_fbo(),
	_color(1.f, 0.6f, 0.f, 0.3f),
	_brushRadius(50.f),
	_brushRadiusDelta(2.0f),
	_pixelSelectionProgram(std::make_unique<QOpenGLShaderProgram>())
{
}

void SelectRenderer::render()
{
	if (!initialized())
		return;

	if (_program->bind()) {
		_program->setUniformValue("overlayTexture", 0);
		_program->setUniformValue("transform", _modelViewProjection);
		_program->setUniformValue("color", _color);

		QuadRenderer::render();

		/*
		_vao.bind();
		{
			//glBindTexture(GL_TEXTURE_2D, _fbo->texture());
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		_vao.release();
		*/

		_program->release();
	}
}

void SelectRenderer::initializePrograms()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, overlayVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, overlayFragmentShaderSource.c_str());
	_program->link();

	/*
	_pixelSelectionProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, pixelSelectionVertexShaderSource.c_str());
	_pixelSelectionProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, pixelSelectionFragmentShaderSource.c_str());
	_pixelSelectionProgram->link();
	*/
}

void SelectRenderer::setImageSize(const QSize& size)
{
	auto createFBO = false;

	if (_fbo.get() == nullptr) {
		createFBO = true;
	}
	else {
		if (size != _fbo->size()) {
			createFBO = true;
		}
	}

	setSize(size);

	if (createFBO) {
		_fbo = std::make_unique<QOpenGLFramebufferObject>(size.width(), size.height());
	}
}

void SelectRenderer::updatePixelSelection(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions)
{
	return;

	if (!_fbo->bind())
		return;
	
	glViewport(0, 0, _fbo->width(), _fbo->height());

	QMatrix4x4 transform;

	transform.ortho(0.0f, _fbo->width(), 0.0f, _fbo->height(), -1.0f, +1.0f);

	_vao.bind();
	{
		if (_pixelSelectionProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, _fbo->texture());

			_pixelSelectionProgram->setUniformValue("pixelSelectionTexture", 0);
			_pixelSelectionProgram->setUniformValue("matrix", transform);
			_pixelSelectionProgram->setUniformValue("selectionType", static_cast<int>(selectionType));
			_pixelSelectionProgram->setUniformValue("imageSize", static_cast<float>(_fbo->size().width()), static_cast<float>(_fbo->size().height()));

			switch (selectionType)
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft			= mousePositions.front();
					const auto rectangleBottomRight		= mousePositions.back();
					const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(_fbo->width()), rectangleTopLeft.y() / static_cast<float>(_fbo->height()));
					const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(_fbo->width()), rectangleBottomRight.y() / static_cast<float>(_fbo->height()));

					auto rectangleUV	= std::make_pair(rectangleTopLeftUV, rectangleBottomRightUV);
					auto topLeft		= QVector2D(rectangleTopLeftUV.x(), rectangleTopLeftUV.y());
					auto bottomRight	= QVector2D(rectangleBottomRightUV.x(), rectangleBottomRightUV.y());

					if (rectangleBottomRightUV.x() < rectangleTopLeftUV.x()) {
						topLeft.setX(rectangleBottomRightUV.x());
						bottomRight.setX(rectangleTopLeftUV.x());
					}

					if (rectangleBottomRightUV.y() < rectangleTopLeftUV.y()) {
						topLeft.setY(rectangleBottomRightUV.y());
						bottomRight.setY(rectangleTopLeftUV.y());
					}

					_pixelSelectionProgram->setUniformValue("rectangleTopLeft", topLeft);
					_pixelSelectionProgram->setUniformValue("rectangleBottomRight", bottomRight);

					break;
				}

				case SelectionType::Brush:
				{
					const auto brushCenter			= mousePositions[mousePositions.size() - 1];
					const auto previousBrushCenter	= mousePositions.size() > 1 ? mousePositions[mousePositions.size() - 2] : brushCenter;

					_pixelSelectionProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
					_pixelSelectionProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
					_pixelSelectionProgram->setUniformValue("brushRadius", _brushRadius);

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					QList<QVector2D> mousePositions;

					mousePositions.reserve(static_cast<std::int32_t>(mousePositions.size()));

					for (const auto& mousePosition : mousePositions) {
						mousePositions.push_back(QVector2D(mousePosition.x(), mousePosition.y()));
					}

					_pixelSelectionProgram->setUniformValueArray("points", &mousePositions[0], static_cast<std::int32_t>(mousePositions.size()));
					_pixelSelectionProgram->setUniformValue("noPoints", static_cast<int>(mousePositions.size()));

					break;
				}

				default:
					break;
			}

			_pixelSelectionProgram->enableAttributeArray(0);
			_pixelSelectionProgram->enableAttributeArray(1);
			_pixelSelectionProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
			_pixelSelectionProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_pixelSelectionProgram->release();
		}
	}
	_vao.release();

	_fbo->release();
}

void SelectRenderer::resetPixelSelection()
{
	/*
	makeCurrent();

	if (_pixelSelectionFBO->bind()) {
		glViewport(0, 0, _displayImage->width(), _displayImage->height());
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	doneCurrent();

	update();
	*/
}

float SelectRenderer::brushRadius() const
{
	return _brushRadius;
}

void SelectRenderer::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 10000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius" << brushRadius;
}

float SelectRenderer::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void SelectRenderer::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;
}

void SelectRenderer::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void SelectRenderer::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

bool SelectRenderer::initialized() const
{
	if (_fbo.get() == nullptr)
		return false;

	return _fbo->isValid();
}