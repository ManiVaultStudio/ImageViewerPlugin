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

/**
 * Image viewer widget class
 * This widget displays high-dimensional image data
 */
class ImageViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to image viewer plugin
	 */
	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~ImageViewerWidget() override;

public:
	/** Returns the interaction mode */
	InteractionMode interactionMode() const;

	/**
	 * Sets the interaction mode
	 * @param interactionMode Interaction mode
	 */
	void setInteractionMode(const InteractionMode& interactionMode);

	/** Returns the selection type */
	SelectionType selectionType() const;

	/**
	 * Sets the selection type
	 * @param selectionType Selection type
	 */
	void setSelectionType(const SelectionType& selectionType);

	/** Returns the selection modifier */
	SelectionModifier selectionModifier() const;

	/**
	 * Sets the selection modifier
	 * @param selectionModifier Selection modifier
	 */
	void setSelectionModifier(const SelectionModifier& selectionModifier);

	/** Returns the current mouse position */
	QPoint mousePosition() const;

	/** Returns the mouse positions that were recorded during an interaction event */
	std::vector<QPoint> mousePositions() const;

	/** Whether the user is currently selecting */
	bool selecting() const;

	/** Publish selection to HDPS */
	void publishSelection();

	/** Select all */
	void selectAll();

	/** Select none */
	void selectNone();

	/** Invert the selection */
	void invertSelection();

	/** Returns the image quad renderer */
	std::shared_ptr<ImageQuadRenderer> imageQuadRenderer();

	/** Returns the selection renderer */
	std::shared_ptr<SelectionRenderer> selectionRenderer();

public:
	/**
	 * Invoked when the display image changes
	 * @param displayImage Display image
	 */
	void onDisplayImageChanged(std::shared_ptr<QImage> displayImage);

	/**
	 * Invoked when the selection image changes
	 * @param selectionImage Selection image
	 * @param selectionBounds Bounds of the selected pixels in the selection image
	 */
	void onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);

	/**
	 * Invoked when the selection opacity changes
	 * @param selectionOpacity Selection opacity
	 */
	void onSelectionOpacityChanged(const float& selectionOpacity);

	/**
	 * Invoked when the current dataset changes
	 * @param currentDataset Current dataset name
	 */
	void onCurrentDatasetChanged(const QString& currentDataset);

	/**
	 * Invoked when the current image index changes
	 * @param currentImageId Current image index
	 */
	void onCurrentImageIdChanged(const std::int32_t& currentImageId);
	
private:
	/** Initializes OpenGL */
	void initializeGL() Q_DECL_OVERRIDE;

	/** Invoked when the OpenGL window changes size */
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;

	/** Paints the OpenGL content */
	void paintGL() Q_DECL_OVERRIDE;

private:
	/**
	 * Invoked when a key is pressed
	 * @param keyEvent Key event
	 */
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when a key is released
	 * @param keyEvent Key event
	 */
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;

public:
	/** Starts mouse interaction */
	void startMouseInteraction();

	/** Ends mouse interaction */
	void endMouseInteraction();

	/**
	 * Start selection mode
	 * @param selectionType Selection type
	 */
	void startSelectionMode(const SelectionType& selectionType);

	/** Ens selection mode*/
	void endSelectionMode();

	/** Start selection */
	void startSelection();

	/** End selection */
	void endSelection();

	/** Start navigating */
	void startNavigationMode();

	/** End navigating */
	void endNavigationMode();

	/** Start window/level interaction mode */
	void startWindowLevelMode();

	/** End window/level interaction mode */
	void endWindowLevelMode();
	
public:
	/**
	 * Move the view horizontally/vertically
	 * @param delta Amount to move
	 */
	void pan(const QPointF& delta);

	/** Return the current zoom level */
	float zoom() const;

	/**
	 * Zoom the view
	 * @param factor Factor to zoom by
	 */
	void zoomBy(const float& factor);

	/**
	 * Zoom at position
	 * @param position Position to zoom around
	 * @param factor Factor to zoom by
	 */
	void zoomAt(const QPointF& position, const float& factor);

	/** Zoom to extents of the rendered image quad */
	void zoomExtents();

	/**
	 * Zoom to rectangle
	 * @param rectangle Rectangle to zoom to
	 */
	void zoomToRectangle(const QRectF& rectangle);

	/** Zoom to selected pixels */
	void zoomToSelection();

	/** Reset the view */
	void resetView();

private:
	/** Returns the context menu */
	QMenu* contextMenu();

	/** Returns the view menu */
	QMenu* viewMenu();

	/** Returns the selection menu */
	QMenu* selectionMenu();

public:
	/** Returns the model view matrix */
	QMatrix4x4 modelView() const;

	/** Returns the projection matrix */
	QMatrix4x4 projection() const;

	/** Convert screen coordinate to world coordinate */
	QVector3D screenToWorld(const QPointF& screen) const;

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/** Pointer to image viewer plugin */
	std::shared_ptr<ImageQuadRenderer>		_imageQuadRenderer;			/** Image quad renderer */
	std::shared_ptr<SelectionRenderer>		_selectionRenderer;			/** Selection renderer */
	InteractionMode							_interactionMode;			/** Type of interaction e.g. navigation, selection and window/level */
	QPoint									_mousePosition;				/** Real-time mouse position */
	std::vector<QPoint>						_mousePositions;			/** All recorded mouse positions during interaction event */
	QPointF									_pan;						/** Move view horizontally/vertically */
	float									_zoom;						/** Zoom view in/out */
	float									_zoomSensitivity;			/** Zoom sensitivity */
	int										_margin;					/** Margin between image and viewer widget boundaries */
	bool									_selecting;					/** Whether selection is taking place */
	SelectionType							_selectionType;				/** Type of selection e.g. rectangle, brush and polygon */
	SelectionModifier						_selectionModifier;			/** The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	QVector4D								_pixelSelectionColor;		/** The color of selected pixels (data points) */
	std::unique_ptr <QOpenGLDebugLogger>	_openglDebugLogger;			/** OpenGL debug logger (on in debug mode) */
};