#pragma once

#include "Common.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class Prop;
class Renderer;

/**
 * Shape class
 *
 * Abstract base class for rendering shapes onto the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class Shape : public QObject
{
public:

	/** Constructor
	 * @param prop Parent prop
	 * @param name Name of the shape
	 */
	Shape(Prop* prop, const QString& name);

	/** Destructor */
	~Shape() override;

public:

	/** Initializes the shape */
	virtual void initialize();

	/** Destroys the shape */
	virtual void destroy();

	/** Returns if the shape can be rendered */
	[[nodiscard]] virtual bool canRender() const = 0;

	/** Renders the shape */
	virtual void render() = 0;

	/** Returns the shape name */
	[[nodiscard]] QString name() const;

	/**
	 * Sets the name
	 * @param name Shape name
	 */
	void setName(const QString& name);

	/** Returns the prop to which the shape is attached */
	Prop* prop();

	/** Returns the full shape name (actor_name::prop_name::shape_name */
	QString fullName();

	/** Returns the vertex array object */
	QOpenGLVertexArrayObject& vao() { return _vao; }

	/** Returns the vertex buffer object */
	QOpenGLBuffer& vbo() { return _vbo; }

protected:
	Prop*						_prop;		/** Parent prop */
	QString						_name;		/** Name of the shape */
	QOpenGLVertexArrayObject	_vao;		/** OpenGL Vertex Array Object (VAO) */
	QOpenGLBuffer				_vbo;		/** OpenGL Vertex Buffer Object (VBO) */

	friend class Prop;
};