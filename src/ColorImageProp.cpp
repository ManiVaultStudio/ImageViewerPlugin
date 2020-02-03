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

	connect(shape<QuadShape>("QuadShape"), &QuadShape::rectangleChanged, this, [&](const QRectF& rectangle) {
		_matrix.setColumn(3, QVector4D(-0.5f * rectangle.width(), -0.5f * rectangle.height(), _matrix.column(3).z(), 1.f));

		emit imageSizeChanged(imageSize());
	});
}

void ColorImageProp::setImage(std::shared_ptr<QImage> image)
{
	const auto quadShapeTexture = _textures["QuadShape"];

	quadShapeTexture->destroy();
	quadShapeTexture->create();
	quadShapeTexture->setSize(image->size().width(), image->size().height());
	quadShapeTexture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	quadShapeTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
	quadShapeTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	quadShapeTexture->allocateStorage();
	quadShapeTexture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, image->bits());

	shape<QuadShape>("QuadShape")->setRectangle(QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	emit changed(this);
}

QSize ColorImageProp::imageSize() const
{
	if (!_initialized)
		return QSize();
	
	const auto quadRectangle = dynamic_cast<QuadShape*>(_shapes["QuadShape"].get())->rectangle();

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
	Prop::initialize();

	const auto quadShapeShaderProgram = _shaderPrograms["QuadShape"];

	quadShapeShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	quadShapeShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!quadShapeShaderProgram->link()) {
		throw std::exception("Unable to link color image quad shader program");
	}

	const auto stride = 5 * sizeof(GLfloat);

	auto quadShape = shape<QuadShape>("QuadShape");

	if (quadShapeShaderProgram->bind()) {
		quadShape->vao().bind();
		quadShape->vbo().bind();

		quadShapeShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
		quadShapeShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
		quadShapeShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		quadShapeShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
		quadShapeShaderProgram->release();

		quadShape->vao().release();
		quadShape->vbo().release();
	}
	else {
		throw std::exception("Unable to bind color image quad shader program");
	}

	const auto quadShapeTexture = _textures["QuadShape"];

	quadShapeTexture->setWrapMode(QOpenGLTexture::Repeat);
	quadShapeTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_initialized = true;
}

void ColorImageProp::render()
{
	if (!canRender())
		return;

	Prop::render();

	const auto quadShape				= _shapes["QuadShape"];
	const auto quadShapeShaderProgram	= _shaderPrograms["QuadShape"];
	const auto quadShapeTexture			= _textures["QuadShape"];

	quadShapeTexture->bind();

	if (quadShapeShaderProgram->bind()) {
		quadShapeShaderProgram->setUniformValue("imageTexture", 0);
		quadShapeShaderProgram->setUniformValue("minPixelValue", _minPixelValue);
		quadShapeShaderProgram->setUniformValue("maxPixelValue", _maxPixelValue);
		quadShapeShaderProgram->setUniformValue("transform", actor()->modelViewProjectionMatrix() * _matrix);

		quadShape->render();

		quadShapeShaderProgram->release();
	}

	quadShapeTexture->release();
}