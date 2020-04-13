#include "ImagesProp.h"
#include "QuadShape.h"
#include "LayersModel.h"

#include <QDebug>

const std::string vertexShaderSource =
#include "ImageLayerVertex.glsl"
;

const std::string fragmentShaderSource =
#include "ImageLayerFragment.glsl"
;

ImagesLayerProp::ImagesLayerProp(RenderNode* renderNode, const QString& name, const Layer::Type& type) :
	Prop(renderNode, name),
	_type(type),
	_image(),
	_displayRange{0.0f, 1000.0f},
	_opacity(0.0f)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Quad", QOpenGLTexture::Target2D);

//	_actor->bindOpenGLContext();

	initialize();
}

ImagesLayerProp::~ImagesLayerProp() = default;

void ImagesLayerProp::initialize()
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

void ImagesLayerProp::render()
{
	/*
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
			shaderProgram->setUniformValue("type", static_cast<int>(_type));
			shaderProgram->setUniformValue("minPixelValue", _displayRange[0]);
			shaderProgram->setUniformValue("maxPixelValue", _displayRange[1]);
			shaderProgram->setUniformValue("opacity", _opacity);
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
	*/
}

void ImagesLayerProp::setImage(const QImage& image)
{
	//qDebug() << fullName() << "set image";

	_image = image;
	
	auto texture = textureByName("Quad");

	switch (_image.format())
	{
		case QImage::Format::Format_RGB32:
		{
			texture->destroy();
			texture->create();
			texture->setSize(image.size().width(), image.size().height());
			texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
			texture->setWrapMode(QOpenGLTexture::ClampToEdge);
			texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
			texture->allocateStorage();
			texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt8, image.bits());

			break;
		}

		case QImage::Format::Format_RGBX64:
		case QImage::Format::Format_RGBA64:
		case QImage::Format::Format_RGBA64_Premultiplied:
		{
			texture->destroy();
			texture->create();
			texture->setSize(image.size().width(), image.size().height());
			texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
			texture->setWrapMode(QOpenGLTexture::ClampToEdge);
			texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
			texture->allocateStorage();
			texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image.bits());

			break;
		}

		default:
			break;
	}
	

	const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image.width()), static_cast<float>(image.height())));

	shapeByName<QuadShape>("Quad")->setRectangle(rectangle);
	
	updateModelMatrix();
}

void ImagesLayerProp::setDisplayRange(const float& min, const float& max)
{
	//qDebug() << fullName() << "set display range" << min << max;

	_displayRange[0] = min;
	_displayRange[1] = max;
}

void ImagesLayerProp::setOpacity(const float& opacity)
{
	//qDebug() << fullName() << "set opacity" << QString::number(opacity, 'f', 2);

	_opacity = opacity;
}

void ImagesLayerProp::setOrder(const std::uint32_t& order)
{
	//qDebug() << fullName() << "set order" << QString::number(order);

	_order = order;

	updateModelMatrix();
}

void ImagesLayerProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), static_cast<float>(_order));

	setModelMatrix(modelMatrix);
}