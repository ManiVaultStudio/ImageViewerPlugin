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

const std::string imageQuadVertexShaderSource =
#include "ImageQuadVertex.glsl"
;

const std::string imageQuadFragmentShaderSource =
#include "ImageQuadFragment.glsl"
;



/*
static const char* imageFragmentShaderSource =
"uniform sampler2D image;\n"
"uniform float minPixelValue;\n"
"uniform float maxPixelValue;\n"
"void main() {\n"
"	float value		= texture2D(image, gl_TexCoord[0].st).r * 65535.0;"
"	float fraction	= value - minPixelValue;\n"
"	float range		= maxPixelValue - minPixelValue;\n"
"	float clamped	= clamp(fraction / range, 0.0, 1.0);\n"
"   gl_FragColor	= vec4(clamped, clamped, clamped, 1.0);\n"
"}\n";
*/
/*
static const char* overlayFragmentShader =
"uniform sampler2D texture;\n"
"void main() {\n"
"	float value		= texture2D(texture, gl_TexCoord[0].st).r * 255.0\n;"
"   gl_FragColor	= value > 0 ? vec4(1) : vec4(0);\n"
"}\n";

static const char* selectionFragmentShader =
"uniform sampler2D overlay;\n"
"uniform vec4 selectionColor;\n"
"void main() {\n"
"	float value		= texture2D(overlay, gl_TexCoord[0].st).r * 255.0;"
"   gl_FragColor	= value > 0 ? selectionColor : vec4(0);\n"
"}\n";


#version 420 core

uniform sampler2D image;
uniform float minPixelValue;
uniform float maxPixelValue;

in vec2 vertexUV;
out vec4 fragmentColor;

void main() {
	float value = texture(image, vertexUV).r * 65535.0;
	float fraction = value - minPixelValue;
	float range = maxPixelValue - minPixelValue;
	float clamped = clamp(fraction / range, 0.0, 1.0);
	fragmentColor = vec4(clamped, clamped, clamped, 1.0);
};
*/

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

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageViewerWidget::onSelectionImageChanged);
	
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




void ImageViewerWidget::onSelectionImageChanged(const QSize& imageSize, TextureData& selectionImage)
{
	if (!isValid())
		return;

	qDebug() << "On selection image changed";

	//textureData("selection") = selectionImage;

	//applyTextureData("selection");

	resetTexture("overlay");

	update();
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	enableSelection(false);

	resetTexture("overlay");
}

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
	/*
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
	*/
}

void ImageViewerWidget::drawInfo(QPainter* painter)
{
	QFont font = painter->font();
	font.setFamily("courier");
	font.setPixelSize(16);
	
	painter->setFont(font);

	const QRect rectangle = QRect(0, 0, 1000, 500);
	QRect boundingRect;

	auto infoLines = QStringList();

	infoLines << "ALT: Navigation mode";

	if (QGuiApplication::queryKeyboardModifiers().testFlag(Qt::AltModifier))
	{
		infoLines << "ALT + LMB: Pan";
		infoLines << "ALT + MWL: Zoom";
	}
	else {
		infoLines << "R: Rectangle selection";
		infoLines << "B: Brush selection";
	}

	painter->setPen(Qt::white);
	painter->drawText(rectangle, 0, infoLines.join("\n"), &boundingRect);
}

