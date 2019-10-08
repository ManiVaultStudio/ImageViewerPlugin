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
	_mousePosition(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(25),
	_selectionRealtime(false),
	_selectionColor(255, 0, 0, 200),
	_selectionProxyColor(245, 184, 17, 100),
	_selectionGeometryColor(255, 0, 0, 255),
	_selectedPointIds(),
	_zoomToExtentsAction(nullptr),
	_imageSize(),
	_vertexBuffer(QOpenGLBuffer::VertexBuffer)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	setMouseTracking(true);

	//connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	//connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	//connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageViewerWidget::onSelectionImageChanged);
	
	QSurfaceFormat surfaceFormat;
	
	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	surfaceFormat.setVersion(3, 3);
	surfaceFormat.setSamples(16);
	//surfaceFormat.setDepthBufferSize(24);
	//surfaceFormat.setStencilBufferSize(8);

	setFormat(surfaceFormat);

	/*
	_textures.insert(std::pair<QString, std::unique_ptr<QOpenGLTexture>>("image", std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D)));
	_textures.insert(std::pair<QString, std::unique_ptr<QOpenGLTexture>>("overlay", std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D)));
	_textures.insert(std::pair<QString, std::unique_ptr<QOpenGLTexture>>("selection", std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D)));

	_shaders.insert(std::pair<QString, QOpenGLShaderProgram*>("image", new QOpenGLShaderProgram()));
	_shaders.insert(std::pair<QString, QOpenGLShaderProgram*>("overlay", new QOpenGLShaderProgram()));
	_shaders.insert(std::pair<QString, QOpenGLShaderProgram*>("selection", new QOpenGLShaderProgram()));
	
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageViewerWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageViewerWidget::onSelectionImageChanged);
	*/
}

