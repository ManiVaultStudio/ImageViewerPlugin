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
    Q_OBJECT

public:

    /**
     * Constructor
     * @param layer Reference to layer in which the prop resides
     * @param name Name of the prop
     */
    SelectionProp(Layer& layer, const QString& name);

    /** Destructor */
    ~SelectionProp() = default;

    /**
     * Renders the prop
     * @param modelViewProjectionMatrix Model view projection matrix
     */
    void render(const QMatrix4x4& modelViewProjectionMatrix) override;

    /**
     * Set image size
     * @param imageSize Image size
     */
    void setImageSize(const QSize& imageSize);

    /** Get the bounding rectangle of the prop in world coordinates */
    QRectF getWorldBoundingRectangle() const override;

    /** Initializes the prop */
    void initialize() override;

    /** Updates the internal model matrix */
    void updateModelMatrix();

    /**
     * Set selection data
     * @param selectionData Selection data
     */
    void setSelectionData(const std::vector<std::uint8_t>& selectionData);

protected:
    Layer&      _layer;     /** Reference to layer */
};