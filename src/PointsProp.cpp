#include "PointsProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "Range.h"
#include "Renderer.h"
#include "Node.h"
#include "PointsLayer.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <stdexcept> // For runtime_error.

const std::string vertexShaderSource =
	#include "PointsVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "PointsFragment.glsl"
;

const GLuint PointsProp::channels[] = {
	static_cast<GLuint>(TextureId::ColorMap),
	static_cast<GLuint>(TextureId::Channel1),
	static_cast<GLuint>(TextureId::Channel2),
	static_cast<GLuint>(TextureId::Channel3)
};

PointsProp::PointsProp(PointsLayer* pointsLayer, const QString& name) :
	Prop(reinterpret_cast<Node*>(pointsLayer), name),
	_noChannels(0)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");

	addTexture("ColorMap", QOpenGLTexture::Target2D);
	addTexture("Channels", QOpenGLTexture::Target2DArray);

	QObject::connect(pointsLayer, &PointsLayer::channelChanged, [this, pointsLayer](const std::uint32_t& channelId) {
		renderer->bindOpenGLContext();
		{
			auto channel = pointsLayer->channel(channelId);

			const auto imageSize = channel->getImageSize();

			if (!imageSize.isValid())
				return;

			auto texture = textureByName("Channels");

			if (!texture->isCreated())
				texture->create();

			if (imageSize != QSize(texture->width(), texture->height())) {
				texture->destroy();
				texture->create();
				texture->setLayers(4);
				texture->setSize(imageSize.width(), imageSize.height(), 1);
				texture->setSize(imageSize.width(), imageSize.height(), 2);
				texture->setSize(imageSize.width(), imageSize.height(), 3);
				texture->setSize(imageSize.width(), imageSize.height(), 4);
				texture->setFormat(QOpenGLTexture::R32F);
				texture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float32);
				texture->setWrapMode(QOpenGLTexture::ClampToEdge);
				texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
			}

			texture->setData(0, channel->getId(), QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::Float32, channel->getElements().data());

			const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(imageSize.width()), static_cast<float>(imageSize.height())));

			this->shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

			updateModelMatrix();
		}
		renderer->releaseOpenGLContext();
	});
	
	QObject::connect(pointsLayer, &PointsLayer::colorMapChanged, [this](const QImage& colorMap) {
		renderer->bindOpenGLContext();
		{
			if (colorMap.isNull())
				return;

			auto& texture = textureByName("ColorMap");
			
			texture.reset(new QOpenGLTexture(colorMap));

			if (!texture->isCreated())
				texture->create();

			texture->setWrapMode(QOpenGLTexture::ClampToEdge);
		}
		renderer->releaseOpenGLContext();
	});

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
				throw std::runtime_error("Unable to compile quad vertex shader");

			if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
				throw std::runtime_error("Unable to compile quad fragment shader");

			if (!shaderProgram->link())
				throw std::runtime_error("Unable to link quad shader program");

			const auto stride = 5 * sizeof(GLfloat);

			auto shape = shapeByName<QuadShape>("Quad");

			if (shaderProgram->bind()) {
				shape->getVAO().bind();
				shape->getVBO().bind();

				shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
				shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
				shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
				shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
				shaderProgram->release();

				shape->getVAO().release();
				shape->getVBO().release();
			}
			else {
				throw std::runtime_error("Unable to bind quad shader program");
			}

			_initialized = true;
		}
		renderer->releaseOpenGLContext();
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

		if (textureByName("ColorMap")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
			textureByName("ColorMap")->bind();
		}

		if (textureByName("Channels")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE1);
			textureByName("Channels")->bind();
		}

		auto pointsLayer = static_cast<PointsLayer*>(_node);
		
		if (shaderProgram->bind()) {
			const QVector2D displayRanges[] = {
				pointsLayer->channel(0)->getDisplayRangeVector(),
				pointsLayer->channel(1)->getDisplayRangeVector(),
				pointsLayer->channel(2)->getDisplayRangeVector()
			};

			const auto noChannels		= pointsLayer->noChannels(Qt::EditRole).toInt();
			const auto useConstantColor	= pointsLayer->useConstantColor(Qt::EditRole).toBool();
			const auto constantColor	= pointsLayer->constantColor(Qt::EditRole).value<QColor>();
			const auto colorSpace		= pointsLayer->colorSpace(Qt::EditRole).toInt();
			const auto pointType		= pointsLayer->pointType(Qt::EditRole).toInt();

			shaderProgram->setUniformValue("colorMapTexture", 0);
			shaderProgram->setUniformValue("channelTextures", 1);
			shaderProgram->setUniformValue("noChannels", noChannels);
			shaderProgram->setUniformValue("useConstantColor", useConstantColor);
			shaderProgram->setUniformValue("constantColor", constantColor);
			shaderProgram->setUniformValue("colorSpace", colorSpace);
			shaderProgram->setUniformValue("pointType", pointType);
			shaderProgram->setUniformValueArray("displayRanges", displayRanges, 3);
			shaderProgram->setUniformValue("opacity", opacity);
			shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::runtime_error("Unable to bind quad shader program");
		}

		if (textureByName("Channels")->isCreated())
			textureByName("Channels")->release();

		if (textureByName("ColorMap")->isCreated())
			textureByName("ColorMap")->release();
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
	auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	rectangle.setSize(_node->scale(Qt::EditRole).toFloat() * rectangle.size());

	return rectangle;
}

void PointsProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}