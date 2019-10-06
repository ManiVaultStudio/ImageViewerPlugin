#include "ImageWidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

const std::string vertexShaderSource =
#include "Vertex.glsl"
;

const std::string imageQuadFragmentShaderSource =
#include "ImageQuadFragment.glsl"
;

ImageWidget::ImageWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	clearColour(Qt::black),
	imageShaderProgram(0),
	_imageTexture(QOpenGLTexture::Target2D),
	_aspectRatio(1.0),
	_initialMousePosition(),
	_selecting(false),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_brushRadius(10.f),
	_brushRadiusDelta(2.0f),
	_window(1.0f),
	_level(0.5f),
	_imageMin(0.f),
	_imageMax(0.f)
{
	hasTexture = false;

	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(8);

	setFormat(surfaceFormat);

	setFixedWidth(0);
	setFixedHeight(0);

	//setWindowFlag(Qt::WA_PaintOnScreen, false);
	//setWindowFlags(windowFlags() & (~Qt::WA_PaintOnScreen));
}

ImageWidget::~ImageWidget()
{
	makeCurrent();

	if (vbo.isCreated())
		vbo.destroy();
	
	if (_imageTexture.isCreated())
		_imageTexture.destroy();

	delete imageShaderProgram;

	doneCurrent();
}

void ImageWidget::setImage(std::vector<std::uint16_t>& image, const QSize& size, const double& imageMin, const double& imageMax)
{
	makeCurrent();

	if (width() != size.width() || height() != size.height()) {
		setFixedWidth(size.width());
		setFixedHeight(size.height());

		_imageMin = imageMin;
		_imageMax = imageMax;

		_imageTexture.destroy();
		_imageTexture.create();
		_imageTexture.setSize(size.width(), size.height(), 1);
		_imageTexture.setFormat(QOpenGLTexture::TextureFormat::R16_UNorm);
		_imageTexture.allocateStorage();
		_imageTexture.setMinMagFilters(QOpenGLTexture::Filter::Linear, QOpenGLTexture::Filter::Linear);
	}

	_imageTexture.setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(image.data()));

	hasTexture = true;
	_aspectRatio = (float)(size.height()) / size.width();

	makeObject();
	doneCurrent();
	
	update();
}

// Run once when widget is set up
void ImageWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

	QOpenGLShader* vertexShader				= new QOpenGLShader(QOpenGLShader::Vertex, this);
	QOpenGLShader* imageFragmentShader		= new QOpenGLShader(QOpenGLShader::Fragment, this);
	QOpenGLShader* overlayFragmentShader	= new QOpenGLShader(QOpenGLShader::Fragment, this);
	QOpenGLShader* selectionFragmentShader	= new QOpenGLShader(QOpenGLShader::Fragment, this);

	vertexShader->compileSourceCode(vertexShaderSource.c_str());
	imageFragmentShader->compileSourceCode(imageQuadFragmentShaderSource.c_str());

	imageShaderProgram = new QOpenGLShaderProgram;

	imageShaderProgram->addShader(vertexShader);
	imageShaderProgram->addShader(imageFragmentShader);
	imageShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	imageShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	imageShaderProgram->link();
}

void ImageWidget::paintGL()
{
	glClearColor(clearColour.redF(), clearColour.greenF(), clearColour.blueF(), clearColour.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double window = 0.0;
	double level = 0.0;

	computeWindowLevel(window, level);

	const auto minPixelValue = std::clamp(_imageMin, level - (window / 2.0), _imageMax);
	const auto maxPixelValue = std::clamp(_imageMin, level + (window / 2.0), _imageMax);

	imageShaderProgram->bind();
	imageShaderProgram->setUniformValue("texture", 0);

	imageShaderProgram->setUniformValue("minPixelValue", static_cast<GLfloat>(minPixelValue));
	imageShaderProgram->setUniformValue("maxPixelValue", static_cast<GLfloat>(maxPixelValue));

	QMatrix4x4 m;

	m.ortho(0.0f, +1.0f, _aspectRatio, 0.0f, 4.0f, 15.0f);
	m.translate(0.0f, 0.0f, -5.0f);

	imageShaderProgram->setUniformValue("matrix", m);
	imageShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	imageShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	imageShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
	imageShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

	if (hasTexture)
	{
		_imageTexture.bind();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		_imageTexture.release();
	}
}

void ImageWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

void ImageWidget::makeObject()
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

double ImageWidget::window() const
{
	return _window;
}

void ImageWidget::setWindowLevel(const double& window, const double& level)
{
	if (window == _window && level == _level)
		return;

	qDebug() << "Set window/level" << window << level;

	_window = window;
	_level = level;

	update();
}

void ImageWidget::resetWindowLevel()
{
	_window = 1.0;
	_level = 0.5;

	update();
}

void ImageWidget::update()
{
	emit rendered();
}

double ImageWidget::level() const
{
	return _level;
}

void ImageWidget::computeWindowLevel(double& window, double& level)
{
	const double min		= _imageMin;
	const double max		= _imageMax;
	const double maxWindow	= _imageMax - _imageMin;

	level	= std::clamp(min, min + _level * maxWindow, max);
	window	= std::clamp(min, _window * maxWindow, max);
}

SelectionType ImageWidget::selectionType() const
{
	return _selectionType;
}

void ImageWidget::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;

	qDebug() << "Set selection type to" << selectionTypeTypeName(selectionType);

	_selectionType = selectionType;

	if (selectionType == SelectionType::Brush) {
		_selectionModifier = SelectionModifier::Add;
	}
	else {
		_selectionModifier = SelectionModifier::Replace;
	}

	update();

	emit selectionTypeChanged();
}

