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

    using DisplayRange  = QPair<float, float>;
    using DisplayRanges = QVector<DisplayRange>;

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
     * Set the color map image
     * @param colorMapImage Color map image
     */
    void setColorMapImage(const QImage& colorMapImage);

    /**
     * Set channel scalar data
     * @param channelIndex Channel index
     * @param sourceImageRectangle Source image rectangle
     * @param targetImageRectangle Target image rectangle
     * @param imageSize Image size
     * @param scalarData Scalar data
     * @param displayRange Display range
     */
    void setChannelScalarData(const std::uint32_t& channelIndex, const QRect& sourceImageRectangle, const QRect& targetImageRectangle, const QSize& imageSize, const QVector<float>& scalarData, const DisplayRange& displayRange);

    /**
     * Set image interpolation type
     * @param interpolationType Interpolation type
     */
    void setInterpolationType(const InterpolationType& interpolationType);

    /** Returns the bounding rectangle of the prop */
    QRectF getWorldBoundingRectangle() const override;

    /** Initializes the prop */
    void initialize() override;

protected:
    Layer&          _layer;             /** Reference to layer */
    DisplayRanges   _displayRanges;     /** Display ranges */
};
