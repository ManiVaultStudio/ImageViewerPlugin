#pragma once

#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class LayersRenderer;
class Renderable;
class Shape;

/**
 * Prop class
 *
 * Abstract base class for props
 * Props are used to draw nodes (layers) on the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class Prop
{
public: // Construction/destruction

    /**
     * Constructor
     * @param renderable Reference to renderable object
     * @param name Name of the prop
     */
    Prop(Renderable& renderable, const QString& name);

    /** Destructor */
    virtual ~Prop() {};

public:

    /** Returns whether the prop is initialized */
    bool isInitialized() const;

    /** Returns the prop name */
    QString getName() const;

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
    QString getFullName();

public: // Geometry

    /** Returns the model matrix */
    QMatrix4x4 getModelMatrix() const;

    /**
     * Sets the model matrix
     * @param matrix Model matrix
     */
    void setModelMatrix(const QMatrix4x4& modelMatrix);

    /** Gets the bounding rectangle of the prop in world coordinates */
    virtual QRectF getWorldBoundingRectangle() const = 0;

    /** Gets the bounding rectangle of the prop in screen coordinates */
    virtual QRect getScreenBoundingRectangle() const final;

    /**
     * Renders the prop
     * @param modelViewProjectionMatrix Model view projection matrix
     */
    virtual void render(const QMatrix4x4& modelViewProjectionMatrix);

    /** Get reference to the renderer */
    LayersRenderer& getRenderer();

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

protected: // Shape management

    /**
    * Add shape by name
    * @param name Name of the shape
    */
    template<typename T>
    void addShape(const QString& name)
    {
        auto shape = QSharedPointer<T>::create(*this, name);

        _shapes.insert(name, shape);
    }

    /**
    * Get shape by name
    * @param name Name of the shape
    */
    template<typename T>
    T* getShapeByName(const QString& name)
    {
        return dynamic_cast<T*>(_shapes[name].get());
    }

    /**
    * Get shape by name
    * @param name Name of the shape
    */
    template<typename T>
    const T* getShapeByName(const QString& name) const
    {
        return dynamic_cast<T*>(_shapes[name].get());
    }

protected: // Shader program management

    /**
    * Add shader program by name
    * @param name Name of the shader program
    */
    void addShaderProgram(const QString& name);

    /**
    * Get shader program by name
    * @param name Name of the shader program
    */
    QSharedPointer<QOpenGLShaderProgram> getShaderProgramByName(const QString& name);

protected: // Texture management

    /**
    * Add texture by name
    * @param name Name of the texture
    */
    void addTexture(const QString& name, const QOpenGLTexture::Target& target);

    /**
    * Get texture by name
    * @param name Name of the texture
    */
    QSharedPointer<QOpenGLTexture>& getTextureByName(const QString& name);

protected:
    Renderable&     _renderable;        /** Reference to renderable object */
    bool            _initialized;       /** Whether the prop is ready for rendering */
    QString         _name;              /** Name of the prop */
    bool            _visible;           /** Whether the prop is visible or not */

private:
    QMatrix4x4                                              _modelMatrix;           /** Transformation matrix */
    QMap<QString, QSharedPointer<QOpenGLShaderProgram>>     _shaderPrograms;        /** OpenGL shader programs */
    QMap<QString, QSharedPointer<QOpenGLTexture>>           _textures;              /** OpenGL textures */
    QMap<QString, QSharedPointer<Shape>>                    _shapes;                /** Shapes */
};