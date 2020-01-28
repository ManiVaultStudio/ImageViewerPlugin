#pragma once

#include "renderers/Renderer.h"
#include "ImageData/ImageData.h"

#include "Common.h"

#include "Shape.h"

class ImageQuad;
class SelectionBufferQuad;
class SelectionOutline;

class ImageViewerWidget;

class QMouseEvent;
class QWheelEvent;

/**
 * Selection renderer class
 * @author Thomas Kroes
 */
class Renderer : public QObject, public hdps::Renderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param parent Parent widget
	 */
	Renderer(QWidget* parent);

public:
	/** Renders the content */
	void render() override;

	/** Resizes the renderer */
	void resize(QSize renderSize) override;

	/** Initialize the renderer */
	void init() override;

	/** Destroys the renderer */
	void destroy() override;

	/** Return whether the renderer is initialized */
	bool isInitialized() const;

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseReleaseEvent(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMoveEvent(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void mouseWheelEvent(QWheelEvent* wheelEvent);

	/** Convert screen coordinate to world coordinates
	 * @param modelViewMatrix Model > view matrix
	 * @param projectionMatrix Projection matrix
	 * @param screenPoint Point in screen coordinates
	*/
	QVector3D screenToWorld(const QMatrix4x4& modelViewMatrix, const QMatrix4x4& projectionMatrix, const QPointF& screenPoint) const;

	/** Returns the model view matrix */
	QMatrix4x4 modelView() const;

	/** Returns the projection matrix */
	QMatrix4x4 projection() const;

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
	 * @param position Position to zoom around in screen coordinates
	 * @param factor Factor to zoom by
	 */
	void zoomAround(const QPointF& position, const float& factor);

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

public:
	/**
	 * Sets the color image
	 * @param colorImage Color image
	 */
	void setColorImage(std::shared_ptr<QImage> colorImage);

	/**
	 * Sets the selection image
	 * @param selectionImage Selection image
	 */
	void setSelectionImage(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);

	/** Returns the selection opacity */
	float selectionOpacity();

	/**
	 * Sets the selection  opacity
	 * @param selectionOpacity Selection opacity
	 */
	void setSelectionOpacity(const float& selectionOpacity);

	/** Returns the image quad */
	ImageQuad* imageQuad();

	/** Returns the selection buffer quad */
	SelectionBufferQuad* selectionBufferQuad();

	/** Returns the selection outline */
	SelectionOutline* selectionOutline();

	/**
	 * Get shape by name
	 * @param name Name of the shape
	 */
	template<typename T>
	T* shape(const QString& name)
	{
		return dynamic_cast<T*>(_shapes[name].get());
	}

	/** Returns the interaction mode */
	InteractionMode interactionMode() const;

	/**
	 * Set interaction mode
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

	/** Returns the brush radius */
	float brushRadius() const;

	/**
	 * Sets the brush radius
	 * @param brushRadius Brush radius
	 */
	void setBrushRadius(const float& brushRadius);

	/** Returns the brush radius delta (amount to increasing/decreasing) */
	float brushRadiusDelta() const;

	/**
	 * Sets the brush radius delta (amount to increasing/decreasing)
	 * @param brushRadiusDelta Amount to add/remove
	 */
	void setBrushRadiusDelta(const float& brushRadiusDelta);

	/** Increase the brush size by _brushRadiusDelta */
	void brushSizeIncrease();

	/** Decrease the brush size by _brushRadiusDelta */
	void brushSizeDecrease();

protected:
	/**
	 * Invoked when a shape has changed
	 * shape Pointer to a shape
	 */
	void onShapeChanged(Shape* shape);

private:
	/** Add a shape
	 * @param shape Shared pointer to shape
	*/
	void addShape(const QString& name, QSharedPointer<Shape> shape);

	/** Create shapes */
	void createShapes();

	/** Initialize shapes */
	void initializeShapes();

	/** Render shapes */
	void renderShapes();

	/** Destroy shapes */
	void destroyShapes();

signals:
	/**
	 * Signals the selection type changed
	 * @param selectionType Selection type
	 */
	void selectionTypeChanged(const SelectionType& selectionType);

	/**
	 * Signals the selection modifier changed
	 * @param selectionModifier Selection modifier
	 */
	void selectionModifierChanged(const SelectionModifier& selectionModifier);

	/**
	 * Invoked when the brush radius changed
	 * @param brushRadius Brush radius
	 */
	void brushRadiusChanged(const float& brushRadius);

	/**
	 * Signals the brush radius delta changed
	 * @param brushRadiusDelta Brush radius delta
	 */
	void brushRadiusDeltaChanged(const float& brushRadiusDelta);

	/** Signals that the renderer just became dirty (one or more shapes need to be re-rendered) */
	void dirty();

protected:
	QWidget*								_parent;				/** Pointer to parent widget */
	QMap<QString, QSharedPointer<Shape>>	_shapes;				/** Shapes map */
	InteractionMode							_interactionMode;		/** Type of interaction e.g. navigation, selection and window/level */
	QVector<QSharedPointer<QMouseEvent>>	_mouseEvents;			/** Recorded mouse events during interaction */
	QPointF									_pan;					/** Move view horizontally/vertically */
	float									_zoom;					/** Zoom view in/out */
	float									_zoomSensitivity;		/** Zoom sensitivity */
	int										_margin;				/** Margin between image and viewer widget boundaries */
	SelectionType							_selectionType;			/** Type of selection e.g. rectangle, brush */
	SelectionModifier						_selectionModifier;		/** The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	float									_brushRadius;			/** Brush radius */
	float									_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
};