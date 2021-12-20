#include "ImageAction.h"
#include "Layer.h"

#include "util/ColorSpace.h"
#include "util/Interpolation.h"
#include "event/Event.h"

#include "PointData.h"
#include "ClusterData.h"

using namespace hdps;
using namespace hdps::util;

ImageAction::ImageAction(Layer& layer) :
    GroupAction(&layer, true),
    EventListener(),
    _layer(layer),
    _opacityAction(this, "Opacity", 0.0f, 100.0f, 100.0f, 100.0f, 1),
    _subsampleFactorAction(this, "Subsample", 1, 8, 1, 1),
    _colorSpaceAction(this, "Color space", colorSpaces.values(), "Mono", "Mono"),
    _scalarChannel1Action(*this, ScalarChannelAction::Channel1, ScalarChannelAction::channelIndexes.value(ScalarChannelAction::Channel1)),
    _scalarChannel2Action(*this, ScalarChannelAction::Channel2, ScalarChannelAction::channelIndexes.value(ScalarChannelAction::Channel2)),
    _scalarChannel3Action(*this, ScalarChannelAction::Channel3, ScalarChannelAction::channelIndexes.value(ScalarChannelAction::Channel3)),
    _colorMapAction(this, "Color map", ColorMap::Type::OneDimensional, "Black to white", "Black to white"),
    _interpolationTypeAction(this, "Interpolate", interpolationTypes.values(), "Bilinear", "Bilinear"),
    _useConstantColorAction(this, "Use constant color", false, false),
    _constantColorAction(this, "Constant color", QColor(Qt::white), QColor(Qt::white))
{
    setText("Image");
    setEventCore(Application::core());

    // Establish whether the source dataset is a clusters dataset
    const auto isClusterType = _layer.getSourceDataset()->getDataType() == ClusterType;

    _subsampleFactorAction.setVisible(false);

    _opacityAction.setToolTip("Image layer opacity");
    _subsampleFactorAction.setToolTip("Subsampling factor");
    _scalarChannel1Action.setToolTip("Scalar channel 1");
    _scalarChannel2Action.setToolTip("Scalar channel 2");
    _scalarChannel3Action.setToolTip("Scalar channel 3");
    _colorSpaceAction.setToolTip("The color space used to shade the image");
    _colorMapAction.setToolTip("Image color map");
    _interpolationTypeAction.setToolTip("The type of two-dimensional image interpolation used");
    _useConstantColorAction.setToolTip("Use constant color to shade the image");
    _constantColorAction.setToolTip("Constant color");

    _opacityAction.setSuffix("%");

    // Set initial color map type
    _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);

    // Disable horizontal range actions
    _colorMapAction.getSettingsAction().getHorizontalAxisAction().getRangeAction().getRangeMinAction().setEnabled(false);
    _colorMapAction.getSettingsAction().getHorizontalAxisAction().getRangeAction().getRangeMaxAction().setEnabled(false);

    // Disable vertical range actions
    _colorMapAction.getSettingsAction().getVerticalAxisAction().getRangeAction().getRangeMinAction().setEnabled(false);
    _colorMapAction.getSettingsAction().getVerticalAxisAction().getRangeAction().getRangeMaxAction().setEnabled(false);

    // Get the dimension names of the points dataset
    const auto dimensionNames = _layer.getDimensionNames();

    const auto useConstantColorToggled = [this]() {
        _colorMapAction.setEnabled(!_useConstantColorAction.isChecked());
        _constantColorAction.setEnabled(_useConstantColorAction.isChecked());
        _layer.invalidate();
    };

    connect(&_useConstantColorAction, &ToggleAction::toggled, this, useConstantColorToggled);

    connect(&_scalarChannel1Action, &ScalarChannelAction::changed, this, [this]() {
        emit channelChanged(_scalarChannel1Action);
    });
    
    connect(&_scalarChannel2Action, &ScalarChannelAction::changed, this, [this]() {
        emit channelChanged(_scalarChannel2Action);
    });

    connect(&_scalarChannel3Action, &ScalarChannelAction::changed, this, [this]() {
        emit channelChanged(_scalarChannel3Action);
    });

    //connect(&_scalarChannelMaskAction, &ScalarChannelAction::changed, this, [this]() {
    //    emit channelChanged(_scalarChannelMaskAction);
    //});

    const auto render = [this]() {
        _layer.invalidate();
    };

    connect(&_opacityAction, &DecimalAction::valueChanged, this, render);
    connect(&_subsampleFactorAction, &IntegralAction::valueChanged, this, render);
    connect(&_interpolationTypeAction, &OptionAction::currentIndexChanged, this, render);
    connect(&_constantColorAction, &ColorAction::colorChanged, this, render);

    updateScalarChannelActions();
    useConstantColorToggled();

    connect(&_colorSpaceAction, &OptionAction::currentIndexChanged, this, &ImageAction::updateScalarChannelActions);

    // Set color space to mono in case of one dimension
    if (dimensionNames.count() == 1)
        _colorSpaceAction.setCurrentText("Mono");

    // Set color space to duo in case of two dimensions
    if (dimensionNames.count() == 2)
        _colorSpaceAction.setCurrentText("Duo");

    // Set channel dimension names
    _scalarChannel1Action.getDimensionAction().setOptions(dimensionNames);
    _scalarChannel2Action.getDimensionAction().setOptions(dimensionNames);
    _scalarChannel3Action.getDimensionAction().setOptions(dimensionNames);

    _scalarChannel1Action.getDimensionAction().setCurrentIndex(0);
    _scalarChannel1Action.getDimensionAction().setDefaultIndex(0);

    if (isClusterType) {
        
        // Configure image interpolation action
        _interpolationTypeAction.setCurrentIndex(static_cast<std::int32_t>(InterpolationType::NearestNeighbor));
        _interpolationTypeAction.setEnabled(false);

        // Configure color space action
        _colorSpaceAction.setEnabled(false);
        _colorSpaceAction.setCurrentText("Mono");

        // Configure color map action
        _colorMapAction.setEnabled(false);

        // Configure constant color action
        _constantColorAction.setEnabled(false);

        // Disable all channels
        _scalarChannel1Action.getWindowLevelAction().setEnabled(false);
        _scalarChannel2Action.getWindowLevelAction().setEnabled(false);
        _scalarChannel3Action.getWindowLevelAction().setEnabled(false);
    }
    else {
        if (_layer.getNumberOfImages() >= 2) {
            _scalarChannel2Action.getDimensionAction().setCurrentIndex(1);
            _scalarChannel2Action.getDimensionAction().setDefaultIndex(1);
        }

        if (_layer.getNumberOfImages() >= 3) {
            _scalarChannel3Action.getDimensionAction().setCurrentIndex(2);
            _scalarChannel3Action.getDimensionAction().setDefaultIndex(2);
        }
    }

    // Update the color map image when the discrete color map option changes
    connect(&_colorMapAction, &ColorMapAction::imageChanged, this, &ImageAction::updateColorMapImage);
    connect(&_colorMapAction.getSettingsAction().getDiscreteAction(), &ColorMapDiscreteAction::toggled, this, &ImageAction::updateColorMapImage);

    const auto updateScalarChannels = [this]() {
        _scalarChannel1Action.computeScalarData();
        _scalarChannel2Action.computeScalarData();
        _scalarChannel3Action.computeScalarData();
    };

    // Register for events for points datasets
    registerDataEventByType(PointType, [this, updateScalarChannels](DataEvent* dataEvent) {

        // The points dataset might have been deleted so check first if it is valid
        if (!_layer.getSourceDataset().isValid())
            return;

        // Only process points dataset that is referenced by us
        if (dataEvent->getDataset() != _layer.getSourceDataset())
            return;

        switch (dataEvent->getType())
        {
            case EventType::DataChanged:
            {
                updateScalarChannels();

                break;
            }

            case EventType::DataSelectionChanged:
            {
                _layer.computeSelectionIndices();
                break;
            }

            default:
                break;
        }
    });

    // Register for events for clusters datasets
    registerDataEventByType(ClusterType, [this, updateScalarChannels](DataEvent* dataEvent) {

        // The points dataset might have been deleted so check first if it is valid
        if (!_layer.getSourceDataset().isValid())
            return;

        // Only process points dataset that is referenced by us
        if (dataEvent->getDataset() != _layer.getSourceDataset())
            return;

        switch (dataEvent->getType())
        {
            case EventType::DataChanged:
            {
                updateScalarChannels();
                updateColorMapImage();
                break;
            }

            case EventType::DataSelectionChanged:
            {
                _layer.computeSelectionIndices();
                break;
            }

            default:
                break;
        }
    });

    // Do an initial update of the scalar data
    updateScalarChannels();
}