SelectionModifier ImageWidget::selectionModifier() const
{
	return _selectionModifier;
}

void ImageWidget::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	qDebug() << "Set selection modifier to" << selectionModifierName(selectionModifier);

	if (selectionType() == SelectionType::Brush && selectionModifier == SelectionModifier::Replace) {
	}
	else {
		_selectionModifier = selectionModifier;

		emit selectionModifierChanged();
	}
}

void ImageWidget::setBrushRadius(const float& brushRadius)
{
	qDebug() << "Set brush radius" << brushRadius;

	_brushRadius = qBound(0.01f, 10000.f, brushRadius);

	update();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	//if (!imageInitialized())
	//	return;

	/*
	qDebug() << "Mouse press event";

	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
	{
		_mousePosition = mouseEvent->pos();

		switch (_interactionMode)
		{
		case InteractionMode::Navigation:
			break;
		case InteractionMode::Selection:
		{
			if (_imageViewerPlugin->selectable()) {
				if (_selectionModifier == SelectionModifier::Replace) {
					qDebug() << "Reset selection";

					_imageViewerPlugin->setSelection(Indices());
				}

	_initialMousePosition = _mousePosition;

	enableSelection(true);
}

break;
		}
		case InteractionMode::WindowLevel:
			break;
		default:
			break;
		}

		break;
	}

	case Qt::RightButton:
	{
		setInteractionMode(InteractionMode::WindowLevel);
		break;
	}

	default:
		break;
	}*/
}

/*
void ImageWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "mouseMoveEvent";
	
	qDebug() << "mouseMoveEvent";

	const auto deltaWindow = (mouseEvent->pos().x() - _mousePosition.x()) / 100.0;
	const auto deltaLevel = (mouseEvent->pos().y() - _mousePosition.y()) / 100.0;
	const auto window = std::max<double>(0, std::min<double>(_window + deltaWindow, 1.0f));
	const auto level = std::max<double>(0, std::min<double>(_level + deltaLevel, 1.0f));

	setWindowLevel(window, level);

	_mousePosition = mouseEvent->pos();

	if (!imageInitialized())
		return;

	//qDebug() << "Mouse move event";

	switch (mouseEvent->buttons())
	{
	case Qt::LeftButton:
	{
		switch (_interactionMode)
		{
		case InteractionMode::Navigation:
		{
			pan(QPointF(mouseEvent->pos().x() - _mousePosition.x(), -(mouseEvent->pos().y() - _mousePosition.y())));
			break;
		}

		case InteractionMode::Selection:
		{
			if (_imageViewerPlugin->selectable()) {
				updateSelection();
			}
			break;
		}

		case InteractionMode::WindowLevel:
		{
			break;
		}

		default:
			break;
		}

		update();

		break;
	}

	case Qt::RightButton:
	{
		const auto deltaWindow	= (mouseEvent->pos().x() - _mousePosition.x()) / static_cast<double>(_imageSize.width());
		const auto deltaLevel	= (mouseEvent->pos().y() - _mousePosition.y()) / static_cast<double>(_imageSize.height());
		const auto window		= std::max<double>(0, std::min<double>(_imageViewerPlugin->window() + deltaWindow, 1.0f));
		const auto level		= std::max<double>(0, std::min<double>(_imageViewerPlugin->level() + deltaLevel, 1.0f));

		_imageViewerPlugin->setWindowLevel(window, level);

		break;
	}

	default:
		break;
	}

	_mousePosition = mouseEvent->pos();
	
}
*/
void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	/*
	if (!imageInitialized())
		return;

	qDebug() << "Mouse release event";

	if (_interactionMode != InteractionMode::WindowLevel) {
		if (_imageViewerPlugin->selectable()) {
			if (mouseEvent->button() == Qt::RightButton)
			{
				contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
			}
		}
	}

	switch (_interactionMode)
	{
	case InteractionMode::Navigation:
	{
		QWidget::setCursor(Qt::OpenHandCursor);
		break;
	}

	case InteractionMode::Selection:
	{
		if (_imageViewerPlugin->selectable()) {
			if (_selecting) {
				if (_imageViewerPlugin->selectable()) {
					enableSelection(false);
					updateSelection();
				}

				commitSelection();
			}
		}
		break;
	}

	case InteractionMode::WindowLevel:
	{
		setInteractionMode(InteractionMode::Selection);
		break;
	}

	default:
		break;
	}

	update();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
	*/
}

void ImageWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	/*
	if (!imageInitialized())
		return;

	qDebug() << "Mouse wheel event";

	switch (_interactionMode)
	{
	case InteractionMode::Navigation:
	{
		const auto world_x = (wheelEvent->posF().x() - _pan.x()) / _zoom;
		const auto world_y = (wheelEvent->posF().y() - _pan.y()) / _zoom;

		auto zoomCenter = wheelEvent->posF();

		zoomCenter.setY(height() - wheelEvent->posF().y());

		if (wheelEvent->delta() > 0) {
			zoomAt(zoomCenter, 1.f - _zoomSensitivity);
		}
		else {
			zoomAt(zoomCenter, 1.f + _zoomSensitivity);
		}

		update();
		break;
	}
	case InteractionMode::Selection:
	{
		if (_selectionType == SelectionType::Brush) {
			if (wheelEvent->delta() > 0) {
				setBrushRadius(_brushRadius + _brushRadiusDelta);
			}
			else {
				setBrushRadius(_brushRadius - _brushRadiusDelta);
			}
		}

		break;
	}
	case InteractionMode::WindowLevel:
		break;
	default:
		break;
	}
	*/
}