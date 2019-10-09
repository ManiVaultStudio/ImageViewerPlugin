#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include "PointsPlugin.h"

#include <vector>
#include <set>
#include <algorithm>

#include <QSize>
#include <QDebug>
#include <QMenu>
#include <QList>
#include <QtMath>
#include <QPainter>
#include <QGuiApplication>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7

const std::string vertexShaderSource =
#include "Vertex.glsl"
;

const std::string imageFragmentShaderSource =
#include "ImageFragment.glsl"
;

const std::string overlayFragmentShaderSource =
#include "OverlayFragment.glsl"
;

const std::string selectionFragmentShaderSource =
#include "SelectionFragment.glsl"
;

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_textures(),
	_shaders(),
	_interactionMode(InteractionMode::Selection),
	_initialMousePosition(),
	_mousePosition(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(25),
	_selecting(false),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_selectionRealtime(false),
	_brushRadius(10.f),
	_brushRadiusDelta(2.0f),
	_selectionColor(255, 0, 0, 200),
	_selectionProxyColor(245, 184, 17, 100),
	_selectionGeometryColor(255, 0, 0, 255),
	_selectedPointIds(),
	_zoomToExtentsAction(nullptr),
	_vertexBuffer(QOpenGLBuffer::VertexBuffer)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);

	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageViewerWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageViewerWidget::onSelectionImageChanged);
	//connect(_imageViewerPlugin, &ImageViewerPlugin::windowLevelChanged, [this]() { update(); });

	QSurfaceFormat surfaceFormat;

	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	//surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	//surfaceFormat.setVersion(4, 3);
	surfaceFormat.setSamples(16);
	//surfaceFormat.setDepthBufferSize(24);
	//surfaceFormat.setStencilBufferSize(8);

	setFormat(surfaceFormat);

	_textures.insert(std::pair<QString, std::unique_ptr<QOpenGLTexture>>("image", std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D)));
	_textures.insert(std::pair<QString, std::unique_ptr<QOpenGLTexture>>("overlay", std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D)));
	_textures.insert(std::pair<QString, std::unique_ptr<QOpenGLTexture>>("selection", std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D)));

	_shaders.insert(std::pair<QString, QOpenGLShaderProgram*>("image", new QOpenGLShaderProgram()));
	_shaders.insert(std::pair<QString, QOpenGLShaderProgram*>("overlay", new QOpenGLShaderProgram()));
	_shaders.insert(std::pair<QString, QOpenGLShaderProgram*>("selection", new QOpenGLShaderProgram()));
}

InteractionMode ImageViewerWidget::interactionMode() const
{
	return _interactionMode;
}

void ImageViewerWidget::setInteractionMode(const InteractionMode& interactionMode)
{
	if (interactionMode == _interactionMode)
		return;

	qDebug() << "Set interaction mode to" << interactionModeTypeName(interactionMode);

	switch (interactionMode)
	{
		case InteractionMode::Navigation:
			QWidget::setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
			QWidget::setCursor(Qt::ArrowCursor);
			break;

		default:
			break;
	}

	_interactionMode = interactionMode;
}

SelectionType ImageViewerWidget::selectionType() const
{
	return _selectionType;
}

void ImageViewerWidget::setSelectionType(const SelectionType& selectionType)
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

SelectionModifier ImageViewerWidget::selectionModifier() const
{
	return _selectionModifier;
}

void ImageViewerWidget::setSelectionModifier(const SelectionModifier& selectionModifier)
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

void ImageViewerWidget::setBrushRadius(const float& brushRadius)
{
	qDebug() << "Set brush radius" << brushRadius;

	_brushRadius = qBound(0.01f, 10000.f, brushRadius);

	update();

	emit brushRadiusChanged();
}

void ImageViewerWidget::onDisplayImageChanged(std::unique_ptr<Image<std::uint16_t>>& displayImage)
{
	if (!isValid())
		return;

	makeCurrent();

	qDebug() << "Display image changed" << *displayImage.get();

	auto imageSizeChanged = false;

	if (_displayImage.get() == nullptr)
		imageSizeChanged = true;
	else
		imageSizeChanged = displayImage->width() != _displayImage->width() || displayImage->height() != _displayImage->height();
	
	_displayImage.swap(displayImage);

	if (imageSizeChanged)
		setupTextures();

	_textures["image"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(_displayImage->pixels().data()));

	if (imageSizeChanged) {
		qDebug() << "Reset view";
		createImageQuad();
		zoomExtents();
	}

	doneCurrent();

	update();
}

