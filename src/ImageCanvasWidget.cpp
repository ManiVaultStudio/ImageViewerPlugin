#include "ImageCanvasWidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

ImageCanvasWidget::ImageCanvasWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	clearColour(Qt::black),
	program(0),
	_imageTexture(QOpenGLTexture::Target2D),
	_aspectRatio(1.0),
	_window(1.0f),
	_level(0.5f),
	_imageMin(0.f),
	_imageMax(0.f)
{
	hasTexture = false;

	connect(this, &ImageCanvasWidget::windowLevelChanged, [this]() { update(); });
}

ImageCanvasWidget::~ImageCanvasWidget()
{
	makeCurrent();

	if (vbo.isCreated())
		vbo.destroy();
	
	if (_imageTexture.isCreated())
		_imageTexture.destroy();

	delete program;

	doneCurrent();
}

void ImageCanvasWidget::setImage(std::vector<std::uint16_t>& image, const QSize& imageSize)
{
	makeCurrent();

	if (width() != imageSize.width() || height() != imageSize.height()) {
		setFixedWidth(imageSize.width());
		setFixedHeight(imageSize.height());

		_imageTexture.destroy();
		_imageTexture.create();
		_imageTexture.setSize(imageSize.width(), imageSize.height(), 1);
		_imageTexture.setFormat(QOpenGLTexture::TextureFormat::R16_UNorm);
		_imageTexture.allocateStorage();
		_imageTexture.setMinMagFilters(QOpenGLTexture::Filter::Linear, QOpenGLTexture::Filter::Linear);
	}

	_imageTexture.setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(image.data()));

	hasTexture = true;
	_aspectRatio = (float)(imageSize.height()) / imageSize.width();

	

	makeObject();
	doneCurrent();

	
	update();
}

void ImageCanvasWidget::setClearColor(const QColor &color)
{
	clearColour = color;
	update();
}

// Run once when widget is set up
void ImageCanvasWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

	
}

const std::string imageQuadFragmentShaderSource =
#include "ImageQuadFragment.glsl"
;

void ImageCanvasWidget::paintGL()
{
	glClearColor(clearColour.redF(), clearColour.greenF(), clearColour.blueF(), clearColour.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	const char *vsrc =
		"attribute highp vec4 vertex;\n"
		"attribute mediump vec4 texCoord;\n"
		"varying mediump vec4 texc;\n"
		"uniform mediump mat4 matrix;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = matrix * vertex;\n"
		"    texc = texCoord;\n"
		"}\n";
	vshader->compileSourceCode(vsrc);

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char *fsrc =
		"uniform sampler2D texture;\n"
		"varying mediump vec4 texc;\n"
		"void main(void)\n"
		"{\n"
			" float value = texture2D(texture, texc.st).r;\n"
		"    gl_FragColor = vec4(vec3(value), 1);\n"
		"}\n";
	fshader->compileSourceCode(imageQuadFragmentShaderSource.c_str());

	double window = 0.0;
	double level = 0.0;

	computeWindowLevel(window, level);

	const auto minPixelValue = std::clamp(_imageMin, level - (window / 2.0), _imageMax);
	const auto maxPixelValue = std::clamp(_imageMin, level + (window / 2.0), _imageMax);

	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	program->link();

	program->bind();
	program->setUniformValue("texture", 0);

	program->setUniformValue("minPixelValue", static_cast<GLfloat>(minPixelValue));
	program->setUniformValue("maxPixelValue", static_cast<GLfloat>(maxPixelValue));


	QMatrix4x4 m;

	m.ortho(0.0f, +1.0f, _aspectRatio, 0.0f, 4.0f, 15.0f);
	m.translate(0.0f, 0.0f, -5.0f);

	program->setUniformValue("matrix", m);
	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

	if (hasTexture)
	{
		_imageTexture.bind();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		_imageTexture.release();
	}
}

void ImageCanvasWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

void ImageCanvasWidget::makeObject()
{
	static const float coords[4][3] = {
	  { 1.0f, _aspectRatio, 0.0f },
	  { 0.0f, _aspectRatio, 0.0f },
	  { 0.0f, 0.0f,     0.0f },
	  { 1.0f, 0.0f,     0.0f }
	};

	QVector<GLfloat> vertData;
	for (int j = 0; j < 4; ++j)
	{
		// vertex position
		vertData.append(1.0 * coords[j][0]);
		vertData.append(1.0 * coords[j][1]);
		vertData.append(1.0 * coords[j][2]);

		// texture coordinate
		vertData.append(j == 0 || j == 3);
		vertData.append(j == 2 || j == 3);
	}

	vbo.create();
	vbo.bind();
	vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

void ImageCanvasWidget::onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage, const double& imageMin, const double& imageMax)
{
	qDebug() << "Display image changed";

	auto shouldZoomExtents = false;

	/*
	if (imageSize != _imageSize) {
		_imageSize = imageSize;

		setupTextures();

		shouldZoomExtents = true;
	}

	_textures["image"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(displayImage.data()));
	*/

	setImage(displayImage, imageSize);

	/*
	_imageView->update();
	const auto size = 10000;

	_scene->update(-size / 2, -size / 2, size, size);

	update();
	*/
	/*
	if (shouldZoomExtents)
		zoomExtents();
	*/

	_imageMin = imageMin;
	_imageMax = imageMax;
}

double ImageCanvasWidget::window() const
{
	return _window;
}

void ImageCanvasWidget::setWindowLevel(const double& window, const double& level)
{
	if (window == _window && level == _level)
		return;

	qDebug() << "Set window/level" << window << level;

	_window = window;
	_level = level;

	emit windowLevelChanged(_window, _level);
}

void ImageCanvasWidget::resetWindowLevel()
{
	_window = 1.0;
	_level = 0.5;

	emit windowLevelChanged(_window, _level);
}

double ImageCanvasWidget::level() const
{
	return _level;
}

void ImageCanvasWidget::computeWindowLevel(double& window, double& level)
{
	const double min		= _imageMin;
	const double max		= _imageMax;
	const double maxWindow	= _imageMax - _imageMin;

	level	= std::clamp(min, min + _level * maxWindow, max);
	window	= std::clamp(min, _window * maxWindow, max);
}

void ImageCanvasWidget::mouseMoveEvent(QMouseEvent* mouseEvent) {

	qDebug() << "mouseMoveEvent";

	const auto deltaWindow = (mouseEvent->pos().x() - _mousePosition.x()) / 100.0;
	const auto deltaLevel = (mouseEvent->pos().y() - _mousePosition.y()) / 100.0;
	const auto window = std::max<double>(0, std::min<double>(_window + deltaWindow, 1.0f));
	const auto level = std::max<double>(0, std::min<double>(_level + deltaLevel, 1.0f));

	setWindowLevel(window, level);

	_mousePosition = mouseEvent->pos();
}