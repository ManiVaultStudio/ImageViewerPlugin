#pragma once

#include "Common.h"

#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class Renderer;
class Node;
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
	 * @param node Node
	 * @param name Name of the prop
	 */
	Prop(Node* node, const QString& name);

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

	/** Renders the prop */
	virtual void render(const QMatrix4x4& nodeMVP, const float& opacity);

	/** Computes the enveloping bounding rectangle of the port */
	virtual QRectF boundingRectangle() const = 0;

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
	QSharedPointer<QOpenGLTexture>& textureByName(const QString& name)
	{
		return _textures[name];
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

public:
	static Renderer* renderer;

protected:
	Node*													_node;					/** Pointer to parent (if any) */
	bool													_initialized;			/** Whether the prop is ready for rendering */
	QString													_name;					/** Name of the prop */
	bool													_visible;				/** Whether the prop is visible or not */

private:
	QMatrix4x4												_modelMatrix;			/** Transformation matrix */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>		_shaderPrograms;		/** OpenGL shader programs */
	QMap<QString, QSharedPointer<QOpenGLTexture>>			_textures;				/** OpenGL textures */
	QMap<QString, QSharedPointer<Shape>>					_shapes;				/** Shapes */
};

using SharedProp = QSharedPointer<Prop>;