void ImageViewerWidget::onSelectionImageChanged(std::unique_ptr<Image<std::uint8_t>>& selectionImage)
{
	if (!isValid())
		return;

	qDebug() << "Selection image changed";

	_selectionImage.swap(selectionImage);

	_textures["selection"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(_selectionImage->pixels().data()));

	update();
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	enableSelection(false);
}

void ImageViewerWidget::onCurrentImageIdChanged(const std::int32_t& currentImageId)
{
	enableSelection(false);

	update();
}

void ImageViewerWidget::drawCircle(const QPointF& center, const float& radius, const int& noSegments /*= 30*/)
{
	glBegin(GL_LINE_LOOP);

	for (int ii = 0; ii < noSegments; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(noSegments);
		float x = radius * cosf(theta);
		float y = radius * sinf(theta);

		glVertex2f(x + center.x(), y + center.y());
	}

	glEnd();
}

void ImageViewerWidget::drawSelectionRectangle(const QPoint& start, const QPoint& end)
{
	const auto z = -0.5;

	glColor4f(_selectionGeometryColor.red(), _selectionGeometryColor.green(), _selectionGeometryColor.blue(), 1.f);

	glBegin(GL_LINE_LOOP);

	glVertex3f(start.x(), height() - start.y(), z);
	glVertex3f(end.x(), height() - start.y(), z);
	glVertex3f(end.x(), height() - end.y(), z);
	glVertex3f(start.x(), height() - end.y(), z);

	glEnd();
}

void ImageViewerWidget::drawSelectionBrush()
{
	auto brushCenter = QWidget::mapFromGlobal(QCursor::pos());

	brushCenter.setY(height() - brushCenter.y());

	glColor4f(_selectionGeometryColor.red(), _selectionGeometryColor.green(), _selectionGeometryColor.blue(), 1.f);

	drawCircle(brushCenter, _brushRadius, 20);
}

void ImageViewerWidget::drawSelectionGeometry()
{
	switch (_selectionType)
	{
		case SelectionType::Rectangle:
		{
			if (_selecting) {
				const auto currentMouseWorldPos = QWidget::mapFromGlobal(QCursor::pos());
				drawSelectionRectangle(_initialMousePosition, currentMouseWorldPos);
			}

			break;
		}

		case SelectionType::Brush:
		{
			drawSelectionBrush();
			break;
		}

		default:
			break;
	}
}

void ImageViewerWidget::enableSelection(const bool& enable)
{
	_selecting = enable;

	update();
}

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

	QOpenGLShader* vertexShader				= new QOpenGLShader(QOpenGLShader::Vertex, this);
	QOpenGLShader* imageFragmentShader		= new QOpenGLShader(QOpenGLShader::Fragment, this);
	QOpenGLShader* overlayFragmentShader	= new QOpenGLShader(QOpenGLShader::Fragment, this);
	QOpenGLShader* selectionFragmentShader	= new QOpenGLShader(QOpenGLShader::Fragment, this);

	if (vertexShader->compileSourceCode(vertexShaderSource.c_str())) {
		if (imageFragmentShader->compileSourceCode(imageFragmentShaderSource.c_str())) {
			_imageShaderProgram.addShader(vertexShader);
			_imageShaderProgram.addShader(imageFragmentShader);
			_imageShaderProgram.bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
			_imageShaderProgram.bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
			_imageShaderProgram.link();
		}

		/*
		if (overlayFragmentShader->compileSourceCode(overlayFragmentShaderSource.c_str())) {
			_overlayShaderProgram->addShader(vertexShader);
			_overlayShaderProgram->addShader(overlayFragmentShader);
			_overlayShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
			_overlayShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
			_overlayShaderProgram->link();
		}
		*/

		if (selectionFragmentShader->compileSourceCode(selectionFragmentShaderSource.c_str())) {
			_selectionShaderProgram.addShader(vertexShader);
			_selectionShaderProgram.addShader(selectionFragmentShader);
			_selectionShaderProgram.bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
			_selectionShaderProgram.bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
			_selectionShaderProgram.link();
		}
	}

	_vertexBuffer.create();
	_vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";

	if (h == 0)
		h = 1;

