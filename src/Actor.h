#pragma once

#include "Common.h"

#include <QObject>
#include <QColor>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

#include "Shape.h"

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
	/** Constructor
	 * @param renderer Renderer
	 * @param name Name of the actor
	 */
	Actor(Renderer* renderer, const QString& name);

	/** Destructor */
	~Actor();

	/**
	 * Performs a cleanup e.g. removes various OpenGL buffers
	 * Must be called in the correct OpenGL context (the context from which initializeGL() is called)
	 */
	virtual void destroy();

	/** Initialize the Actor (must be called in appropriate OpenGL context) */
	virtual void initialize();

	/** Renders the Actor */
	virtual void render();

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

	/** Returns the translation */
	QVector3D translation() const;

	/**
	 * Set translation
	 * @param translation Translation
	 */
	void setTranslation(const QVector3D& translation);

	/** Returns the model > view matrix */
	QMatrix4x4 modelViewMatrix() const;

	/** Returns the model > view > projection matrix */
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
	
	/** Returns whether the Actor is active e.g. interaction with the mouse */
	bool isActive() const;

	/** Activate the Actor */
	virtual void activate();

	/** Deactivate the Actor */
	virtual void deactivate();

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

	/**
	* Get shape by name
	* @param name Name of the shape
	*/
	template<typename T>
	T* shape(const QString& name)
	{
		return dynamic_cast<T*>(_shapes[name].get());
	}

	/**
	* Add shape by name
	* @param name Name of the shape
	*/
	template<typename T>
	void addShape(const QString& name)
	{
		auto shape = QSharedPointer<T>::create(this, name);

		_shapes.insert(name, shape);

		connect(shape.get(), &Shape::changed, [&](Shape* shape) {
			emit changed(this);
		});
	}

protected:	
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
	Renderer*								_renderer;					/** Pointer to renderer */
	QString									_name;						/** Name of the Actor */
	bool									_enabled;					/** Whether the Actor is enabled or not */
	bool									_active;					/** Actors is being interacted with */
	int										_registeredEvents;			/** Defines which (mouse) events should be received by the actor */
	float									_opacity;					/** Render opacity */
	QMatrix4x4								_modelMatrix;				/** Model matrix */
	QMap<QString, QSharedPointer<Shape>>	_shapes;					/** Shapes map */
};