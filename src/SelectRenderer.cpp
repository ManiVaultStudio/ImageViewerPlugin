#include "SelectRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

SelectRenderer::SelectRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_pixelSelectionFBO(),
	_pixelSelectionColor(1.f, 0.6f, 0.f, 0.3f)
{
}

void SelectRenderer::init()
{
	initializeOpenGLFunctions();
	initializeProgram();

	_vbo.create();
	_vbo.bind();
	_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_vbo.release();

	_vao.create();

	_program->bind();
	_vao.bind();
	_vbo.bind();

	_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3);

	_vao.release();
	_vbo.release();
	_program->release();
}

void SelectRenderer::resize(QSize renderSize)
{
}

void SelectRenderer::render()
{
	if (!initialized())
		return;

	_program->bind();
	{
		/*
		_program->setUniformValue("overlayTexture", 0);
		_program->setUniformValue("transform", _modelViewProjection);
		_program->setUniformValue("color", _pixelSelectionColor);
		*/

		//QuadRenderer::render();
	}
	_program->release();
}

void SelectRenderer::destroy()
{
	//_texture->destroy();
	_vbo.destroy();
	_vao.destroy();
}

void SelectRenderer::initializeProgram()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, overlayVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, overlayFragmentShaderSource.c_str());
	_program->link();
}

void SelectRenderer::setSize(const QSize& size)
{
	auto createFBO = false;

	if (_pixelSelectionFBO.get() == nullptr) {
		createFBO = true;
	}
	else {
		if (size != _pixelSelectionFBO->size()) {
			createFBO = true;
		}
	}

	if (createFBO)
		_pixelSelectionFBO = std::make_unique<QOpenGLFramebufferObject>(size.width(), size.height());
}

void SelectRenderer::updatePixelSelection()
{
	/*
	//qDebug() << "Update selection" << selectionTypeName(_selectionType);

	makeCurrent();

	if (!_pixelSelectionFBO->bind())
		return;

	glViewport(0, 0, _displayImage->width(), _displayImage->height());

	QMatrix4x4 transform;

	transform.ortho(0.0f, _displayImage->width(), 0.0f, _displayImage->height(), -1.0f, +1.0f);


	if (_imageQuadVAO.bind()) {
		if (_pixelSelectionShaderProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, _pixelSelectionFBO->texture());

			_pixelSelectionShaderProgram->setUniformValue("pixelSelectionTexture", 0);
			_pixelSelectionShaderProgram->setUniformValue("matrix", transform);
			_pixelSelectionShaderProgram->setUniformValue("selectionType", static_cast<int>(_selectionType));
			_pixelSelectionShaderProgram->setUniformValue("imageSize", static_cast<float>(_displayImage->size().width()), static_cast<float>(_displayImage->size().height()));

			switch (_selectionType)
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft			= screenToWorld(_mousePositions.front());
					const auto rectangleBottomRight		= screenToWorld(_mousePositions.back());
					const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(_displayImage->width()), rectangleTopLeft.y() / static_cast<float>(_displayImage->height()));
					const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(_displayImage->width()), rectangleBottomRight.y() / static_cast<float>(_displayImage->height()));

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

					_pixelSelectionShaderProgram->setUniformValue("rectangleTopLeft", topLeft);
					_pixelSelectionShaderProgram->setUniformValue("rectangleBottomRight", bottomRight);

					break;
				}

				case SelectionType::Brush:
				{
					const auto currentMousePosition		= _mousePositions[_mousePositions.size() - 1];
					const auto previousMousePosition	= _mousePositions.size() > 1 ? _mousePositions[_mousePositions.size() - 2] : currentMousePosition;
					const auto brushCenter				= screenToWorld(currentMousePosition);
					const auto previousBrushCenter		= screenToWorld(previousMousePosition);

					_pixelSelectionShaderProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
					_pixelSelectionShaderProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
					_pixelSelectionShaderProgram->setUniformValue("brushRadius", _brushRadius);

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					QList<QVector2D> mousePositions;

					mousePositions.reserve(static_cast<std::int32_t>(_mousePositions.size()));

					for (const auto p : _mousePositions) {
						mousePositions.push_back(QVector2D(screenToWorld(p).x(), screenToWorld(p).y()));
					}

					_pixelSelectionShaderProgram->setUniformValueArray("points", &mousePositions[0], static_cast<std::int32_t>(_mousePositions.size()));
					_pixelSelectionShaderProgram->setUniformValue("noPoints", static_cast<int>(_mousePositions.size()));

					break;
				}

				default:
					break;
			}

			_pixelSelectionShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
			_pixelSelectionShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
			_pixelSelectionShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
			_pixelSelectionShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_pixelSelectionShaderProgram->release();
		}

		_imageQuadVAO.release();
	}

	_pixelSelectionFBO->release();

	doneCurrent();

	update();
	*/
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