//	glOrtho(0, w, 0, h, -100, 100);

	zoomExtents();
}

void ImageViewerWidget::paintGL() {

	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!initialized())
		return;

	if (!_vertexBuffer.bind())
		return;

	const auto halfSize = size() / 2;

	QMatrix4x4 projection, camera, model, transform;
	
	projection.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -10.0f, +10.0f);
	camera.lookAt(QVector3D(0, 0, -1), QVector3D(0, 0, 0), QVector3D(0, -1, 0));
	model.scale(_zoom, _zoom, 1.0f);
	model.translate(_pan.x(), _pan.y());
	
	transform = projection * camera * model;
	
	if (_imageShaderProgram.isLinked()) {
		_imageShaderProgram.bind();

		_imageShaderProgram.setUniformValue("imageTexture", 0);
		//_imageShaderProgram.setUniformValue("minPixelValue", static_cast<GLfloat>(minPixelValue));
		//_imageShaderProgram.setUniformValue("maxPixelValue", static_cast<GLfloat>(maxPixelValue));
		_imageShaderProgram.setUniformValue("minPixelValue", static_cast<float>(_displayImage->min()));
		_imageShaderProgram.setUniformValue("maxPixelValue", static_cast<float>(_displayImage->max()));
		_imageShaderProgram.setUniformValue("matrix", transform);
		_imageShaderProgram.enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_imageShaderProgram.enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_imageShaderProgram.setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_imageShaderProgram.setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

		if (_textures["image"]->isCreated()) {
			_textures["image"]->bind();

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_textures["image"]->release();
		}
	}

	if (_selectionShaderProgram.isLinked()) {
		transform.translate(0.0f, 0.0f, 1.0f);

		_selectionShaderProgram.bind();

		_selectionShaderProgram.setUniformValue("selectionTexture", 0);
		_selectionShaderProgram.setUniformValue("matrix", transform);
		_selectionShaderProgram.enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_selectionShaderProgram.enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_selectionShaderProgram.setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_selectionShaderProgram.setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

		if (_textures["selection"]->isCreated()) {
			_textures["selection"]->bind();

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_textures["selection"]->release();
		}
	}

	_vertexBuffer.release();

	/*
	if (_interactionMode == InteractionMode::Selection) {
		drawSelectionGeometry();
	}
	*/
}

void ImageViewerWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
		if (_interactionMode == InteractionMode::Selection) {
			switch (keyEvent->key())
			{
				case Qt::Key::Key_R:
					setSelectionType(SelectionType::Rectangle);
					break;

				case Qt::Key::Key_B:
					setSelectionType(SelectionType::Brush);
					break;

				case Qt::Key::Key_F:
					setSelectionType(SelectionType::Freehand);
					break;

				case Qt::Key::Key_Shift:
					setSelectionModifier(SelectionModifier::Add);
					break;

				case Qt::Key::Key_Control:
					setSelectionModifier(SelectionModifier::Remove);
					break;

				case Qt::Key::Key_Space:
					setInteractionMode(InteractionMode::Navigation);
					break;

				default:
					break;
			}
		}
	}

	QOpenGLWidget::keyPressEvent(keyEvent);
}

void ImageViewerWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
		switch (keyEvent->key())
		{
			case Qt::Key::Key_Shift:
			case Qt::Key::Key_Control:
			{
				if (selectionType() != SelectionType::Brush) {
					setSelectionModifier(SelectionModifier::Replace);
				}

				break;
			}

			case Qt::Key::Key_Space:
				setInteractionMode(InteractionMode::Selection);
				break;

			default:
				break;
		}
	}

	QOpenGLWidget::keyReleaseEvent(keyEvent);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (!initialized())
		return;

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
						/*
						if (_selectionModifier == SelectionModifier::Replace) {
							qDebug() << "Reset selection";

							_imageViewerPlugin->setSelection(Indices());
						}
						*/

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
	}
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent) {

	if (!initialized())
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
					pan(QPointF(mouseEvent->pos().x() - _mousePosition.x(), mouseEvent->pos().y() - _mousePosition.y()));
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
			const auto deltaWindow = (mouseEvent->pos().x() - _mousePosition.x()) / static_cast<double>(_displayImage->width());
			const auto deltaLevel = (mouseEvent->pos().y() - _mousePosition.y()) / static_cast<double>(_displayImage->height());
			//const auto window = std::max<double>(0, std::min<double>(_imageViewerPlugin->window() + deltaWindow, 1.0f));
			//const auto level = std::max<double>(0, std::min<double>(_imageViewerPlugin->level() + deltaLevel, 1.0f));

			//_imageViewerPlugin->setWindowLevel(window, level);

			break;
		}

		default:
			break;
	}

	_mousePosition = mouseEvent->pos();
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent) {

	if (!initialized())
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
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent) {

	if (!initialized())
		return;

	qDebug() << "Mouse wheel event";

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			const auto world_x = (wheelEvent->posF().x() - _pan.x()) / _zoom;
			const auto world_y = (wheelEvent->posF().y() - _pan.y()) / _zoom;

			auto zoomCenter = wheelEvent->posF();

			//zoomCenter.setY(height() - wheelEvent->posF().y());

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
}

void ImageViewerWidget::pan(const QPointF& delta) {

	qDebug() << "Pan" << delta;

	_pan.setX(_pan.x() + (delta.x() / _zoom));
	_pan.setY(_pan.y() + (delta.y() / _zoom));
}

void ImageViewerWidget::zoom(const float& factor) {

	qDebug() << "Zoom" << factor;

	_zoom *= factor;

	_pan.setX(_pan.x() * factor);
	_pan.setY(_pan.y() * factor);
}

void ImageViewerWidget::zoomAt(const QPointF& screenPosition, const float& factor) {

	qDebug() << "Zoom at" << screenPosition << factor;

	//pan(QPointF(-screenPosition.x(), -screenPosition.y()));
	zoom(factor);
	//pan(QPointF(screenPosition.x(), screenPosition.y()));
}

void ImageViewerWidget::zoomExtents()
{
	if (_imageViewerPlugin->currentDataset().isEmpty())
		return;
	
	qDebug() << "Zoom extents";

	resetView();

	const auto factorX = (width() - _margin) / static_cast<float>(_displayImage->width());
	const auto factorY = (height() - _margin) / static_cast<float>(_displayImage->height());

	zoom(factorX < factorY ? factorX : factorY);
	pan(QPointF(0, 0));

	update();
}

void ImageViewerWidget::resetView()
{
	qDebug() << "Reset view";

	_pan.setX(0);
	_pan.setY(0);

	_zoom = 1.f;

	update();
}

bool ImageViewerWidget::initialized()
{
	return _displayImage.get() != nullptr;
}

QPoint ImageViewerWidget::screenToWorld(const QPoint& screen) const
{
	return QPoint(((screen.x()) / _zoom) - _pan.x(), -((-(screen.y() - height()) / _zoom) - _pan.y()));
}

QPoint ImageViewerWidget::worldToScreen(const QPoint& world) const
{
	return QPoint();
}

void ImageViewerWidget::createImageQuad()
{
	qDebug() << "Create image quad" << *_displayImage.get();

	const auto halfWidth	= _displayImage->width() / 2.0f;
	const auto halfHeight	= _displayImage->height() / 2.0f;
	
	const float coordinates[4][3] = {
	  { halfWidth, halfHeight, 0.0f },
	  { -halfWidth, halfHeight, 0.0f },
	  { -halfWidth, -halfHeight, 0.0f },
	  { halfWidth, -halfHeight, 0.0f }
	};
	
	QVector<GLfloat> vertexData;

	for (int j = 0; j < 4; ++j)
	{
		// vertex position
		vertexData.append(1.0 * coordinates[j][0]);
		vertexData.append(1.0 * coordinates[j][1]);
		vertexData.append(1.0 * coordinates[j][2]);

		// texture coordinate
		vertexData.append(j == 0 || j == 3);
		vertexData.append(j == 2 || j == 3);
	}

	_vertexBuffer.bind();
	_vertexBuffer.allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
	_vertexBuffer.release();
}

