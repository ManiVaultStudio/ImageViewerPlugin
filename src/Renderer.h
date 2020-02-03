#pragma once

#include "renderers/Renderer.h"
#include "ImageData/ImageData.h"

#include "Common.h"

#include "Actor.h"

class ColorImageQuad;
class SelectionBufferQuad;
class SelectionOutline;

class ImageViewerWidget;

class QMouseEvent;
class QWheelEvent;
class QOpenGLWidget;
class QMenu;

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
	 * @param parentWidget Parent OpenGL widget
	 */
	Renderer(QOpenGLWidget* parentWidget);

public:
	/** Initialize the renderer */
	void init() override;

	/** Resizes the renderer */
	void resize(QSize renderSize) override;

	/** Renders the content */
	void render() override;

	/** Destroys the renderer */
	void destroy() override;

	/** Returns mouse events that were recorded during interaction */
	QVector<QSharedPointer<QMouseEvent>> mouseEvents() const;

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

	/**
	 * Invoked when a key is pressed
	 * @param keyEvent Key event
	 */
	void keyPressEvent(QKeyEvent* keyEvent);

	/**
	 * Invoked when a key is released
	 * @param keyEvent Key event
	 */
	void keyReleaseEvent(QKeyEvent* keyEvent);

	/** Convert screen points to world point
	 * @param modelViewMatrix Model > view matrix
	 * @param screenPoint Point in screen coordinates
	*/
	QVector3D screenToWorld(const QMatrix4x4& modelViewMatrix, const QPointF& screenPoint) const;

	/** Returns the view matrix */
	QMatrix4x4 viewMatrix() const;

	/** Returns the projection matrix */
	QMatrix4x4 projectionMatrix() const;

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
	 * Zoom around screen point
	 * @param screenPoint Point in screen coordinates
	 * @param factor Factor to zoom by
	 */
	void zoomAround(const QPointF& screenPoint, const float& factor);

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

	/** Returns the context menu */
	QMenu* contextMenu();

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
	ColorImageQuad* imageQuad();

	/** Returns the selection buffer quad */
	SelectionBufferQuad* selectionBufferQuad();

	/** Returns the selection outline */
	SelectionOutline* selectionOutline();

	/**
	* Get actor by name
	* @param name Name of the actor
	*/
	template<typename T>
	T* actor(const QString& name)
	{
		return dynamic_cast<T*>(_actors[name].get());
	}

	/** Returns the interaction mode */
	InteractionMode interactionMode() const;

	/**
	 * Set interaction mode
	 * @param interactionMode Interaction mode
	 */
	void setInteractionMode(const InteractionMode& interactionMode);

	/** Binds the OpenGL context */
	void bindOpenGLContext();

	/** Releases the OpenGL context */
	void releaseOpenGLContext();

protected:
	/**
	 * Invoked when an actor has changed
	 * @param actor Actor
	 */
	void onActorChanged(Actor* actor);

private:
	/** Add an actor
	 * @param actor Shared pointer to actor
	*/
	void addActor(const QString& name, QSharedPointer<Actor> actor);

	/** Create shapes */
	void createActors();

	/** Initialize shapes */
	void initializeActors();

	/** Render shapes */
	void renderActors();

	/** Destroy shapes */
	void destroyActors();

signals:
	/** Signals that the renderer just became dirty (one or more shapes need to be re-rendered) */
	void dirty();

protected:
	QOpenGLWidget*							_parentWidget;			/** Pointer to parent widget */
	QMap<QString, QSharedPointer<Actor>>	_actors;				/** Actors map */
	InteractionMode							_interactionMode;		/** Type of interaction e.g. navigation, selection and window/level */
	QVector<QSharedPointer<QMouseEvent>>	_mouseEvents;			/** Recorded mouse events during interaction */
	QPointF									_pan;					/** Move view horizontally/vertically */
	float									_zoom;					/** Zoom view in/out */
	float									_zoomSensitivity;		/** Zoom sensitivity */
	int										_margin;				/** Margin between image and viewer widget boundaries */
};