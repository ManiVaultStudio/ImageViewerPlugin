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

	/**
	 * Determines whether the Actor is properly initialized (all buffers etc. are setup correctly)
	 * @return Whether the Actor is initialized
	 */
	bool isInitialized() const;

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

	/**
	 * Determines whether this Actor should receive mouse press events  
	 * @return Whether this Actor should receive mouse press events
	 */
	bool shouldReceiveMousePressEvents() const;

	/**
	 * Determines whether this Actor should receive release events
	 * @return Whether this Actor should receive mouse release events
	 */
	bool shouldReceiveMouseReleaseEvents() const;

	/**
	 * Determines whether this Actor should receive mouse move events
	 * @return Whether this Actor should receive mouse move events
	 */
	bool shouldReceiveMouseMoveEvents() const;

	/**
	 * Determines whether this Actor should receive mouse wheel events
	 * @return Whether this Actor should receive mouse wheel events
	 */
	bool shouldReceiveMouseWheelEvents() const;
	
	/** Returns whether the Actor is active e.g. interaction with the mouse */
	bool isActive() const;

	/** Activate the Actor */
	virtual void activate();

	/** Deactivate the Actor */
	virtual void deactivate();

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

		connect(shape.get(), &ImageQuad::changed, [&](Shape* shape) {
			emit changed(this);
		});
	}

protected:	
	/**
	 * Determines whether this Actor may process mouse press events (e.g. is the Actor enabled and/or active) */
	bool mayProcessMousePressEvent() const;

	/**
	 * Determines whether this Actor may process mouse release events (e.g. is the Actor enabled and/or active) */
	bool mayProcessMouseReleaseEvent() const;

	/**
	 * Determines whether this Actor may process mouse move events (e.g. is the Actor enabled and/or active) */
	bool mayProcessMouseMoveEvent() const;

	/**
	 * Determines whether this Actor may process mouse wheel events (e.g. is the Actor enabled and/or active) */
	bool mayProcessMouseWheelEvent() const;

signals:
	/** Signals that the Actor has been successfully initialized */
	void initialized();

	/** Signals that the Actor name changed
	 * @param name New Actor name
	 */
	void nameChanged(const QString& name);

	/** Signals that the Actor has been enabled or disabled
	 * @param enabled Whether the Actor is enabled or not
	 */
	void enabledChanged(const bool& enabled);

	/** Signals that the model matrix changed
	 * @param modelMatrix Model matrix
	 */
	void modelMatrixChanged(const QMatrix4x4& modelMatrix);

	/** Signals that the Actor changed */
	void changed(Actor* Actor);

protected:
	Renderer*								_renderer;					/** Pointer to renderer */
	QString									_name;						/** Name of the Actor */
	bool									_active;					/** Actors is being interacted with */
	int										_receiveMouseEvents;		/** Defines which type of mouse events should be received by the Actor */
	bool									_initialized;				/** Whether the Actor is initialized or not */
	bool									_enabled;					/** Whether the Actor is enabled or not */
	QMatrix4x4								_modelMatrix;				/** Model matrix */
	QMap<QString, QSharedPointer<Shape>>	_shapes;					/** Shapes map */
};