void ImageAction::init()
{
    updateColorMapImage();
}

const std::uint32_t ImageAction::getNumberOfActiveScalarChannels() const
{
    switch (static_cast<ColorSpaceType>(_colorSpaceAction.getCurrentIndex()))
    {
        case ColorSpaceType::Mono:
            return 1;

        case ColorSpaceType::Duo:
            return 2;

        case ColorSpaceType::RGB:
        case ColorSpaceType::HSL:
        case ColorSpaceType::LAB:
            return 3;

        default:
            break;
    }

    return 0;
}

QImage ImageAction::getColorMapImage() const
{
    if (_layer.getSourceDataset()->getDataType() == ClusterType) {
        
        // Get pointer to clusters
        auto clusters = Dataset<Clusters>(_layer.getSourceDataset())->getClusters();

        // Create discrete one-dimensional color map image
        QImage discreteColorMapImage(static_cast<std::int32_t>(clusters.size()), 1, QImage::Format::Format_RGB32);

        auto clusterIndex = 0;

        // Populate color map
        for (const auto& cluster : clusters) {

            // Assign pixel color by cluster color
            discreteColorMapImage.setPixelColor(clusterIndex, 0, cluster.getColor());
            clusterIndex++;
        }

        return discreteColorMapImage.convertToFormat(QImage::Format_RGB32);
    }
    else {
        return _colorMapAction.getColorMapImage();
    }
}

