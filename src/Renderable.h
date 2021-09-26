#pragma once

#include <QObject>
#include <QMatrix4x4>
#include <QMap>

#include "Prop.h"

#include <stdexcept>

class Renderer;

/**
 * Renderable class
 *
 * Base class for renderable nodes
 *
 * @author Thomas Kroes
 */
class Renderable : public QObject
{
public: // Construction/destruction

    /**
     * Constructor
     * @param renderer Reference to the renderer in which the renderable object will reside
     */
    Renderable(Renderer& renderer);

    /** Destructor */
    ~Renderable() = default;

public: // Rendering

    /**
     * Renders the props
     * @param parentMVP Parent model view projection matrix
     */
    virtual void render(const QMatrix4x4& modelViewProjectionMatrix) = 0;

    /** Get reference to the renderer */
    Renderer& getRenderer();

    /** Returns the model matrix */
    QMatrix4x4 getModelMatrix() const;

    /**
     * Sets the model matrix
     * @param modelMatrix Model matrix
     */
    void setModelMatrix(const QMatrix4x4& modelMatrix);

    /** Returns the model-view matrix */
    QMatrix4x4 getModelViewMatrix() const;

    /** Returns the model-view-projection matrix */
    QMatrix4x4 getModelViewProjectionMatrix() const;

    /** Get the bounding rectangle of the prop in world coordinates */
    virtual QRectF getWorldBoundingRectangle() const = 0;

    /**
     * Retrieve a prop by name
     * @param name Prop name
     */
    template<typename T>
    const T* getPropByName(const QString& name) const
    {
        try {
            for (auto prop : _props)
                if (prop->getName() == name)
                    return dynamic_cast<T*>(prop);

            throw std::runtime_error(QString("%1 does not exist").arg(name).toLatin1());
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(QString("Unable to retrieve prop: %1").arg(e.what()).toLatin1());
        }
    }

    /**
     * Retrieve a prop by name
     * @param name Prop name
     */
    template<typename T>
    T* getPropByName(const QString& name)
    {
        const auto constThis = const_cast<const Renderable*>(this);
        return const_cast<T*>(constThis->getPropByName<T>(name));
    }

    /** Returns all props */
    const QVector<Prop*> getProps() const
    {
        return _props;
    }

protected:
    Renderer&           _renderer;      /** Reference to the renderer in which the renderable object will reside */
    QMatrix4x4          _modelMatrix;   /** Model matrix */
    QVector<Prop*>      _props;         /** Props */
};