#pragma once

#include "Node.h"

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

/**
 * Render node class
 *
 * Contains props for rendering in OpenGL
 *
 * @author Thomas Kroes
 */
class RenderNode : public Node
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
	/**
	 * Constructor
	 * @param id Identifier for internal use
	 * @param name Name in the GUI
	 * @param flags Configuration bit flags
	 */
	RenderNode(const QString& id, const QString& name, const int& flags);

	/** Destructor */
	~RenderNode();

	/** Renders the node */
	virtual void render();

public: // Matrix functions

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

protected: // Key/mouse handlers

	/** Node will register mouse press events */
	void registerMousePressEvents();

	/** Node will register mouse release events */
	void registerMouseReleaseEvents();

	/** Node will register mouse move press events */
	void registerMouseMoveEvents();

	/** Node will register mouse wheel events */
	void registerMouseWheelEvents();

	/** Node will register key press events */
	void registerKeyPressEvents();

	/** Node will register key release events */
	void registerKeyReleaseEvents();

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

	/**
	 * Records a mouse event
	 * @param mouseEvent Mouse event
	 */
	void addMouseEvent(QMouseEvent* mouseEvent);

	/** Returns the recorded mouse events */
	QVector<MouseEvent> mouseEvents();

public: // Opacity

	/** Returns the render opacity */
	QVariant opacity(const int& role) const;

	/** Sets the render opacity
	 * @param opacity Render opacity
	*/
	void setOpacity(const float& opacity);

protected: // Prop management

	/** TODO */
	template<typename T, typename ...Args>
	void addProp(Args... args)
	{
		try {
			auto sharedProp = QSharedPointer<T>::create(args...);
			auto prop = dynamic_cast<Prop*>(sharedProp.get());
			auto propName = prop->name();

			if (_props.contains(propName))
				throw std::exception(QString("%1 already has a prop named %2").arg(_name, propName).toLatin1());

			_props.insert(propName, sharedProp);

			QObject::connect(prop, &Prop::becameDirty, [this](Prop* prop) {
				emit becameDirty(this);
			});
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to add prop to %1: %2").arg(_name, e.what()).toLatin1());
		}
	}

	/** TODO */
	void removeProp(const QString& name)
	{
		try {
			if (!_props.contains(name))
				throw std::exception(QString("%1 does not have a prop named %2").arg(_name, name).toLatin1());

			_props.remove(name);
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to remove prop from %1: %2").arg(_name, e.what()).toLatin1());
		}
	}

	/** TODO */
	template<typename T>
	const T* propByName(const QString& name) const
	{
		try {
			if (!_props.contains(name))
				throw std::exception(QString("%1 has no prop named %2").arg(_name, name).toLatin1());

			return dynamic_cast<T*>(_props[name].get());
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to retrieve prop by name from %1: %2").arg(_name, e.what()).toLatin1());
		}
	}

	/** TODO */
	template<typename T>
	T* propByName(const QString& name)
	{
		const auto constThis = const_cast<const Actor*>(this);
		return const_cast<T*>(constThis->propByName<T>(name));
	}
	
	/** TODO */
	const QMap<QString, SharedProp> props() const
	{
		return _props;
	}

public:
	static Renderer* renderer;

protected:
	int							_registeredEvents;		/** Defines which (mouse) events should be received by the actor */
	QVector<MouseEvent>			_mouseEvents;			/** Recorded mouse events */

private:
	float						_opacity;			/** Render opacity */
	QMatrix4x4					_modelMatrix;		/** Model matrix */
	QMap<QString, SharedProp>	_props;				/** Props map */
};