void ImageAction::updateColorMapImage()
{
    // Establish the color map image interpolation type
    const auto isDiscreteColorMap   = _colorMapAction.getSettingsAction().getDiscreteAction().isChecked();
    const auto interpolationType    = isDiscreteColorMap ? InterpolationType::NearestNeighbor : InterpolationType::Bilinear;

    // Set the color map image in the layer
    _layer.setColorMapImage(getColorMapImage(), interpolationType);
}

void ImageAction::updateScalarChannelActions()
{
    // Establish whether the source dataset is a clusters dataset
    const auto isClusterType = _layer.getSourceDataset()->getDataType() == ClusterType;

    switch (static_cast<ColorSpaceType>(_colorSpaceAction.getCurrentIndex()))
    {
        case ColorSpaceType::Mono:
        {
            _scalarChannel1Action.getEnabledAction().setChecked(true);
            _scalarChannel2Action.getEnabledAction().setChecked(false);
            _scalarChannel3Action.getEnabledAction().setChecked(false);

            _scalarChannel1Action.setText("Channel 1");
            _scalarChannel2Action.setText("Channel 2");
            _scalarChannel3Action.setText("Channel 3");

            _colorMapAction.setEnabled(isClusterType ? false : true);
            _colorMapAction.setColorMapType(ColorMap::Type::OneDimensional);

            break;
        }

        case ColorSpaceType::Duo:
        {
            _scalarChannel1Action.getEnabledAction().setChecked(true);
            _scalarChannel2Action.getEnabledAction().setChecked(true);
            _scalarChannel3Action.getEnabledAction().setChecked(false);

            _scalarChannel1Action.setText("Channel 1");
            _scalarChannel2Action.setText("Channel 2");
            _scalarChannel3Action.setText("Channel 3");

            _colorMapAction.setEnabled(true);
            _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);

            break;
        }

        case ColorSpaceType::RGB:
        {
            _scalarChannel1Action.getEnabledAction().setChecked(true);
            _scalarChannel2Action.getEnabledAction().setChecked(true);
            _scalarChannel3Action.getEnabledAction().setChecked(true);

            _scalarChannel1Action.setText("Red");
            _scalarChannel2Action.setText("Green");
            _scalarChannel3Action.setText("Blue");

            _colorMapAction.setEnabled(false);

            break;
        }

        case ColorSpaceType::HSL:
        {
            _scalarChannel1Action.getEnabledAction().setChecked(true);
            _scalarChannel2Action.getEnabledAction().setChecked(true);
            _scalarChannel3Action.getEnabledAction().setChecked(true);

            _scalarChannel1Action.setText("Hue");
            _scalarChannel2Action.setText("Saturation");
            _scalarChannel3Action.setText("Lightness");

            _colorMapAction.setEnabled(false);

            break;
        }

        case ColorSpaceType::LAB:
        {
            _scalarChannel1Action.getEnabledAction().setChecked(true);
            _scalarChannel2Action.getEnabledAction().setChecked(true);
            _scalarChannel3Action.getEnabledAction().setChecked(true);

            _scalarChannel1Action.setText("L");
            _scalarChannel2Action.setText("A");
            _scalarChannel3Action.setText("B");

            _colorMapAction.setEnabled(false);

            break;
        }

        default:
            break;
    }
}
