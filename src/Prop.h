#pragma once

#include "Common.h"

#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

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
	virtual ~Prop();

	/** Returns whether the prop is initialized */
	bool isInitialized() const;

	/** Returns the prop name */
	QString name() const;

	/**
	 * Sets the prop name
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

	/** Returns the full shape name (actor_name::prop_name::shape_name */
	QString fullName();

	/** Returns the model matrix */
	QMatrix4x4 modelMatrix() const;

	/**
	 * Sets the model matrix
	 * @param matrix Model matrix
	 */
	void setModelMatrix(const QMatrix4x4& modelMatrix);

	/** Returns the model-view matrix (viewMatrix * actorMatrix * propModelMatrix) */
	QMatrix4x4 modelViewMatrix() const;

	/** Returns the model-view-projection matrix (projectionMatrix * viewMatrix * actorModelMatrix * propModelMatrix) */
	QMatrix4x4 modelViewProjectionMatrix() const;

	/** Returns the parent actor */
	Actor* actor();

	/** Returns the renderer */
	Renderer* renderer();

	/** Renders the prop */
	virtual void render();

protected:
	/** Initializes the prop */
	virtual void initialize();

	/** Destroys the prop */
	virtual void destroy();

	/** Returns whether the prop can be rendered */
	virtual bool canRender() const;

	/** Update shapes */
	virtual void updateShapes() {};

	/** Update textures */
	virtual void updateTextures() {};

	/**
	* Get shape by name
	* @param name Name of the shape
	*/
	template<typename T>
	T* shapeByName(const QString& name)
	{
		return dynamic_cast<T*>(_shapes[name].get());
	}

	/**
	* Get shape by name
	* @param name Name of the shape
	*/
	template<typename T>
	const T* shapeByName(const QString& name) const
	{
		return dynamic_cast<T*>(_shapes[name].get());
	}

	/**
	* Get shader program by name
	* @param name Name of the shader program
	*/
	QSharedPointer<QOpenGLShaderProgram> shaderProgramByName(const QString& name)
	{
		return _shaderPrograms.value(name);
	}

	/**
	* Get texture by name
	* @param name Name of the texture
	*/
	QSharedPointer<QOpenGLTexture> textureByName(const QString& name)
	{
		return _textures.value(name);
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
	}

	/**
	* Add shader program by name
	* @param name Name of the shader program
	*/
	void addShaderProgram(const QString& name)
	{
		_shaderPrograms.insert(name, QSharedPointer<QOpenGLShaderProgram>::create());
	}

	/**
	* Add texture by name
	* @param name Name of the texture
	*/
	void addTexture(const QString& name, const QOpenGLTexture::Target& target)
	{
		_textures.insert(name, QSharedPointer<QOpenGLTexture>::create(target));
	}

signals:

	/** TODO */
	void becameDirty(Prop* prop);

protected:
	Actor*													_actor;					/** Pointer to owning actor */
	bool													_initialized;			/** Whether the prop is ready for rendering */
	QString													_name;					/** Name of the prop */
	bool													_visible;				/** Whether the prop is visible or not */

private:
	QMatrix4x4												_modelMatrix;			/** Transformation matrix */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>		_shaderPrograms;		/** OpenGL shader programs */
	QMap<QString, QSharedPointer<QOpenGLTexture>>			_textures;				/** OpenGL textures */
	QMap<QString, QSharedPointer<Shape>>					_shapes;				/** Shapes */

	friend class Actor;
};

using SharedProp = QSharedPointer<Prop>;