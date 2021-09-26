#pragma once

#include "Prop.h"
#include "Layer.h"

#include "util/Interpolation.h"

class Layer;

using namespace hdps::util;

/**
 * Image prop class
 *
 * Prop class for rendering layer image
 *
 * @author Thomas Kroes
 */
class ImageProp : public Prop
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
    ImageProp(Layer& layer, const QString& name);

    /** Destructor */
    ~ImageProp() override = default;

public: // Rendering

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

    /**
     * Set image interpolation type
     * @param interpolationType Interpolation type
     */
    void setInterpolationType(const InterpolationType& interpolationType);

    /** Returns the bounding rectangle of the prop */
    QRectF getWorldBoundingRectangle() const override;

    /** Updates the internal model matrix */
    void updateModelMatrix();

    /** Initializes the prop */
    void initialize() override;

protected:
    Layer&          _layer;             /** Reference to layer */
    DisplayRanges   _displayRanges;     /** Display ranges */
};
