#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
#include <QColor>
#include <QRect>
#include <QOpenGLDebugLogger>

#include <memory>

#include "ImageData/ImageData.h"

#include "Common.h"

#include "ImageQuadRenderer.h"
#include "SelectionRenderer.h"
#include "SelectionBoundsRenderer.h"

class QMenu;

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

	void publishSelection();

	void selectAll();
	void selectNone();
	void invertSelection();

public:
	void onDisplayImageChanged(std::shared_ptr<QImage> displayImage);
	void onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);
	void onSelectionOpacityChanged(const float& selectionOpacity);
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
	void drawSelectionOutlinePolygon();
	void drawSelectionOutline();
	void drawSelectionBounds();

public:
	void startMouseInteraction();
	void endMouseInteraction();
	void startSelectionMode(const SelectionType& selectionType);
	void endSelectionMode();
	void startSelection();
	void endSelection();
	void startNavigationMode();
	void endNavigationMode();
	void startWindowLevelMode();
	void endWindowLevelMode();
	
public:
	void pan(const QPointF& delta);
	void zoom(const float& factor);
	void zoomAt(const QPointF & position, const float & factor);
	void zoomExtents();
	void zoomToRectangle(const QRectF& rectangle);
	void zoomToSelection();
	void resetView();

	bool initialized();
	void updatePixelSelection();
	void resetPixelSelection();

protected:
	QMenu* contextMenu();
	QMenu* viewMenu();
	QMenu* selectionMenu();

protected:
	QMatrix4x4 modelView() const;
	QMatrix4x4 projection() const;
	QVector3D screenToWorld(const QPoint& screen) const;

private:
	ImageViewerPlugin*							_imageViewerPlugin;
	std::unique_ptr<ImageQuadRenderer>			_imageQuadRenderer;
	std::unique_ptr<SelectionRenderer>			_selectionRenderer;
	std::unique_ptr<SelectionBoundsRenderer>	_selectionBoundsRenderer;
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
	float										_brushRadius;
	float										_brushRadiusDelta;
	QVector4D									_pixelSelectionColor;
	QVector4D									_selectionOutlineColor;

	bool										_ignorePaintGL;
	std::unique_ptr < QOpenGLDebugLogger>		_openglDebugLogger;
};