#include "ImagesProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "Range.h"
#include "Renderer.h"
#include "Node.h"
#include "ImagesLayer.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

const std::string vertexShaderSource =
	#include "ImagesPropVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "ImagesPropFragment.glsl"
;

ImagesProp::ImagesProp(ImagesLayer* imagesLayer, const QString& name) :
	Prop(reinterpret_cast<Node*>(imagesLayer), name)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Channels", QOpenGLTexture::Target2DArray);

	QObject::connect(imagesLayer, &ImagesLayer::channelChanged, [this, imagesLayer](const std::uint32_t& channelId) {
		renderer->bindOpenGLContext();
		{
			if (channelId == 0) {
				auto channel = imagesLayer->channel(channelId);

				const auto imageSize = channel->imageSize();

				if (!imageSize.isValid())
					return;

				auto texture = textureByName("Channels");

				if (!texture->isCreated())
					texture->create();

				if (imageSize != QSize(texture->width(), texture->height())) {
					texture->destroy();
					texture->create();
					texture->setLayers(3);
					texture->setSize(imageSize.width(), imageSize.height(), 1);
					texture->setSize(imageSize.width(), imageSize.height(), 2);
					texture->setSize(imageSize.width(), imageSize.height(), 3);
					texture->setFormat(QOpenGLTexture::R32F);
					texture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float32);
					texture->setWrapMode(QOpenGLTexture::ClampToEdge);
					texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
				}

				texture->setData(0, channel->id(), QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::Float32, channel->elements().data());

				const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(imageSize.width()), static_cast<float>(imageSize.height())));

				this->shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

				updateModelMatrix();
			}
			
		}
		renderer->releaseOpenGLContext();
	});

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

void ImagesProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
	try {
		if (!canRender())
			return;

		Prop::render(nodeMVP, opacity);

		const auto shape = shapeByName<QuadShape>("Quad");
		const auto shaderProgram = shaderProgramByName("Quad");

		/*
		if (textureByName("ColorMap")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
			textureByName("ColorMap")->bind();
		}
		*/

		if (textureByName("Channels")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE1);
			textureByName("Channels")->bind();
		}

		auto imagesLayer = static_cast<ImagesLayer*>(_node);

		if (shaderProgram->bind()) {
			const QVector2D displayRanges[] = {
				imagesLayer->channel(0)->displayRangeVector(),
				imagesLayer->channel(1)->displayRangeVector(),
				imagesLayer->channel(2)->displayRangeVector()
			};

			//shaderProgram->setUniformValue("colorMapTexture", 0);
			shaderProgram->setUniformValue("channelTextures", 1);
			shaderProgram->setUniformValueArray("displayRanges", displayRanges, 3);
			shaderProgram->setUniformValue("opacity", opacity);
			shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		if (textureByName("Channels")->isCreated())
			textureByName("Channels")->release();

		/*
		if (textureByName("ColorMap")->isCreated())
			textureByName("ColorMap")->release();
		*/
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

QRectF ImagesProp::boundingRectangle() const
{
	auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	rectangle.setSize(_node->scale(Qt::EditRole).toFloat() * rectangle.size());

	return rectangle;
}

void ImagesProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}