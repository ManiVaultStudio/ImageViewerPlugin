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
	void setBrushRadiusDelta(const float& brushRadiusDelta);
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
	void drawSelectionOutlineRectangle(const QPoint& start, const QPoint& end);
	void drawSelectionOutlineBrush();
	void drawSelectionOutlineLasso();
	void drawSelectionOutline();
	void drawSelectionBounds();

	void enableSelection(const bool& enable);

	void pan(const QPointF& delta);
	void zoom(const float& factor);
	void zoomAt(const QPointF & position, const float & factor);
	void zoomExtents();
	void resetView();

	std::uint16_t windowLevel(const float& min, const float& max, const float& levelNorm, const float& windowNorm, const float& pointValue);

	bool initialized();
	void updatePixelSelection();
	void resetPixelSelection();
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

private:
	ImageViewerPlugin*							_imageViewerPlugin;
	std::unique_ptr<QImage>						_displayImage;
	std::unique_ptr<QImage>						_selectionImage;

	// QT OpenGL
	std::unique_ptr<QOpenGLTexture>				_imageTexture;
	std::unique_ptr<QOpenGLTexture>				_selectionTexture;

	std::unique_ptr<QOpenGLShaderProgram>		_imageShaderProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_pixelSelectionShaderProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_overlayShaderProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_selectionShaderProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_selectionGeometryShaderProgram;
	std::unique_ptr<QOpenGLShaderProgram>		_selectionBoundsShaderProgram;

	std::unique_ptr<QOpenGLFramebufferObject>	_pixelSelectionFBO;

	QOpenGLBuffer								_imageQuadVBO;

	InteractionMode								_interactionMode;
	QPoint										_initialMousePosition;
	QPoint										_mousePosition;
	std::vector<QPoint>							_mousePositions;
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
	QVector4D									_pointSelectionColor;
	QVector4D									_pixelSelectionColor;
	QColor										_selectionOutlineColor;
	QVector4D									_selectionBoundsColor;
	std::vector<std::uint32_t>					_selectedPointIds;
	std::uint32_t								_selectionBounds[4];
	std::uint32_t								_noSelectedPixels;

	std::uint16_t								_imageMin;
	std::uint16_t								_imageMax;
	float										_window;
	float										_level;
	bool										_ignorePaintGL;
};
