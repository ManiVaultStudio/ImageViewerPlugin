#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <QMouseEvent>
#include <QColor>
#include <QAction>
#include <QMenu>

#include <memory>

#include "Common.h"

#include "ImageData/ImageData.h"

class ImageViewerPlugin;

class ImageViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);
	~ImageViewerWidget() override;

public:
	InteractionMode interactionMode() const;
	void setInteractionMode(const InteractionMode& interactionMode);
	SelectionType selectionType() const;
	void setSelectionType(const SelectionType& selectionType);
	SelectionModifier selectionModifier() const;
	void setSelectionModifier(const SelectionModifier& selectionModifier);
	void setBrushRadius(const float& brushRadius);
	std::pair<float, float> windowLevel() const;
	void setWindowLevel(const float& window, const float& level);
	void resetWindowLevel();

	void modifySelection();
	void clearSelection();

public:
	void onDisplayImageChanged(std::unique_ptr<QImage>& displayImage);
	void onSelectionImageChanged(std::unique_ptr<QImage>& selectionImage);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onCurrentImageIdChanged(const std::int32_t& currentImageId);

private:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

private:
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;

private:
	void drawCircle(const QPointF & center, const float & radius, const int & noSegments = 30);
	void drawSelectionRectangle(const QPoint& start, const QPoint& end);
	void drawSelectionBrush();
	void drawSelectionGeometry();

	void enableSelection(const bool& enable);

	void pan(const QPointF& delta);
	void zoom(const float& factor);
	void zoomAt(const QPointF & position, const float & factor);
	void zoomExtents();
	void resetView();

	bool initialized();
	void updateSelection();
	void commitSelection();

	QMenu* contextMenu();
	QMenu* viewMenu();
	QMenu* selectionMenu();

	QMatrix4x4 modelView() const;
	QMatrix4x4 projection() const;
	QVector3D screenToWorld(const QPoint& screen) const;

private:
	void createImageQuad();
	void setupTextures();
	void setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter = QOpenGLTexture::Filter::Linear);

signals:
	void selectionTypeChanged();
	void selectionModifierChanged();
	void brushRadiusChanged();

private:
	ImageViewerPlugin*							_imageViewerPlugin;
	std::unique_ptr<QImage>						_displayImage;
	std::unique_ptr<QImage>						_selectionImage;

	// QT OpenGL
	std::unique_ptr<QOpenGLTexture>				_imageTexture;
	std::unique_ptr<QOpenGLTexture>				_selectionTexture;
	std::unique_ptr<QOpenGLShader>				_vertexShader;
	std::unique_ptr<QOpenGLShader>				_imageFragmentShader;
	std::unique_ptr<QOpenGLShader>				_computeOverlayFragmentShader;
	std::unique_ptr<QOpenGLShader>				_overlayFragmentShader;
	std::unique_ptr<QOpenGLShader>				_selectionFragmentShader;
	std::unique_ptr<QOpenGLShaderProgram>		_imageShaderProgram;
	
	std::unique_ptr<QOpenGLShaderProgram>		_pixelSelectionShaderProgram;

	std::unique_ptr<QOpenGLShaderProgram>		_overlayShaderProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_selectionShaderProgram;
	std::unique_ptr<QOpenGLFramebufferObject>	_pixelSelectionFBO;
	QOpenGLBuffer								_imageQuadVBO;

	InteractionMode								_interactionMode;
	QPoint										_initialMousePosition;
	QPoint										_mousePosition;
	QPointF										_pan;
	float										_zoom;
	float										_zoomSensitivity;
	int											_margin;
	bool										_selecting;
	SelectionType								_selectionType;
	SelectionModifier							_selectionModifier;
	bool										_selectionRealtime;
	float										_brushRadius;
	float										_brushRadiusDelta;
	QColor										_pointSelectionColor;
	QColor										_pixelSelectionColor;
	QColor										_selectionGeometryColor;
	Indices										_selectedPointIds;
	QAction*									_zoomToExtentsAction;
	//QOpenGLFramebufferObject					_overlayFrameBufferObject;
	
	
	float										_window;
	float										_level;
	bool										_ignorePaintGL;
};