void ImageViewerWidget::updateSelection()
{
	//qDebug() << "Update selection" << _selectionType;

	/*
	const auto halfImageSize = _imageSize / 2;
	const auto imageRect = QRect(-halfImageSize.width(), -halfImageSize.height(), _imageSize.width(), _imageSize.height());

	auto overlayTextureData = TextureData();

	overlayTextureData.resize(_imageSize.width() * _imageSize.height());

	switch (_selectionType)
	{
		case SelectionType::Rectangle: {
			const auto initialMouseWorldPos = screenToWorld(QPoint(_initialMousePosition.x(), _initialMousePosition.y()));
			const auto currentMouseWorldPos = screenToWorld(QPoint(_mousePosition.x(), _mousePosition.y()));
			const auto selectionTopLeft = QPoint(qMin(initialMouseWorldPos.x(), currentMouseWorldPos.x()), qMin(initialMouseWorldPos.y(), currentMouseWorldPos.y()));
			const auto selectionBottomRight = QPoint(qMax(initialMouseWorldPos.x(), currentMouseWorldPos.x()), qMax(initialMouseWorldPos.y(), currentMouseWorldPos.y()));
			const auto selectionRect = QRect(selectionTopLeft, selectionBottomRight);

			if (imageRect.intersects(selectionRect)) {
				const auto imageSelection = selectionRect.intersected(imageRect);
				const auto noSelectedPixels = imageSelection.width() * imageSelection.height();

				auto selectedPointIds = Indices();

				selectedPointIds.reserve(noSelectedPixels);

				const auto left = imageSelection.x() + halfImageSize.width();
				const auto right = (imageSelection.x() + imageSelection.width()) + halfImageSize.width();
				const auto top = imageSelection.y() + halfImageSize.height();
				const auto bottom = (imageSelection.y() + imageSelection.height()) + halfImageSize.height();
				const auto pixelOffset = _imageViewerPlugin->pixelOffset();

				for (std::int32_t x = left; x < right; x++) {
					for (std::int32_t y = top; y < bottom; y++) {
						const auto imageY = _imageSize.height() - y;
						const auto pointId = imageY * _imageSize.width() + x;

						selectedPointIds.push_back(pointId);
					}
				}

				modifySelection(selectedPointIds, pixelOffset);
			}

			break;
		}

		case SelectionType::Brush: {
			const auto currentMouseWorldPos = screenToWorld(QPoint(_mousePosition.x(), _mousePosition.y()));
			const auto brushRadius = _brushRadius / _zoom;
			const auto offset = QPoint(qCeil(brushRadius), qCeil(brushRadius));
			const auto selectionRect = QRect(currentMouseWorldPos - offset, currentMouseWorldPos + offset);

			if (imageRect.intersects(selectionRect)) {
				const auto imageSelection = selectionRect.intersected(imageRect);
				const auto noSelectedPixels = imageSelection.width() * imageSelection.height();

				auto selectedPointIds = Indices();

				selectedPointIds.reserve(noSelectedPixels);

				const auto left = imageSelection.x() + halfImageSize.width();
				const auto right = (imageSelection.x() + imageSelection.width()) + halfImageSize.width();
				const auto top = imageSelection.y() + halfImageSize.height();
				const auto bottom = (imageSelection.y() + imageSelection.height()) + halfImageSize.height();
				const auto center = currentMouseWorldPos - imageRect.topLeft() + QPointF(0.5f, 0.5f);
				const auto pixelOffset = _imageViewerPlugin->pixelOffset();

				for (std::int32_t x = left; x < right; x++) {
					for (std::int32_t y = top; y < bottom; y++) {
						const auto pixelCenter = QVector2D(x + 0.5f, y + 0.5f);

						if ((pixelCenter - QVector2D(center)).length() < (_brushRadius / _zoom)) {
							const auto imageY = _imageSize.height() - y;
							const auto pointId = imageY * _imageSize.width() + x;

							selectedPointIds.push_back(pointId);
						}
					}
				}

				modifySelection(selectedPointIds, pixelOffset);
			}

			break;
		}

		default:
			break;
	}

	_textures["overlay"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(overlayTextureData.data()));

	update();
	*/
}

