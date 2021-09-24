#pragma once

#include "Shape.h"

#include <QRect>
#include <QVector>

/**
 * Quad shape class
 *
 * Draws a textured quad on the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class QuadShape : public Shape
{
public: // Construction

    /**
     * Constructor
     * @param prop Reference to the prop in which the shape resides
     * @param name Name of the shape
     */
    QuadShape(Prop& prop, const QString& name);

public: // Inherited

    /** Initializes the shape */
    void initialize() override;

    /** Returns if the shape can be rendered */
    bool canRender() const override;

    /** Renders the shape */
    void render() override;

public: // Miscellaneous

    /** Returns the quad rectangle */
    QRectF getRectangle() const;

    /**
     * Set quad rectangle
     * @param rectangle Rectangle in world space
     */
    void setRectangle(const QRectF& rectangle);

    /** Returns the quad size */
    QSizeF getImageSize() const;

private: // Internal function(s)

    /**
     * Creates the OpenGL quad buffers
     * @param rectangle Quad rectangle
     */
    void createQuad();

public:
    static std::uint32_t    _vertexAttribute;       /** Quad vertex attribute location */
    static std::uint32_t    _textureAttribute;      /** Quad texture attribute location */

protected:
    QRectF                  _rectangle;             /** Rectangle (in world space) that defines the quad */
    QVector<GLfloat>        _vertexData;            /** Quad vertex data */
};