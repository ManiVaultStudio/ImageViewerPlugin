#include "PointsProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "ImageRange.h"
#include "Renderer.h"
#include "Node.h"

#include <QDebug>

const std::string vertexShaderSource =
	#include "PointsPropVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "PointsPropFragment.glsl"
;

PointsProp::PointsProp(Node* node, const QString& name) :
	Prop(node, name),
	_channels()
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Quad", QOpenGLTexture::Target2D);

	initialize();
}

PointsProp::~PointsProp() = default;

void PointsProp::initialize()
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

void PointsProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
	try {
		if (!canRender())
			return;

		Prop::render(nodeMVP, opacity);

		const auto shape			= shapeByName<QuadShape>("Quad");
		const auto shaderProgram	= shaderProgramByName("Quad");
		const auto texture			= textureByName("Quad");

		texture->bind();

		if (shaderProgram->bind()) {
			//const auto displayRange = _channels.displayRange(Qt::EditRole).value<Range>();
			
			shaderProgram->setUniformValue("imageTexture", 0);
			//shaderProgram->setUniformValue("minPixelValue", displayRange.min());
			//shaderProgram->setUniformValue("maxPixelValue", displayRange.max());
			shaderProgram->setUniformValue("opacity", opacity);
			shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		texture->release();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

QRectF PointsProp::boundingRectangle() const
{
	return shapeByName<QuadShape>("Quad")->rectangle();
}

WindowLevelImage& PointsProp::channel(const int& id)
{
	return _channels[id];
}

const WindowLevelImage& PointsProp::channel(const int& id) const
{
	return _channels[id];
}

void PointsProp::setChannel(const int& id, const QImage& channel)
{
	//qDebug() << fullName() << "set image";

	renderer->bindOpenGLContext();
	{
		_channels[id].setImage(channel);

		auto texture = textureByName("Quad");

		texture->destroy();
		texture->create();
		texture->setSize(channel.size().width(), channel.size().height());
		texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
		texture->setWrapMode(QOpenGLTexture::ClampToEdge);
		texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
		texture->allocateStorage();
		texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, channel.bits());

		const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(channel.width()), static_cast<float>(channel.height())));

		shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

		updateModelMatrix();
	}
	Renderable::renderer->releaseOpenGLContext();
}

void PointsProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}