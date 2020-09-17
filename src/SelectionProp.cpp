#include "SelectionProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "Range.h"
#include "Renderer.h"
#include "SelectionLayer.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPixelTransferOptions>

#include <stdexcept> // For runtime_error.

const std::string vertexShaderSource =
	#include "SelectionVertex.glsl"
;

const std::string fragmentShaderSource =
	#include "SelectionFragment.glsl"
;

SelectionProp::SelectionProp(SelectionLayer* selectionLayer, const QString& name) :
	Prop(reinterpret_cast<Node*>(selectionLayer), name)
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Channels", QOpenGLTexture::Target2DArray);

	QObject::connect(selectionLayer, &SelectionLayer::channelChanged, [this, selectionLayer](const std::uint32_t& channelId) {
		try
		{
			renderer->bindOpenGLContext();

			auto channel = selectionLayer->channel(channelId);

			const auto imageSize = channel->getImageSize();

			if (!imageSize.isValid())
				return;

			auto texture = textureByName("Channels");

			if (!texture->isCreated())
				texture->create();

			if (imageSize != QSize(texture->width(), texture->height())) {
				texture->destroy();
				texture->create();
				texture->setLayers(ult(SelectionLayer::ChannelIndex::Count));
				texture->setSize(imageSize.width(), imageSize.height(), 1);
				texture->setFormat(QOpenGLTexture::R8_UNorm);
				texture->setWrapMode(QOpenGLTexture::ClampToEdge);
				texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
				texture->allocateStorage();
			}

			QOpenGLPixelTransferOptions options;

			options.setAlignment(1);

			texture->setData(0, channel->getId(), QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, channel->getElements().data(), &options);// channel->elements().data());

			const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(imageSize));

			this->shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

			updateModelMatrix();
		}
		catch (std::exception& e)
		{
			qDebug() << _name << "channel texture update failed:" << e.what();
		}
		catch (...) {
			qDebug() << _name << "channel texture update failed due to unhandled exception";
		}
	});

	initialize();
}

SelectionProp::~SelectionProp() = default;

void SelectionProp::initialize()
{
	try
	{
		renderer->bindOpenGLContext();

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

		if (!shaderProgram->bind())
			throw std::runtime_error("Unable to bind quad shader program");

		shape->getVAO().bind();
		shape->getVBO().bind();

		shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
		shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
		shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
		shaderProgram->release();

		shape->getVAO().release();
		shape->getVBO().release();

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

void SelectionProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
	try {
		if (!canRender())
			return;

		Prop::render(nodeMVP, opacity);

		const auto shape			= shapeByName<QuadShape>("Quad");
		const auto shaderProgram	= shaderProgramByName("Quad");
		const auto quadTexture		= textureByName("Quad");

		if (!textureByName("Channels")->isCreated())
			throw std::runtime_error("Channels texture is not created");

		renderer->getOpenGLContext()->functions()->glActiveTexture(GL_TEXTURE0);

		textureByName("Channels")->bind();

		if (!shaderProgram->bind())
			throw std::runtime_error("Unable to bind shader program");

		auto selectionLayer = static_cast<SelectionLayer*>(_node);

		const auto overlayColor = selectionLayer->overlayColor(Qt::EditRole).value<QColor>();

		shaderProgram->setUniformValue("channelTextures", 0);
		shaderProgram->setUniformValue("textureSize", QSizeF(selectionLayer->imageSize()));
		shaderProgram->setUniformValue("overlayColor", overlayColor);
		shaderProgram->setUniformValue("opacity", opacity);
		shaderProgram->setUniformValue("transform", nodeMVP * modelMatrix());

		shape->render();

		shaderProgram->release();

		textureByName("Channels")->release();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

QRectF SelectionProp::getBoundingRectangle() const
{
	return shapeByName<QuadShape>("Quad")->rectangle();
}

void SelectionProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = shapeByName<QuadShape>("Quad")->rectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}