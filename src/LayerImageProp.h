#pragma once

#include "Prop.h"
#include "Layer.h"

class Layer;

/**
 * Layer image prop class
 *
 * Prop class for rendering layer image
 *
 * @author Thomas Kroes
 */
class LayerImageProp : public Prop
{
    Q_OBJECT

public:

    using DisplayRange  = std::pair<float, float>;
    using DisplayRanges = std::vector<DisplayRange>;

public: // Enumerations

    /** Texture identifiers */
    enum class TextureId {
        ColorMap = 0,
        Channel1,
        Channel2,
        Channel3,
        Channel4
    };

public: // Construction/destruction

    /**
     * Constructor
     * @param layer Reference to layer in which the prop resides
     * @param name Name of the prop
     */
    LayerImageProp(Layer& layer, const QString& name);

    /** Destructor */
    ~LayerImageProp() override = default;

public: // Rendering

    /**
     * Renders the prop
     * @param nodeMVP Node model view projection matrix
     * @param opacity Render opacity [0-1]
     */
    void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

    void setImageSize(const QSize& imageSize);

    /**
     * Set the color map image
     * @param colorMapImage Color map image
     */
    void setColorMapImage(const QImage& colorMapImage);

    /**
     * Set channel scalar data
     * @param channelIndex Channel index
     * @param scalarData Scalar data
     * @param displayRange Display range
     */
    void setChannelScalarData(const std::uint32_t& channelIndex, const std::vector<float>& scalarData, const DisplayRange& displayRange);

    /** Returns the bounding rectangle of the prop */
    QRectF getBoundingRectangle() const override;

protected: // Inherited

    /** Initializes the prop */
    void initialize() override;

protected: // Miscellaneous

    /** Updates the internal model matrix */
    void updateModelMatrix();

protected:
    Layer&          _layer;             /** Reference to layer */
    DisplayRanges   _displayRanges;     /** Display ranges */
};
