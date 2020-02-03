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

	/**
	 * Determines whether the shape is properly initialized (all buffers etc. are setup correctly)
	 * @return Whether the shape is initialized
	 */
	bool isInitialized() const;

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

protected:
	/** Initializes the shape */
	virtual void initialize();

	/** Destroys the shape */
	virtual void destroy();

	/** Returns the full shape name e.g. Actor::Prop::Shape */
	QString fullName();

protected:
	Prop*						_prop;				/** Parent prop */
	QString						_name;				/** Name of the shape */
	bool						_initialized;		/** Whether the shape is initialized or not */
	QOpenGLVertexArrayObject	_vao;				/** OpenGL Vertex Array Object (VAO) */
	QOpenGLBuffer				_vbo;				/** OpenGL Vertex Buffer Object (VBO) */
};