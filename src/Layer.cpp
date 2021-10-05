#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"
#include "DataHierarchyItem.h"
#include "ImageProp.h"
#include "SelectionProp.h"
#include "SelectionToolProp.h"

#include "util/Exception.h"

#include <set>

Layer::Layer(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName) :
    Renderable(imageViewerPlugin.getImageViewerWidget()->getRenderer()),
    _imageViewerPlugin(imageViewerPlugin),
    _images(datasetName),
    _points(_images->getHierarchyItem().getParent()->getDatasetName()),
    _layerAction(*this, imageViewerPlugin.getSettingsAction().getLayersAction())
{
    if (!_images.isValid())
        throw std::runtime_error("The layer images dataset is not valid after initialization");

    if (!_points.isValid())
        throw std::runtime_error("The layer points dataset is not valid after initialization");

    // Create layer render props
    _props << new ImageProp(*this, "ImageProp");
    _props << new SelectionProp(*this, "SelectionProp");
    _props << new SelectionToolProp(*this, "SelectionToolProp");

    this->getPropByName<ImageProp>("ImageProp")->setGeometry(_images->getSourceRectangle(), _images->getTargetRectangle());
    this->getPropByName<SelectionProp>("SelectionProp")->setGeometry(_images->getSourceRectangle(), _images->getTargetRectangle());
    this->getPropByName<SelectionToolProp>("SelectionToolProp")->setGeometry(_images->getSourceRectangle(), _images->getTargetRectangle());

    // Update the color map image in the image prop
    const auto updateColorMap = [this]() {

        // Set the color map image in the prop
        this->getPropByName<ImageProp>("ImageProp")->setColorMapImage(_layerAction.getImageAction().getColorMapAction().getColorMapImage());

        // Render
        invalidate();
    };

    // Update the color map scalar data in the image prop
    const auto updateChannelScalarData = [this](ChannelAction& channelAction) {

        switch (channelAction.getIndex()) {
            case ChannelAction::Channel1:
            case ChannelAction::Channel2:
            case ChannelAction::Channel3:
            case ChannelAction::Mask:
            {
                // Assign the scalar data to the prop
                this->getPropByName<ImageProp>("ImageProp")->setChannelScalarData(channelAction.getIndex(), channelAction.getImageSize(), channelAction.getScalarData(), channelAction.getDisplayRange());

                break;
            }

            case ChannelAction::Selection:
            {
                // Get selection channel
                auto& selectionChannel = _layerAction.getImageAction().getChannelSelectionAction();

                // Assign the scalar data to the prop
                this->getPropByName<SelectionProp>("SelectionProp")->setSelectionData(channelAction.getImageSize(), selectionChannel.getSelectionData());

                break;
            }
        }

        // Render
        invalidate();
    };

    // Update the interpolation type in the image prop
    const auto updateInterpolationType = [this]() {

        // Assign the scalar data to the prop
        this->getPropByName<ImageProp>("ImageProp")->setInterpolationType(static_cast<InterpolationType>(_layerAction.getImageAction().getInterpolationTypeAction().getCurrentIndex()));

        // Render
        invalidate();
    };

    // Update the image prop
    const auto updateProp = [this]() {
        invalidate();
    };

    connect(&_layerAction.getGeneralAction().getVisibleAction(), &ToggleAction::toggled, this, updateProp);
    connect(&_layerAction.getImageAction().getColorMapAction(), &ColorMapAction::imageChanged, this, updateColorMap);
    connect(&_layerAction.getImageAction(), &ImageAction::channelChanged, this, updateChannelScalarData);
    connect(&_layerAction.getImageAction().getInterpolationTypeAction(), &OptionAction::currentIndexChanged, this, updateInterpolationType);
    
    auto& selectionAction = _layerAction.getSelectionAction();

    // Update prop when selection overlay color and opacity change
    connect(&selectionAction.getOverlayColor(), &ColorAction::colorChanged, this, updateProp);
    connect(&selectionAction.getOverlayOpacity(), &DecimalAction::valueChanged, this, updateProp);

    // Update the model matrix and re-render
    const auto updateModelMatrixAndReRender = [this]() {
        updateModelMatrix();
        invalidate();
    };

    // Update model matrix when layer scale changes
    connect(&_layerAction.getGeneralAction().getScaleAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);

    // Update model matrix when layer position changes
    connect(&_layerAction.getGeneralAction().getXPositionAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);
    connect(&_layerAction.getGeneralAction().getYPositionAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);

    updateColorMap();
    updateChannelScalarData(_layerAction.getImageAction().getChannel1Action());
    updateChannelScalarData(_layerAction.getImageAction().getChannelSelectionAction());
    updateInterpolationType();
    updateModelMatrixAndReRender();
}

void Layer::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    // Don't render if invisible
    if (!_layerAction.getGeneralAction().getVisibleAction().isChecked())
        return;

    // Render props
    for (auto prop : _props)
        prop->render(modelViewProjectionMatrix);
}

ImageViewerPlugin& Layer::getImageViewerPlugin()
{
    return _imageViewerPlugin;
}

void Layer::invalidate()
{
    _imageViewerPlugin.getImageViewerWidget()->update();
}

void Layer::updateModelMatrix()
{
    QMatrix4x4 translateMatrix, scaleMatrix;
    
    auto& generalAction = _layerAction.getGeneralAction();

    // Compute the scale and translate matrices
    translateMatrix.translate(-generalAction.getXPositionAction().getValue(), generalAction.getYPositionAction().getValue(), 0.0f);
    scaleMatrix.scale(0.01f * generalAction.getScaleAction().getValue());

    // Assign model matrix
    setModelMatrix(translateMatrix * scaleMatrix);
}

