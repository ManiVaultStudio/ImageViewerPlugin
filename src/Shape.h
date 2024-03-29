#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class Prop;
class LayersRenderer;

/**
 * Shape class
 *
 * Abstract base class for rendering shapes onto the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class Shape
{
public:

    /** Constructor
     * @param prop Reference to prop in which the shapes resides
     * @param name Name of the shape
     */
    Shape(Prop& prop, const QString& name);

    /** Destructor */
    ~Shape() = default;

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
    QString getName() const;

    /**
     * Sets the name
     * @param name Shape name
     */
    void setName(const QString& name);

    /** Returns reference to the prop in which the shapes resides */
    Prop& getProp();

    /** Returns the full shape name (actor_name::prop_name::shape_name */
    QString getFullName();

    /** Returns the vertex array object */
    QOpenGLVertexArrayObject& getVAO() { return _vao; }

    /** Returns the vertex buffer object */
    QOpenGLBuffer& getVBO() { return _vbo; }

protected:
    Prop&                       _prop;      /** Reference to prop in which the shapes resides */
    QString                     _name;      /** Name of the shape */
    QOpenGLVertexArrayObject    _vao;       /** OpenGL Vertex Array Object (VAO) */
    QOpenGLBuffer               _vbo;       /** OpenGL Vertex Buffer Object (VBO) */

    friend class Prop;
};