/*


void ImageViewerWidget::onCurrentImageIdChanged(const std::int32_t& currentImageId)
{
	enableSelection(false);

	update();
}

void ImageViewerWidget::drawQuad(const float& z) {
	const auto halfImageSize = _imageSize / 2;

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); glVertex3f(-halfImageSize.width(), -halfImageSize.height(), z);
		glTexCoord2f(0, 1); glVertex3f(-halfImageSize.width(), halfImageSize.height(), z);
		glTexCoord2f(1, 1); glVertex3f(halfImageSize.width(), halfImageSize.height(), z);
		glTexCoord2f(1, 0); glVertex3f(halfImageSize.width(), -halfImageSize.height(), z);
	}
	glEnd();
}

void ImageViewerWidget::drawCircle(const QPointF& center, const float& radius, const int& noSegments)
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

//	drawCircle(brushCenter, _brushRadius, 20);
}

void ImageViewerWidget::drawTextureQuad(QOpenGLTexture& texture, const float& z)
{
	glEnable(GL_TEXTURE_2D);

	texture.bind();

	drawQuad(z);

	texture.release();

	glDisable(GL_TEXTURE_2D);
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
*/

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

	QOpenGLShader* vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	QOpenGLShader* imageFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	QOpenGLShader* overlayFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	QOpenGLShader* selectionFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);

	if (vertexShader->compileSourceCode(vertexShaderSource.c_str())) {
		if (imageFragmentShader->compileSourceCode(imageFragmentShaderSource.c_str())) {
			_imageShaderProgram->addShader(vertexShader);
			_imageShaderProgram->addShader(imageFragmentShader);
			_imageShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
			_imageShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
			_imageShaderProgram->link();
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
			_selectionShaderProgram->addShader(vertexShader);
			_selectionShaderProgram->addShader(selectionFragmentShader);
			_selectionShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
			_selectionShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
			_selectionShaderProgram->link();
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ImageViewerWidget::resizeGL(int width, int height)
{
	qDebug() << "Resizing image viewer";

	glViewport(0, 0, width, height);

//	zoomExtents();
}

void ImageViewerWidget::paintGL() {

	glClearColor(0, 0, 0, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	double window = 0.0;
	double level = 0.0;

	computeWindowLevel(window, level);

	const auto minPixelValue = std::clamp(_imageMin, level - (window / 2.0), _imageMax);
	const auto maxPixelValue = std::clamp(_imageMin, level + (window / 2.0), _imageMax);

	QMatrix4x4 transform;

	transform.ortho(0.0f, +1.0f, _aspectRatio, 0.0f, -10.0f, +10.0f);

	if (_imageShaderProgram->isLinked()) {
		transform.translate(0.0f, 0.0f, 0.0f);

		_imageShaderProgram->bind();

		_imageShaderProgram->setUniformValue("imageTexture", 0);
		_imageShaderProgram->setUniformValue("minPixelValue", static_cast<GLfloat>(minPixelValue));
		_imageShaderProgram->setUniformValue("maxPixelValue", static_cast<GLfloat>(maxPixelValue));
		_imageShaderProgram->setUniformValue("matrix", transform);

		_imageShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_imageShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);

		_imageShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_imageShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

		if (_imageTexture.isCreated()) {
			_imageTexture.bind();

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_imageTexture.release();
		}
	}

	if (_selectionShaderProgram->isLinked()) {
		transform.translate(0.0f, 0.0f, 1.0f);

		_selectionShaderProgram->bind();

		_selectionShaderProgram->setUniformValue("selectionTexture", 0);
		_selectionShaderProgram->setUniformValue("matrix", transform);

		_selectionShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_selectionShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);

		_selectionShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_selectionShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

		if (_selectionTexture.isCreated()) {
			_selectionTexture.bind();

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_selectionTexture.release();
		}
	}
	*/
}

/*
QPoint ImageViewerWidget::screenToWorld(const QPoint& screen) const
{
	return QPoint(((screen.x()) / _zoom) - _pan.x(), -((-(screen.y() - height()) / _zoom) - _pan.y()));
}

QPoint ImageViewerWidget::worldToScreen(const QPoint& world) const
{
	return QPoint();
}

void ImageViewerWidget::updateSelection()
{
	//qDebug() << "Update selection" << _selectionType;
	
	const auto halfImageSize	= _imageSize / 2;
	const auto imageRect		= QRect(-halfImageSize.width(), -halfImageSize.height(), _imageSize.width(), _imageSize.height());

	auto overlayTextureData = TextureData();

	overlayTextureData.resize(_imageSize.width() * _imageSize.height());

	switch (_selectionType)
	{
		case SelectionType::Rectangle: {
			const auto initialMouseWorldPos = screenToWorld(QPoint(_initialMousePosition.x(), _initialMousePosition.y()));
			const auto currentMouseWorldPos = screenToWorld(QPoint(_mousePosition.x(), _mousePosition.y()));
			const auto selectionTopLeft		= QPoint(qMin(initialMouseWorldPos.x(), currentMouseWorldPos.x()), qMin(initialMouseWorldPos.y(), currentMouseWorldPos.y()));
			const auto selectionBottomRight = QPoint(qMax(initialMouseWorldPos.x(), currentMouseWorldPos.x()), qMax(initialMouseWorldPos.y(), currentMouseWorldPos.y()));
			const auto selectionRect		= QRect(selectionTopLeft, selectionBottomRight);
			
			if (imageRect.intersects(selectionRect)) {
				const auto imageSelection		= selectionRect.intersected(imageRect);
				const auto noSelectedPixels		= imageSelection.width() * imageSelection.height();
				
				auto selectedPointIds = Indices();

				selectedPointIds.reserve(noSelectedPixels);

				const auto left			= imageSelection.x() + halfImageSize.width();
				const auto right		= (imageSelection.x() + imageSelection.width()) + halfImageSize.width();
				const auto top			= imageSelection.y() + halfImageSize.height();
				const auto bottom		= (imageSelection.y() + imageSelection.height()) + halfImageSize.height();
				const auto pixelOffset	= _imageViewerPlugin->pixelOffset();

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
			const auto brushRadius			= _brushRadius / _zoom;
			const auto offset				= QPoint(qCeil(brushRadius), qCeil(brushRadius));
			const auto selectionRect		= QRect(currentMouseWorldPos - offset, currentMouseWorldPos + offset);

			if (imageRect.intersects(selectionRect)) {
				const auto imageSelection	= selectionRect.intersected(imageRect);
				const auto noSelectedPixels = imageSelection.width() * imageSelection.height();
				
				auto selectedPointIds = Indices();

				selectedPointIds.reserve(noSelectedPixels);

				const auto left			= imageSelection.x() + halfImageSize.width();
				const auto right		= (imageSelection.x() + imageSelection.width()) + halfImageSize.width();
				const auto top			= imageSelection.y() + halfImageSize.height();
				const auto bottom		= (imageSelection.y() + imageSelection.height()) + halfImageSize.height();
				const auto center		= currentMouseWorldPos - imageRect.topLeft() + QPointF(0.5f, 0.5f);
				const auto pixelOffset	= _imageViewerPlugin->pixelOffset();

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
}

void ImageViewerWidget::modifySelection(const Indices& selectedPointIds, const std::int32_t& pixelOffset)
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

	overlayTextureData.resize(_imageSize.width() * _imageSize.height());

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

void ImageViewerWidget::setupTextures()
{
	qDebug() << "Setup textures" << _imageSize;

	setupTexture(_textures["image"].get(), QOpenGLTexture::TextureFormat::R16_UNorm);
	setupTexture(_textures["overlay"].get(), QOpenGLTexture::TextureFormat::R16_UNorm);// , QOpenGLTexture::Filter::Nearest);
	setupTexture(_textures["selection"].get(), QOpenGLTexture::TextureFormat::R8U, QOpenGLTexture::Filter::Nearest);
}

void ImageViewerWidget::setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter)
{
	openGltexture->destroy();
	openGltexture->create();
	openGltexture->setSize(_imageSize.width(), _imageSize.height(), 1);
	openGltexture->setFormat(textureFormat);
	openGltexture->allocateStorage();
	openGltexture->setMinMagFilters(filter, filter);
}

void ImageViewerWidget::resetTexture(const QString & textureName)
{
	if (_textures[textureName]->isCreated()) {
		// resetTextureData(textureName);
		// applyTextureData(textureName);
	}
}
*/

InteractionMode ImageViewerWidget::interactionMode() const
{
	return _interactionMode;
}

void ImageViewerWidget::setInteractionMode(const InteractionMode& interactionMode)
{
	if (interactionMode == _interactionMode)
		return;

	_interactionMode = interactionMode;

	qDebug() << "Set interaction mode to" << interactionModeTypeName(_interactionMode);

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
			setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
			setCursor(Qt::ArrowCursor);
			break;

		default:
			break;
	}
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

	emit selectionTypeChanged(_selectionType);
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

		emit selectionModifierChanged(_selectionModifier);
	}
}

