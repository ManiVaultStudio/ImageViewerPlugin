#include "ImageLayerProp.h"
#include "QuadShape.h"
#include "Actor.h"
#include "ImageLayer.h"

#include <QDebug>

const std::string vertexShaderSource =
#include "ImageLayerVertex.glsl"
;

const std::string fragmentShaderSource =
#include "ImageLayerFragment.glsl"
;

ImageLayerProp::ImageLayerProp(Actor* actor, const QString& name, ImageLayer* imageLayer) :
	Prop(actor, name),
	_imageLayer(imageLayer)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Quad", QOpenGLTexture::Target2D);

	setImage(imageLayer->image());

	QObject::connect(imageLayer, &ImageLayer::imageChanged, this, &ImageLayerProp::setImage);
	
	QObject::connect(imageLayer, &ImageLayer::opacityChanged, this, [this](const float& opacity) {
		emit becameDirty(this);
	});

	QObject::connect(imageLayer, &ImageLayer::displayRangeChanged, this, [this](const float& opacity) {
		emit becameDirty(this);
	});

	_actor->bindOpenGLContext();

	initialize();
}

ImageLayerProp::~ImageLayerProp() = default;

void ImageLayerProp::initialize()
{
	try
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
	catch (std::exception& e)
	{
		qDebug() << _name << "initialization failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "initialization failed due to unhandled exception";
	}
}

void ImageLayerProp::render()
{
	try {
		if (!canRender())
			return;

		Prop::render();

		const auto shape			= shapeByName<QuadShape>("Quad");
		const auto shaderProgram	= shaderProgramByName("Quad");
		const auto texture			= textureByName("Quad");

		texture->bind();

		if (shaderProgram->bind()) {
			shaderProgram->setUniformValue("imageTexture", 0);
			shaderProgram->setUniformValue("minPixelValue", _imageLayer->displayRange().first);
			shaderProgram->setUniformValue("maxPixelValue", _imageLayer->displayRange().second);
			shaderProgram->setUniformValue("opacity", _imageLayer->opacity());
			shaderProgram->setUniformValue("transform", modelViewProjectionMatrix());

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

void ImageLayerProp::setImage(const QImage& image)
{
	const auto texture = textureByName("Quad");

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

	QMatrix4x4 modelMatrix;

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);

	emit becameDirty(this);
}