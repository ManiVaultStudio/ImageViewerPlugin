#include "ColorImageProp.h"
#include "QuadShape.h"
#include "Actor.h"

#include <QDebug>

const std::string vertexShaderSource =
#include "ImageQuadVertex.glsl"
;

const std::string fragmentShaderSource =
#include "ImageQuadFragment.glsl"
;

ColorImageProp::ColorImageProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_minPixelValue(),
	_maxPixelValue()
{
	addShape<QuadShape>("QuadShape");
	addShaderProgram("QuadShape");
	addTexture("QuadShape", QOpenGLTexture::Target2D);

	connect(shapeByName<QuadShape>("QuadShape"), &QuadShape::rectangleChanged, this, [&](const QRectF& rectangle) {
		_matrix.setColumn(3, QVector4D(-0.5f * rectangle.width(), -0.5f * rectangle.height(), _matrix.column(3).z(), 1.f));

		emit imageSizeChanged(imageSize());
	});
}

void ColorImageProp::setImage(std::shared_ptr<QImage> image)
{
	const auto texture = textureByName("QuadShape");

	texture->destroy();
	texture->create();
	texture->setSize(image->size().width(), image->size().height());
	texture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	texture->setWrapMode(QOpenGLTexture::ClampToEdge);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	texture->allocateStorage();
	texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image->bits());

	shapeByName<QuadShape>("QuadShape")->setRectangle(QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	emit changed(this);
}

QSize ColorImageProp::imageSize() const
{
	if (!_initialized)
		return QSize();

	const auto quadRectangle = shapeByName<QuadShape>("QuadShape")->rectangle();

	return QSize(static_cast<int>(quadRectangle.width()), static_cast<int>(quadRectangle.height()));
}

void ColorImageProp::setMinPixelValue(const float& minPixelValue)
{
	if (minPixelValue == _minPixelValue)
		return;

	_minPixelValue = minPixelValue;
}

void ColorImageProp::setMaxPixelValue(const float& maxPixelValue)
{
	if (maxPixelValue == _maxPixelValue)
		return;

	_maxPixelValue = maxPixelValue;
}

void ColorImageProp::initialize()
{
	try
	{
		Prop::initialize();

		const auto shaderProgram = shaderProgramByName("QuadShape");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str()))
			throw std::exception("Unable to compile quad vertex shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
			throw std::exception("Unable to compile quad fragment shader");

		if (!shaderProgram->link())
			throw std::exception("Unable to link quad shader program");

		const auto stride = 5 * sizeof(GLfloat);

		auto shape = shapeByName<QuadShape>("QuadShape");

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

		const auto texture = textureByName("QuadShape");

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

void ColorImageProp::render()
{
	if (!canRender())
		return;

	Prop::render();

	const auto shape = shapeByName<QuadShape>("QuadShape");
	const auto shaderProgram = shaderProgramByName("QuadShape");
	const auto texture = textureByName("QuadShape");

	texture->bind();

	if (shaderProgram->bind()) {
		shaderProgram->setUniformValue("imageTexture", 0);
		shaderProgram->setUniformValue("minPixelValue", _minPixelValue);
		shaderProgram->setUniformValue("maxPixelValue", _maxPixelValue);
		shaderProgram->setUniformValue("transform", actor()->modelViewProjectionMatrix() * _matrix);

		shape->render();

		shaderProgram->release();
	}

	texture->release();
}