#pragma once

#include "Common.h"

#include <QObject>
#include <QColor>
#include <QVector2D>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

#include "Prop.h"

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class Renderer;

/**
 * Actor class
 * @author Thomas Kroes
 */
class Actor : public QObject
{
	Q_OBJECT

public:
	/**
	 * Mouse point class
	 * @author Thomas Kroes
	 */
	class MouseEvent
	{
	public:
		/** Default constructor */
		MouseEvent() {};

		/**
		 * Constructor
		 * @param screenPoint Point in screen coordinates [0..width, 0..height]
		 * @param worldPosition Position in world space
		 */
		MouseEvent(const QVector2D& screenPoint, const QVector3D& worldPosition = QVector3D()) :
			_screenPoint(screenPoint),
			_worldPosition(worldPosition)
		{
		}

		QVector2D screenPoint() const {
			return _screenPoint;
		}

		QVector3D worldPosition() const {
			return _worldPosition;
		}

	private:
		QVector2D		_screenPoint;		/** Point in screen coordinates */
		QVector3D		_worldPosition;		/** World position */
	};

public:
	/** Constructor
	 * @param renderer Renderer
	 * @param name Name of the actor
	 */
	Actor(Renderer* renderer, const QString& name);

	/** Destructor */
	~Actor();

	/** Returns the Actor name */
	QString name() const;

	/**
	 * Sets the name
	 * @param name Actor name
	 */
	void setName(const QString& name);

	/** Returns whether the Actor is enabled (visible) */
	bool isEnabled() const;

	/**
	 * Sets whether the Actor is enabled or not (visible)
	 * @param enabled Whether the Actor is enabled or not
	 */
	void setEnabled(const bool& enabled);

	/** Enables the Actor */
	void enable();

	/** Disables the Actor */
	void disable();

	/** Returns whether the Actor can be rendered */
	virtual bool canRender() const;
	
	/** Returns the model matrix */
	QMatrix4x4 modelMatrix() const;

	/**
	 * Sets the model matrix
	 * @param modelMatrix Model matrix
	 */
	void setModelMatrix(const QMatrix4x4& modelMatrix);

	/** Returns the model-view matrix (viewMatrix * actorModelMatrix) */
	QMatrix4x4 modelViewMatrix() const;

	/** Returns the model-view-projection matrix (projectionMatrix * viewMatrix * actorModelMatrix) */
	QMatrix4x4 modelViewProjectionMatrix() const;

	/** Invoked when a mouse button is pressed */
	virtual void onMousePressEvent(QMouseEvent* mouseEvent);

	/** Invoked when a mouse button is released */
	virtual void onMouseReleaseEvent(QMouseEvent* mouseEvent);

	/** Invoked when the mouse pointer is moved */
	virtual void onMouseMoveEvent(QMouseEvent* mouseEvent);

	/** Invoked when the mouse wheel is rotated */
	virtual void onMouseWheelEvent(QWheelEvent* wheelEvent);

	/** Invoked when a key is pressed */
	virtual void onKeyPressEvent(QKeyEvent* keyEvent);

	/** Invoked when a key is released */
	virtual void onKeyReleaseEvent(QKeyEvent* keyEvent);

	/** Returns whether this actor should receive mouse press events */
	bool shouldReceiveMousePressEvents() const;

	/** Returns whether this actor should receive mouse release events */
	bool shouldReceiveMouseReleaseEvents() const;

	/** Returns whether this actor should receive mouse move events */
	bool shouldReceiveMouseMoveEvents() const;

	/** Returns whether this actor should receive mouse wheel events */
	bool shouldReceiveMouseWheelEvents() const;

	/** Returns whether this actor should receive key press events */
	bool shouldReceiveKeyPressEvents() const;

	/** Returns whether this actor should receive key release events */
	bool shouldReceiveKeyReleaseEvents() const;
	
	/** Returns whether the actor is visible */
	bool isVisible() const;

	/** Show the actor */
	virtual void show();

	/** Hide the actor */
	virtual void hide();

	/** Set visible
	 * @param visible Visible
	 */
	virtual void setVisible(const bool& visible);

	/** Returns the render opacity */
	float opacity() const;

	/** Sets the render opacity
	 * @param opacity Render opacity
	*/
	void setOpacity(const float& opacity);

	/** Binds the OpenGL context */
	void bindOpenGLContext();

	/** Releases the OpenGL context */
	void releaseOpenGLContext();

	Renderer* renderer();

	/** Returns const pointer to prop by name */
	template<typename T>
	const T* propByName(const QString& name) const
	{
		return dynamic_cast<T*>(_props[name].get());
	}

	/** Returns pointer to prop by name */
	template<typename T>
	T* propByName(const QString& name)
	{
		const auto constThis = const_cast<const Actor*>(this);
		return const_cast<T*>(constThis->propByName<T>(name));
	}
	
	/**
	* Add prop by name
	* @param name Name of the prop
	*/
	template<typename T>
	void addProp(const QString& name)
	{
		auto prop = QSharedPointer<T>::create(this, name);

		_props.insert(name, prop);

		connect(prop.get(), &Prop::changed, [&](Prop* shape) {
			emit changed(this);
		});
	}

	/** Returns the recorded mouse events */
	QVector<MouseEvent> mouseEvents();

protected:
	/** Destroys the actor */
	virtual void destroy();

	/** Initializes the actor */
	virtual void initialize();

	/** Renders the actor */
	virtual void render();

	/** Returns whether this actor may process mouse press events */
	bool mayProcessMousePressEvent() const;

	/** Returns whether this actor may process mouse release events */
	bool mayProcessMouseReleaseEvent() const;

	/** Returns whether this actor may process mouse move events */
	bool mayProcessMouseMoveEvent() const;

	/** Returns whether this actor may process mouse wheel events */
	bool mayProcessMouseWheelEvent() const;

	/** Returns whether this actor may process key press events */
	bool mayProcessKeyPressEvent() const;

	/** Returns whether this actor may process key release events */
	bool mayProcessKeyReleaseEvent() const;

	/**
	 * Records a mouse event
	 * @param mouseEvent Mouse event
	 */
	void addMouseEvent(QMouseEvent* mouseEvent);

signals:
	/** Signals that the Actor name changed
	 * @param name New Actor name
	 */
	void nameChanged(const QString& name);

	/** Signals that the Actor has been enabled or disabled
	 * @param enabled Whether the Actor is enabled or not
	 */
	void enabledChanged(const bool& enabled);

	/** Signals that the opacity changed
	 * @param opacity Opacity
	 */
	void opacityChanged(const float& opacity);

	/** Signals that the model matrix changed
	 * @param modelMatrix Model matrix
	 */
	void modelMatrixChanged(const QMatrix4x4& modelMatrix);

	/** Signals that the Actor changed */
	void changed(Actor* Actor);

protected:
	Renderer*								_renderer;				/** Pointer to renderer */
	int										_registeredEvents;		/** Defines which (mouse) events should be received by the actor */
	QVector<MouseEvent>						_mouseEvents;			/** Recorded mouse events */

private:
	QString									_name;					/** Name of the Actor */
	bool									_enabled;				/** Whether the Actor is enabled or not */
	bool									_visible;				/** Actors is being interacted with */
	float									_opacity;				/** Render opacity */
	QMatrix4x4								_modelMatrix;			/** Model matrix */
	QMap<QString, QSharedPointer<Prop>>		_props;					/** Props map */

	friend class Renderer;
};