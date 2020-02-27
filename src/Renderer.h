#pragma once

#include "renderers/Renderer.h"

#include "Actor.h"

#include <QColor>
#include <QWidget>

class ImageViewerWidget;
class ImageDatasetsModel;
class SelectionPickerActor;

class QMouseEvent;
class QWheelEvent;
class QOpenGLWidget;
class QMenu;
class QOpenGLWidget;

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
	 * @param parentWidget Parent image viewer widget
	 */
	Renderer(QOpenGLWidget* parent);

public:
	/** Initialize the renderer */
	void init() override;

	/** Resizes the renderer */
	void resize(QSize renderSize) override {};

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

	/** Convert point in screen coordinates to point in world coordinates
	 * @param modelViewMatrix Model-view matrix
	 * @param screenPoint Point in screen coordinates [0..width, 0..height]
	 * @return Position in world coordinates
	 */
	QVector3D screenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QVector2D& screenPoint) const;

	/** Convert position in world coordinates to point in normalized screen coordinates
	 * @param position Position in world coordinates
	 * @return Point in normalized screen coordinates [-1..1, -1..1]
	 */
	QVector2D worldPositionToNormalizedScreenPoint(const QVector3D& position) const;

	/** Convert position in world coordinates to point in screen coordinates
	 * @param position Position in world coordinates
	 * @return Point in screen coordinates [0..width, 0..height]
	 */
	QVector2D worldPositionToScreenPoint(const QVector3D& position) const;

	/** Convert point in screen coordinates to point in normalized screen coordinates
	 * @param screenPoint Point in screen coordinates [0..width, 0..height]
	 * @return Point in normalized screen coordinates [-1..1, -1..1]
	 */
	QVector2D screenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const;

	/** Returns the matrix that converts screen coordinates [0..width, 0..height] to normalized screen coordinates [-1..1, -1..1] */
	QMatrix4x4 screenToNormalizedScreenMatrix() const;

	/** Returns the matrix that converts normalized screen coordinates [-1..1, -1..1] to screen coordinates [0..width, 0..height] */
	QMatrix4x4 normalizedScreenToScreenMatrix() const;

	/** Returns the view matrix */
	QMatrix4x4 viewMatrix() const;

	/** Returns the projection matrix */
	QMatrix4x4 projectionMatrix() const;

	/**
	 * Move the view horizontally/vertically
	 * @param delta Amount to move
	 */
	void pan(const QVector2D& delta);

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
	void zoomAround(const QVector2D& screenPoint, const float& factor);

	/**
	 * Zoom to rectangle
	 * @param rectangle Rectangle to zoom to
	 */
	void zoomToRectangle(const QRectF& rectangle);

	/** Zoom to selected pixels */
	void zoomToSelection();

	/** Reset the view */
	void resetView();

	/** Returns the view menu */
	QMenu* viewMenu();

	/** Returns the context menu */
	QMenu* contextMenu();

	/** Whether the display of a context menu is allowed */
	bool allowsContextMenu();

	/** Adds a named color
	 * @param name Name of the color
	 * @param color The color
	 */
	void addNamedColor(const QString& name, const QColor& color);

	/** Returns a color by name
	 * @param name Name of the color
	 */
	QColor colorByName(const QString& name, const std::int32_t& alpha = -1) const;

public: // Parent widget queries

	/** Returns the parent widget */
	QOpenGLWidget* parentWidget() const;

	/** Returns the parent widget size */
	QSize parentWidgetSize() const;

public:
	/**
	 * Sets the color image
	 * @param colorImage Color image
	 */
	void setColorImage(QSharedPointer<QImage> colorImage);

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

protected: // Event handlers

	/**
	 * Invoked when an actor has changed
	 * @param actor Actor
	 */
	void onActorChanged(Actor* actor);
	
public: // Actors management

	/** TODO */
	template<typename T, typename ...Args>
	void addActor(Args... args)
	{
		try
		{
			auto actor = dynamic_cast<Actor*>(new T(args...));

			if (actor == nullptr)
				throw std::exception(QString("Supplied type is not an Actor").toLatin1());

			auto name = actor->name();

			if (_actors.contains(name))
				throw std::exception(QString("%2 already exists").arg(name).toLatin1());

			_actors.insert(name, actor);

			QObject::connect(actor, &Actor::becameDirty, this, &Renderer::becameDirty);
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to add actor: %1").arg(e.what()).toLatin1());
		}
	}

	/** TODO */
	void removeActor(const QString& name)
	{
		try
		{
			if (!_actors.contains(name))
				throw std::exception(QString("%2 does not exist").arg(name).toLatin1());

			_actors.remove(name);
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to remove actor: %1").arg(e.what()).toLatin1());
		}
	}

	/** Returns const pointer to actor by name */
	template<typename T>
	const T* actorByName(const QString& name) const
	{
		return dynamic_cast<T*>(_actors[name].get());
	}

	/** Returns pointer to actor by name */
	template<typename T>
	T* actorByName(const QString& name)
	{
		const auto constThis = const_cast<const Renderer*>(this);
		return const_cast<T*>(constThis->actorByName<T>(name));
	}

	/** TODO */
	void renderActors();

	/** TODO */
	void destroyActors();

signals:

	/** Signals that the renderer just became dirty */
	void becameDirty();

	/**
	 * Signals a key is pressed
	 * @param keyEvent Key event
	 */
	void keyPress(QKeyEvent* keyEvent);

	/**
	 * Signals a key is released
	 * @param keyEvent Key event
	 */
	void keyRelease(QKeyEvent* keyEvent);

	/**
	 * Signals the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePress(QMouseEvent* mouseEvent);

	/**
	 * Signals the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseRelease(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMove(QMouseEvent* mouseEvent);

	/**
	 * Signals the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void mouseWheel(QWheelEvent* wheelEvent);

protected:
	InteractionMode							_interactionMode;		/** Type of interaction e.g. navigation, selection and window/level */
	QVector<QSharedPointer<QMouseEvent>>	_mouseEvents;			/** Recorded mouse events during interaction */
	QVector2D								_pan;					/** Move view horizontally/vertically */
	float									_zoom;					/** Zoom view in/out */
	float									_zoomSensitivity;		/** Zoom sensitivity */
	int										_margin;				/** Margin between image and viewer widget boundaries */
	QMap<QString, QColor>					_colorMap;				/** Color map */

private:
	QMap<QString, Actor*>	_actors;				/** TODO */
};