float ImageViewerWidget::brushRadius() const
{
	return _brushRadius;
}

void ImageViewerWidget::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(0.01f, 10000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	qDebug() << "Set brush radius" << brushRadius;

	_brushRadius = boundBrushRadius;
	
	emit brushRadiusChanged(_brushRadius);

	update();
}

void ImageViewerWidget::zoomToExtents()
{
//	fitInView(_imageWidget->rect(), Qt::KeepAspectRatio);
}

double ImageViewerWidget::window() const
{
	return _window;
}

void ImageViewerWidget::setWindowLevel(const double& window, const double& level)
{
	if (window == _window && level == _level)
		return;

	qDebug() << "Set window/level" << window << level;

	_window = window;
	_level	= level;

	emit windowLevelChanged(_window, _level);

	update();
}

void ImageViewerWidget::resetWindowLevel()
{
	_window = 1.0;
	_level = 0.5;

	update();
}

void ImageViewerWidget::makeObject()
{
	/*
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

	_vbo.create();
	_vbo.bind();
	_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
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
				{
					setSelectionType(SelectionType::Rectangle);
					break;
				}

				case Qt::Key::Key_B:
				{
					setSelectionType(SelectionType::Brush);
					break;
				}

				case Qt::Key::Key_F:
				{
					setSelectionType(SelectionType::Freehand);
					break;
				}

				case Qt::Key::Key_Shift:
				{
					//if (myGLWidget->selectionModifier() != SelectionModifier::Remove)
					setSelectionModifier(SelectionModifier::Add);
					break;
				}

				case Qt::Key::Key_Control:
				{
					//if (myGLWidget->selectionModifier() != SelectionModifier::Add)
					setSelectionModifier(SelectionModifier::Remove);
					break;
				}

				case Qt::Key::Key_Space:
				{
					setInteractionMode(InteractionMode::Navigation);
					break;
				}

				default:
					break;
			}
		}
	}

	QWidget::keyPressEvent(keyEvent);
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
			{
				setInteractionMode(InteractionMode::Selection);
				break;

			}
			default:
				break;
		}
	}

	QOpenGLWidget::keyReleaseEvent(keyEvent);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					//_panning = true;

					setCursor(Qt::ClosedHandCursor);

					mouseEvent->accept();
					break;
				}

				case InteractionMode::Selection:
				{
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

	_interactionStartMousePosition = mouseEvent->pos();

	QOpenGLWidget::mousePressEvent(mouseEvent);
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				//QPointF mouseDelta = mapToScene(mouseEvent->pos()) - mapToScene(_lastMousePosition);
				//pan(mouseDelta);

				//horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (mouseEvent->x() - _lastMousePosition.x()));
				//verticalScrollBar()->setValue(verticalScrollBar()->value() - (mouseEvent->y() - _lastMousePosition.y()));

				mouseEvent->accept();
			}

			break;
		}

		case InteractionMode::Selection:
			break;

		case InteractionMode::WindowLevel:
		{
			const auto deltaWindow	= (mouseEvent->pos().x() - _lastMousePosition.x()) / 200.0;
			const auto deltaLevel	= -(mouseEvent->pos().y() - _lastMousePosition.y()) / 200.0;
			const auto window		= std::max<double>(0, std::min<double>(_window + deltaWindow, 1.0f));
			const auto level		= std::max<double>(0, std::min<double>(_level + deltaLevel, 1.0f));

			setWindowLevel(window, level);

			break;
		}

		default:
			break;
	}

	_lastMousePosition = mouseEvent->pos();

	QOpenGLWidget::mouseMoveEvent(mouseEvent);
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					//_panning = false;

					setCursor(Qt::OpenHandCursor);

					mouseEvent->accept();
					break;
				}

				case InteractionMode::Selection:
				{
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

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			//wheelEvent->angleDelta().y() > 0 ? zoomIn() : zoomOut();
			break;
		}

		case InteractionMode::Selection:
			break;

		case InteractionMode::WindowLevel:
			break;

		default:
			break;
	}

	QOpenGLWidget::wheelEvent(wheelEvent);
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	/*
	enableSelection(false);
	resetTexture("overlay");
	*/
}

void ImageViewerWidget::onDisplayImageChanged(std::vector<std::uint16_t>& displayImage, const QSize& imageSize, const double& imageMin, const double& imageMax)
{
	qDebug() << "Display image changed";

	/*
	auto reset = false;

	if (imageSize != _imageWidget->size()) {
		reset = true;
	}

	_imageWidget->setDisplayImage(displayImage, imageSize, imageMin, imageMax);

	if (reset) {
		zoomToExtents();
		_imageWidget->resetWindowLevel();
	}
	*/
}

void ImageViewerWidget::onSelectionImageChanged(std::vector<std::uint8_t>& displayImage, const QSize& imageSize)
{
	/*
	if (!isValid())
		return;

	qDebug() << "On selection image changed";

	//textureData("selection") = selectionImage;

	//applyTextureData("selection");

	resetTexture("overlay");

	update();
	*/
}