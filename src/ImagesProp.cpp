#include "ImagesProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "ImageRange.h"
#include "Renderer.h"
#include "Node.h"

#include <QDebug>

const std::string vertexShaderSource =
	#include "ImageLayerVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "ImageLayerFragment.glsl"
;

ImagesProp::ImagesProp(Node* node, const QString& name) :
	Prop(node, name),
	_windowLevelImage(),
	_opacity(1.0f),
	_order(0)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Quad", QOpenGLTexture::Target2D);

	initialize();
}

ImagesProp::~ImagesProp() = default;

void ImagesProp::initialize()
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

void ImagesProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
	try {
		if (!canRender())
			return;

		renderer->bindOpenGLContext();
		{
			Prop::render(nodeMVP, opacity);

			const auto shape			= shapeByName<QuadShape>("Quad");
			const auto shaderProgram	= shaderProgramByName("Quad");
			const auto texture			= textureByName("Quad");

			texture->bind();

			if (shaderProgram->bind()) {
				const auto displayRange = _windowLevelImage.displayRange(Qt::EditRole).value<Range>();
			
				//qDebug() << _node->modelViewProjectionMatrix();

				shaderProgram->setUniformValue("imageTexture", 0);
				shaderProgram->setUniformValue("minPixelValue", displayRange.min());
				shaderProgram->setUniformValue("maxPixelValue", displayRange.max());
				shaderProgram->setUniformValue("opacity", _opacity);
				shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

			
				shape->render();

				shaderProgram->release();
			}
			else {
				throw std::exception("Unable to bind quad shader program");
			}

			texture->release();
		}
		Renderable::renderer->releaseOpenGLContext();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

WindowLevelImage& ImagesProp::image()
{
	return _windowLevelImage;
}

const WindowLevelImage& ImagesProp::image() const
{
	return _windowLevelImage;
}

void ImagesProp::setImage(const QImage& image)
{
	//qDebug() << fullName() << "set image";

	renderer->bindOpenGLContext();
	{
		_windowLevelImage.setImage(image);

		auto texture = textureByName("Quad");

		texture->destroy();
		texture->create();
		texture->setSize(image.size().width(), image.size().height());
		texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
		texture->setWrapMode(QOpenGLTexture::ClampToEdge);
		texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
		texture->allocateStorage();
		texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image.bits());

		const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image.width()), static_cast<float>(image.height())));

		shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

		updateModelMatrix();
	}
	Renderable::renderer->releaseOpenGLContext();
}

void ImagesProp::setOpacity(const float& opacity)
{
	//qDebug() << fullName() << "set opacity" << QString::number(opacity, 'f', 2);

	_opacity = opacity;
}

void ImagesProp::setOrder(const std::uint32_t& order)
{
	//qDebug() << fullName() << "set order" << QString::number(order);

	_order = order;

	updateModelMatrix();
}

void ImagesProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), static_cast<float>(_order));

	setModelMatrix(modelMatrix);
}