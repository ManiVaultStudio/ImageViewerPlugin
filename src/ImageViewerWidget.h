#pragma once

#include "Common.h"

#include "ImageQuadRenderer.h"
#include "SelectionRenderer.h"

#include "ImageData/ImageData.h"

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QColor>
#include <QRect>
#include <QOpenGLDebugLogger>

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
	QPoint mousePosition() const;
	std::vector<QPoint> mousePositions() const;
	bool selecting() const;

	void publishSelection();

	void selectAll();
	void selectNone();
	void invertSelection();

	std::shared_ptr<ImageQuadRenderer> imageQuadRenderer();
	std::shared_ptr<SelectionRenderer> selectionRenderer();

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
	float zoom() const;
	void zoomBy(const float& factor);
	void zoomAt(const QPointF& position, const float& factor);
	void zoomExtents();
	void zoomToRectangle(const QRectF& rectangle);
	void zoomToSelection();
	void resetView();

private:
	QMenu* contextMenu();
	QMenu* viewMenu();
	QMenu* selectionMenu();

public:
	QMatrix4x4 modelView() const;
	QMatrix4x4 projection() const;
	QVector3D screenToWorld(const QPointF& screen) const;

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/*! Pointer to image viewer plugin */
	std::shared_ptr<ImageQuadRenderer>		_imageQuadRenderer;			/*! Image quad renderer */
	std::shared_ptr<SelectionRenderer>		_selectionRenderer;			/*! Selection renderer */
	InteractionMode							_interactionMode;			/*! Type of interaction e.g. navigation, selection and window/level */
	QPoint									_mousePosition;				/*! Real-time mouse position */
	std::vector<QPoint>						_mousePositions;			/*! All recorded mouse positions during interaction event */
	QPointF									_pan;						/*! Move view horizontally/vertically */
	float									_zoom;						/*! Zoom view in/out */
	float									_zoomSensitivity;			/*! Zoom sensitivity */
	int										_margin;					/*! Margin between image and viewer widget boundaries */
	bool									_selecting;					/*! Whether selection is taking place */
	SelectionType							_selectionType;				/*! Type of selection e.g. rectangle, brush and polygon */
	SelectionModifier						_selectionModifier;			/*! The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	QVector4D								_pixelSelectionColor;		/*! The color of selected pixels (data points) */
	QVector4D								_selectionOutlineColor;		/*! Color of outlines e.g. selection geometry/bounds */	
	std::unique_ptr <QOpenGLDebugLogger>	_openglDebugLogger;			/*! OpenGL debug logger (on in debug mode) */
};