const QString Layer::getImagesDatasetName() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images.getDatasetName();
}

std::vector<std::uint32_t>& Layer::getSelectionIndices()
{
    if (!_points.isValid())
        throw std::runtime_error("The points dataset is not valid");

    return dynamic_cast<Points&>(getPoints().getSourceData().getSelection()).indices;
}

const std::vector<std::uint32_t>& Layer::getSelectionIndices() const
{
    return const_cast<Layer*>(this)->getSelectionIndices();
}

const std::uint32_t Layer::getNumberOfImages() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images->getNumberOfImages();
}

const QSize Layer::getImageSize() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images->getImageSize();
}

const QStringList Layer::getDimensionNames() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data points from layer; the images/points dataset not valid");

    QStringList dimensionNames;

    if (_points->getDimensionNames().size() == _points->getNumDimensions()) {
        for (const auto& dimensionName : _points->getDimensionNames())
            dimensionNames << dimensionName;
    }
    else {
        for (const auto& dimensionName : _points->getDimensionNames())
            dimensionNames << QString("Dim %1").arg(QString::number(dimensionNames.count()));
    }

    return dimensionNames;
}

void Layer::selectAll()
{
    _points->selectAll();
}

void Layer::selectNone()
{
    _points->selectNone();
}

void Layer::selectInvert()
{
    _points->selectInvert();
}

void Layer::startSelection()
{
    try {

        qDebug() << "Start the layer pixel selection";

        // Compute the selection in the selection tool prop
        this->getPropByName<SelectionToolProp>("SelectionToolProp")->resetOffScreenSelectionBuffer();

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to start the layer pixel selection", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to start the layer pixel selection");
    }
}

void Layer::computeSelection(const QVector<QPoint>& mousePositions)
{
    try {

        qDebug() << "Compute layer selection";

        // Compute the selection in the selection tool prop
        this->getPropByName<SelectionToolProp>("SelectionToolProp")->compute(mousePositions);

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to compute layer selection", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to compute layer selection");
    }
}

void Layer::resetSelectionBuffer()
{
    try {

        getPropByName<SelectionToolProp>("SelectionToolProp")->resetOffScreenSelectionBuffer();

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to reset the off-screen selection buffer", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to reset the off-screen selection buffer");
    }
}

void Layer::publishSelection()
{
    try {

        qDebug() << "Publish layer selection";

        // Make sure we have a valid points dataset
        if (!_points.isValid())
            throw std::runtime_error("The layer points dataset is not valid after initialization");

        // Make sure we have a valid images dataset
        if (!_images.isValid())
            throw std::runtime_error("The layer images dataset is not valid after initialization");

        // Get reference to points selection indices
        auto& selectionIndices = _points->getSelection<Points>().indices;

        // Get reference to the pixel selection tool
        auto& pixelSelectionTool = getImageViewerPlugin().getImageViewerWidget()->getPixelSelectionTool();

        switch (pixelSelectionTool.getType())
        {
            case PixelSelectionType::Rectangle:
            case PixelSelectionType::Brush:
            case PixelSelectionType::Lasso:
            case PixelSelectionType::Polygon:
            case PixelSelectionType::Sample:
            {
                // Get current selection image (for add/subtract)
                const auto selectionImage = getPropByName<SelectionToolProp>("SelectionToolProp")->getSelectionImage().mirrored(false, true);

                const auto noComponents = 4;
                const auto width        = static_cast<float>(getImageSize().width());
                const auto noPixels     = _images->getNumberOfPixels();

                switch (pixelSelectionTool.getModifier())
                {
                    case PixelSelectionModifierType::Replace:
                    {
                        selectionIndices.clear();
                        selectionIndices.reserve(noPixels);

                        for (std::uint32_t pixelIndex = 0; pixelIndex < noPixels; ++pixelIndex) {
                            if (selectionImage.bits()[pixelIndex * noComponents] > 0)
                                selectionIndices.push_back(pixelIndex);
                        }

                        break;
                    }

                    case PixelSelectionModifierType::Add:
                    {
                        auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                        for (std::uint32_t pixelIndex = 0; pixelIndex < noPixels; ++pixelIndex) {
                            if (selectionImage.bits()[pixelIndex * noComponents] > 0) {
                                selectionSet.insert(pixelIndex);
                            }
                        }

                        selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                        break;
                    }

                    case PixelSelectionModifierType::Remove:
                    {
                        auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                        for (std::uint32_t pixelIndex = 0; pixelIndex < noPixels; ++pixelIndex) {
                            if (selectionImage.bits()[pixelIndex * noComponents] > 0) {
                                selectionSet.erase(pixelIndex);
                            }
                        }

                        selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                        break;
                    }

                    default:
                        break;
                }

                break;
            }

            default:
                break;
        }

        // Notify listeners of the selection change
        getImageViewerPlugin().core()->notifySelectionChanged(_points.getSourceData().getName());

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish selection change", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to publish selection change");
    }
}

void Layer::zoomToExtents()
{
    try {

        qDebug() << "Zoom to layer extents";

        // Get pointer to image layer prop
        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        // Zoom to layer extents
        _imageViewerPlugin.getImageViewerWidget()->getRenderer().zoomToObject(*this);

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to zoom to layer extents", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to zoom to layer extents");
    }
}

QRectF Layer::getWorldBoundingRectangle() const
{
    return getPropByName<ImageProp>("ImageProp")->getWorldBoundingRectangle();
}
