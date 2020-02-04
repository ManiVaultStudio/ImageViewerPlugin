#pragma once

#include "Common.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class Prop;
class Renderer;

/**
 * Shape class
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
	~Shape();

public:
	/** Initializes the shape */
	virtual void initialize();

	/** Destroys the shape */
	virtual void destroy();

	/** Returns if the shape can be rendered */
	virtual bool canRender() const = 0;

	/** Renders the shape */
	virtual void render() = 0;

	/** Returns the shape name */
	QString name() const;

	/**
	 * Sets the name
	 * @param name Shape name
	 */
	void setName(const QString& name);

	/** Returns the actor */
	Prop* prop();

	/** Returns the renderer */
	Renderer* renderer();

	/** Returns the full shape name (actor_name::prop_name::shape_name */
	QString fullName();

	QOpenGLVertexArrayObject& vao() { return _vao; }
	QOpenGLBuffer& vbo() { return _vbo; }

protected:
	Prop*						_prop;		/** Parent prop */
	QString						_name;		/** Name of the shape */
	QOpenGLVertexArrayObject	_vao;		/** OpenGL Vertex Array Object (VAO) */
	QOpenGLBuffer				_vbo;		/** OpenGL Vertex Buffer Object (VBO) */

	friend class Prop;
};