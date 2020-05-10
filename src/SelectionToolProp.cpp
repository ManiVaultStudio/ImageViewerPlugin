#include "SelectionToolProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "Range.h"
#include "Renderer.h"
#include "SelectionLayer.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

const std::string vertexShaderSource =
	#include "SelectionToolVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "SelectionToolFragment.glsl"
;

SelectionToolProp::SelectionToolProp(SelectionLayer* selectionLayer, const QString& name) :
	Prop(reinterpret_cast<Node*>(selectionLayer), name)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Quad", QOpenGLTexture::Target2D);

	/*
	QObject::connect(selectionLayer, &SelectionLayer::imageChanged, [this](const QImage& selectionImage) {
		renderer->bindOpenGLContext();
		{
			if (selectionImage.isNull())
				return;

			setImage(selectionImage);
		}
		renderer->releaseOpenGLContext();
	});
	*/

	initialize();
}

SelectionToolProp::~SelectionToolProp() = default;

void SelectionToolProp::initialize()
{
	try
	{
		renderer->bindOpenGLContext();
		{
			Prop::initialize();

			const auto shaderProgram = shaderProgramByName("Quad");

			if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str()))
				throw std::exception("Unable to compile quad vertex shader");

			if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
				throw std::exception("Unable to compile quad fragment shader");

			if (!shaderProgram->link())
				throw std::exception("Unable to link quad shader program");

			const auto stride = 5 * sizeof(GLfloat);

			auto shape = shapeByName<QuadShape>("Quad");

			if (shaderProgram->bind()) {
				shape->vao().bind();
				shape->vbo().bind();

				shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
				shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
				shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
				shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
				shaderProgram->release();

				shape->vao().release();
				shape->vbo().release();
			}
			else {
				throw std::exception("Unable to bind quad shader program");
			}

			const auto texture = textureByName("Quad");

			texture->setWrapMode(QOpenGLTexture::Repeat);
			texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

			_initialized = true;
		}
		Renderable::renderer->releaseOpenGLContext();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "initialization failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "initialization failed due to unhandled exception";
	}
}

void SelectionToolProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
	try {
		if (!canRender())
			return;

		Prop::render(nodeMVP, opacity);

		const auto shape			= shapeByName<QuadShape>("Quad");
		const auto shaderProgram	= shaderProgramByName("Quad");
		const auto quadTexture		= textureByName("Quad");

		if (quadTexture->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
			quadTexture->bind();
		}

		if (shaderProgram->bind()) {
			auto selectionLayer = static_cast<SelectionLayer*>(_node);

			const auto overlayColor = selectionLayer->overlayColor(Qt::EditRole).value<QColor>();

			shaderProgram->setUniformValue("imageTexture", 0);
			shaderProgram->setUniformValue("overlayColor", overlayColor);
			shaderProgram->setUniformValue("opacity", opacity);
			shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		if (quadTexture->isCreated())
			quadTexture->release();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

QRectF SelectionToolProp::boundingRectangle() const
{
	return shapeByName<QuadShape>("Quad")->rectangle();
}

void SelectionToolProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}