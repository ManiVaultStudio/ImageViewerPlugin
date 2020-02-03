#pragma once

#include "Common.h"

#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

class QOpenGLShaderProgram;
class QOpenGLTexture;
class QOpenGLFramebufferObject;

class Renderer;
class Actor;
class Shape;

/**
 * Prop class
 * @author Thomas Kroes
 */
class Prop : public QObject
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	Prop(Actor* actor, const QString& name);

	/** Destructor */
	~Prop();

	/** Performs a cleanup before the prop is deleted */
	virtual void destroy();

	/** Initializes the prop */
	virtual void initialize();

	/** Returns whether the prop is initialized */
	bool isInitialized() const;

	/** Returns the prop name */
	QString name() const;

	/**
	 * Sets the name
	 * @param name Prop name
	 */
	void setName(const QString& name);

	/** Returns whether the prop is visible */
	bool isVisible() const;

	/**
	 * Sets whether the prop is visible
	 * @param visible Whether the prop is visible
	 */
	void setVisible(const bool& visible);

	/** Shows the prop */
	void show();

	/** Hides the prop */
	void hide();

	/** Returns whether the prop can be rendered */
	virtual bool canRender() const;
	
	/** Renders the prop */
	virtual void render();

	/** Binds the OpenGL context */
	void bindOpenGLContext();

	/** Releases the OpenGL context */
	void releaseOpenGLContext();

	/** Returns the renderer in which this prop resides */
	Renderer* renderer();

signals:
	/** Signals that the prop has been successfully initialized */
	void initialized();

	/** Signals that the prop name changed
	 * @param name New prop name
	 */
	void nameChanged(const QString& name);

	/** Signals that the visibility changed
	 * @param visible Whether the prop is visible or not
	 */
	void visibilityChanged(const bool& visible);

	/** Signals that the prop changed */
	void changed(Prop* prop);

protected:
	Actor*													_actor;					/** Pointer to owning actor */
	bool													_initialized;			/** Whether the prop is ready for rendering */
	QString													_name;					/** Name of the prop */
	bool													_visible;				/** Whether the prop is visible or not */
	QMatrix4x4												_matrix;				/** Transformation matrix */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>		_shaderPrograms;		/** OpenGL shader programs */
	QMap<QString, QSharedPointer<QOpenGLTexture>>			_textures;				/** OpenGL textures */
	QMap<QString, QSharedPointer<Shape>>					_shapes;				/** Shapes */
};