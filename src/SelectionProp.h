#pragma once

#include "Prop.h"

class Layer;

/**
 * Selection prop class
 *
 * This prop renders a selection layer onto the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class SelectionProp : public Prop
{
public:

    /**
     * Constructor
     * @param layer Reference to layer in which the prop resides
     * @param name Name of the prop
     */
    SelectionProp(Layer& layer, const QString& name);

    /** Destructor */
    ~SelectionProp() = default;

    /** Initializes the prop */
    void initialize() override;

    /**
     * Renders the prop
     * @param modelViewProjectionMatrix Model view projection matrix
     */
    void render(const QMatrix4x4& modelViewProjectionMatrix) override;

    /** Get the bounding rectangle of the prop in world coordinates */
    QRectF getWorldBoundingRectangle() const override;

    /**
     * Set the geometry
     * @param imageRectangle Image rectangle
     */
    void setGeometry(const QRectF& imageRectangle);

    /**
     * Set selection data
     * @param selectionData Selection data
     */
    void setSelectionData(const std::vector<std::uint8_t>& selectionData);

    /**
     * Set mask data
     * @param maskData Mask data
     */
    void setMaskData(const std::vector<std::uint8_t>& maskData);

protected:
    Layer&      _layer;     /** Reference to layer */
};