void ImageViewerWidget::modifySelection(const Indices& selectedPointIds, const std::int32_t& pixelOffset /*= 0*/)
{
	qDebug() << "Modify selection";

	if (selectedPointIds.size() > 0) {
		switch (_selectionModifier) {
			case SelectionModifier::Replace:
			{
				//qDebug() << "Replace selection";

				_selectedPointIds = selectedPointIds;

				break;
			}

			case SelectionModifier::Add:
			{
				//qDebug() << "Add to selection";

				auto selectionSet = std::set<Index>(_selectedPointIds.begin(), _selectedPointIds.end());

				for (auto& pixelId : selectedPointIds) {
					selectionSet.insert(pixelId);
				}

				_selectedPointIds = Indices(selectionSet.begin(), selectionSet.end());

				break;
			}

			case SelectionModifier::Remove:
			{
				//qDebug() << "Remove from selection";

				auto selectionSet = std::set<Index>(_selectedPointIds.begin(), _selectedPointIds.end());

				for (auto& pixelId : selectedPointIds) {
					selectionSet.erase(pixelId);
				}

				_selectedPointIds = Indices(selectionSet.begin(), selectionSet.end());

				break;
			}
		}
	}
	else
	{
		_selectedPointIds = Indices();
	}

	auto overlayTextureData = std::vector<std::uint16_t>();

	overlayTextureData.resize(_displayImage->width() * _displayImage->height());

	for (auto& selectedPointId : _selectedPointIds) {
		overlayTextureData[selectedPointId - pixelOffset] = 1;
	}

	qDebug() << overlayTextureData;

	_textures["overlay"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(overlayTextureData.data()));

	update();
}

void ImageViewerWidget::clearSelection()
{
	qDebug() << "Clear selection";

	modifySelection(Indices());
	commitSelection();
}

void ImageViewerWidget::commitSelection()
{
	qDebug() << "Commit selection to core";

	// resetTextureData("overlay");

	_imageViewerPlugin->setSelection(_selectedPointIds);
}

QMenu* ImageViewerWidget::contextMenu()
{
	auto* contextMenu = new QMenu();

	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		contextMenu->addMenu(viewMenu());
		contextMenu->addSeparator();
		contextMenu->addMenu(selectionMenu());
	}

	return contextMenu;
}

QMenu* ImageViewerWidget::viewMenu()
{
	auto* viewMenu = new QMenu("View");

	auto* zoomToExtentsAction = new QAction("Zoom extents");

	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");

	connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);

	viewMenu->addAction(zoomToExtentsAction);

	return viewMenu;
}

QMenu* ImageViewerWidget::selectionMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction = new QAction("Rectangle");
	auto* brushSelectionAction = new QAction("Brush");
	auto* freehandSelectionAction = new QAction("Freehand", this);
	auto* clearSelectionAction = new QAction("Clear");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Rectangle);  });
	connect(brushSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Brush);  });
	connect(freehandSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Freehand);  });
	connect(clearSelectionAction, &QAction::triggered, [this]() { clearSelection(); });

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);

	rectangleSelectionAction->setChecked(_selectionType == SelectionType::Rectangle);
	brushSelectionAction->setChecked(_selectionType == SelectionType::Brush);

	freehandSelectionAction->setEnabled(false);

	selectionMenu->addAction(rectangleSelectionAction);
	selectionMenu->addAction(brushSelectionAction);
	selectionMenu->addSeparator();
	selectionMenu->addAction(clearSelectionAction);

	return selectionMenu;
}

void ImageViewerWidget::setupTextures()
{
	qDebug() << "Setup textures";

	setupTexture(_textures["image"].get(), QOpenGLTexture::TextureFormat::R16_UNorm);
	setupTexture(_textures["overlay"].get(), QOpenGLTexture::TextureFormat::R16_UNorm);// , QOpenGLTexture::Filter::Nearest);
	setupTexture(_textures["selection"].get(), QOpenGLTexture::TextureFormat::R8_UNorm, QOpenGLTexture::Filter::Nearest);
}

void ImageViewerWidget::setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter /*= QOpenGLTexture::Filter::Linear*/)
{
	qDebug() << "Setup texture" << *_displayImage.get();

	openGltexture->destroy();
	openGltexture->create();
	openGltexture->setSize(_displayImage->width(), _displayImage->height(), 1);
	openGltexture->setFormat(textureFormat);
	openGltexture->allocateStorage();
	openGltexture->setMinMagFilters(filter, filter);
}