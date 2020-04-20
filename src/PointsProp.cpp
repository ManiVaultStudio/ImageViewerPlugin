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

const std::string vertexShaderSource =
	#include "PointsPropVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "PointsPropFragment.glsl"
;

const GLuint PointsProp::channels[] = {
	static_cast<GLuint>(TextureId::ColorMap),
	static_cast<GLuint>(TextureId::Channel1),
	static_cast<GLuint>(TextureId::Channel2),
	static_cast<GLuint>(TextureId::Channel3)
};

PointsProp::PointsProp(PointsLayer* pointsLayer, const QString& name) :
	Prop(reinterpret_cast<Node*>(pointsLayer), name),
	_noChannels(0),
	_channels()
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");

	addTexture("ColorMap", QOpenGLTexture::Target2D);
	addTexture("Channels", QOpenGLTexture::Target2DArray);

	

	_channels << pointsLayer->channel(0) << pointsLayer->channel(1) << pointsLayer->channel(2) << pointsLayer->channel(3);

	for (auto channel : _channels)
	{
		QObject::connect(channel, &Channel::changed, [this](Channel* channel) {
			renderer->bindOpenGLContext();
			{
				
				const auto imageSize = channel->imageSize();

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
				
				texture->setData(0, channel->id(), QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::Float32, channel->elements().constData());
				
				const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(imageSize.width()), static_cast<float>(imageSize.height())));

				this->shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

				updateModelMatrix();
				/**/
			}
			renderer->releaseOpenGLContext();
		});
	}

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

		if (shaderProgram->bind()) {
			const QVector2D displayRanges[] = {
				_channels.at(0)->displayRangeVector(),
				_channels.at(1)->displayRangeVector(),
				_channels.at(2)->displayRangeVector(),
				_channels.at(3)->displayRangeVector()
			};

			auto pointsLayer = static_cast<PointsLayer*>(_node);

			const auto noChannels	= pointsLayer->noChannels(Qt::EditRole).toInt();
			const auto invertAlpha	= pointsLayer->channel(3)->inverted();

			shaderProgram->setUniformValue("colorMapTexture", 0);
			shaderProgram->setUniformValue("channelTextures", 1);
			shaderProgram->setUniformValue("noChannels", noChannels);
			shaderProgram->setUniformValue("invertAlpha", invertAlpha);
			shaderProgram->setUniformValueArray("displayRanges", displayRanges, 4);
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
	return shapeByName<QuadShape>("Quad")->rectangle();
}

void PointsProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}