void ImageViewerWidget::enableSelection(const bool& enable)
{
	//_selecting = enable;

	update();
}

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	_shaders["image"]->addShaderFromSourceCode(QOpenGLShader::Vertex, imageQuadVertexShaderSource.c_str());
	_shaders["image"]->addShaderFromSourceCode(QOpenGLShader::Fragment, imageQuadFragmentShaderSource.c_str());
	
	_shaders["image"]->link();
	/*
	const auto scale = 1000.f;

	float points[] = { -scale, -scale, 0.0f, 1.0f,
		scale, -scale, 0.0f, 1.0f,
		scale, scale, 0.0f, 1.0f,
		-scale,scale, 0.0f, 1.0f
	};

	_vertexBuffer.create();
	_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

	if (!_vertexBuffer.bind())
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}

	_vertexBuffer.allocate(points, 4 * 4 * sizeof(float));

	//_shaders["image"]->setAttributeBuffer("vertex", GL_FLOAT, 0, 4);
	//_shaders["image"]->enableAttributeArray("vertex");

	

	
	_shaders["overlay"]->addShaderFromSourceCode(QOpenGLShader::Fragment, overlayFragmentShader);
	_shaders["overlay"]->link();

	_shaders["selection"]->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShader);
	_shaders["selection"]->link();
	*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";

	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -100, 100);

//	zoomExtents();
}

void ImageViewerWidget::paintGL() {

	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!imageInitialized())
		return;

	qDebug() << "Paint OpenGL";

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	glScalef(_zoom, _zoom, 1);
	glTranslatef(_pan.x(), _pan.y(), 0);
	
	glColor4f(1.f, 1.f, 1.f, 1.f);

	glEnable(GL_TEXTURE_2D);

	/*
	double window	= 0.0;
	double level	= 0.0;

	computeWindowLevel(window, level);

	const auto minPixelValue = std::clamp(_imageViewerPlugin->imageMin(), level - (window / 2.0), _imageViewerPlugin->imageMax());
	const auto maxPixelValue = std::clamp(_imageViewerPlugin->imageMin(), level + (window / 2.0), _imageViewerPlugin->imageMax());
	*/

	//qDebug() << "======" << window << level << _imageViewerPlugin->imageMin() << _imageViewerPlugin->imageMax() << minPixelValue << maxPixelValue;

	
	_shaders["image"]->bind();
	//_textures["image"]->bind();

	//qDebug() << "Bound texture ID" << _textures["image"]->boundTextureId(QOpenGLTexture::BindingTarget2D);
	//qDebug() << "Texture ID" << _textures["image"]->textureId();

	_shaders["image"]->setUniformValue("image", 0);
	//_shaders["image"]->setUniformValue("minPixelValue", static_cast<GLfloat>(minPixelValue));
	//_shaders["image"]->setUniformValue("maxPixelValue", static_cast<GLfloat>(maxPixelValue));

	const auto scale = 10.f;

	float points[] = { -scale, -scale, 0.0f,
		-scale, scale, 0.0f,
		scale, -scale, 0.0f,
		scale,scale, 0.0f
	};

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);

	//drawQuad(1.0f);
	//glVertexPointer(4, GL_FLOAT, 0, points);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//_textures["image"]->release();
	_shaders["image"]->release();

	
	/*
	if (_imageViewerPlugin->selectable()) {
		if (_shaders["overlay"]->bind()) {
			_textures["overlay"]->bind();

			//qDebug() << _textures["overlay"]->textureId();

			_shaders["overlay"]->setUniformValue("texture", 0);
			//_shaders["overlay"]->setUniformValue("selectionColor", 1, 0, 0, 1);

			drawQuad(0.5f);

			_textures["overlay"]->release();
			_shaders["overlay"]->release();
		}
	}
	*/


	/*
	drawTextureQuad(texture("overlay"), 0.5f);
	drawTextureQuad(texture("selection"), 0.0f);
	}
	*/

	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	if (_interactionMode == InteractionMode::Selection) {
		drawSelectionGeometry();
	}
	*/
}




bool ImageViewerWidget::imageInitialized()
{
	return _textures["image"]->isCreated();
}

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
	/*
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
	*/
}

void ImageViewerWidget::modifySelection(const Indices& selectedPointIds, const std::int32_t& pixelOffset /*= 0*/)
{
	/*
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
	*/
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

void ImageViewerWidget::setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter /*= QOpenGLTexture::Filter::Linear*/)
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