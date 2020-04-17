#include "PointsProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "ImageRange.h"
#include "Renderer.h"
#include "Node.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

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
	addTexture("ColorMap", QOpenGLTexture::Target2D);
	addTexture("Channel0", QOpenGLTexture::Target2D);
	addTexture("Channel1", QOpenGLTexture::Target2D);
	addTexture("Channel2", QOpenGLTexture::Target2D);

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

			textureByName("Channel0")->setWrapMode(QOpenGLTexture::Repeat);
			textureByName("Channel0")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

			textureByName("Channel1")->setWrapMode(QOpenGLTexture::Repeat);
			textureByName("Channel1")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

			textureByName("Channel2")->setWrapMode(QOpenGLTexture::Repeat);
			textureByName("Channel2")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

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

		if (textureByName("ColorMap")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
			textureByName("ColorMap")->bind();
		}

		if (textureByName("Channel0")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE1);
			textureByName("Channel0")->bind();
		}
			
		if (textureByName("Channel1")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE2);
			textureByName("Channel1")->bind();
		}

		if (textureByName("Channel2")->isCreated()) {
			renderer->openGLContext()->functions()->glActiveTexture(GL_TEXTURE3);
			textureByName("Channel2")->bind();
		}

		if (shaderProgram->bind()) {
			const GLuint channels[] = { 1, 2, 3 };

			qDebug() << channels[0] << channels[1] << channels[2];

			shaderProgram->setUniformValue("colorMap", 0);
			shaderProgram->setUniformValueArray("channels", channels, 3);
			shaderProgram->setUniformValueArray("noChannels", channels, 3);

			const QVector2D displayRange[3] = {
				_channels[0].displayRange(Qt::EditRole).value<Range>().toVector2D(),
				_channels[1].displayRange(Qt::EditRole).value<Range>().toVector2D(),
				_channels[2].displayRange(Qt::EditRole).value<Range>().toVector2D()
			};

			qDebug() << displayRange[0] << displayRange[1] << displayRange[2];

			shaderProgram->setUniformValueArray("channelRange", displayRange, 3);
			shaderProgram->setUniformValue("opacity", opacity);
			shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		if (textureByName("Channel2")->isCreated())
			textureByName("Channel2")->release();

		if (textureByName("Channel1")->isCreated())
			textureByName("Channel1")->release();

		if (textureByName("Channel0")->isCreated())
			textureByName("Channel0")->release();

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

WindowLevelImage& PointsProp::channel(const int& id)
{
	return _channels[id];
}

const WindowLevelImage& PointsProp::channel(const int& id) const
{
	return _channels[id];
}

void PointsProp::setChannelImage(const std::uint32_t& channelId, const QImage& image)
{
	//qDebug() << fullName() << "set image";

	renderer->bindOpenGLContext();
	{
		_channels[channelId].setImage(image);

		auto texture = textureByName(QString("Channel%1").arg(channelId));

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

void PointsProp::setColorMap(const QImage& colorMap)
{
	auto texture = textureByName("ColorMap");

	texture->destroy();
	texture->create();
	texture->setSize(colorMap.size().width(), colorMap.size().height());
	texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
	texture->setWrapMode(QOpenGLTexture::ClampToEdge);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	texture->allocateStorage();
	texture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt8, colorMap.bits());
}

void PointsProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}