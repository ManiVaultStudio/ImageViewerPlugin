#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"
#include "DataHierarchyItem.h"
#include "ImageProp.h"
#include "SelectionProp.h"

#include "util/Exception.h"

Layer::Layer(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName) :
    Renderable(imageViewerPlugin.getImageViewerWidget()->getRenderer()),
    _imageViewerPlugin(imageViewerPlugin),
    _images(datasetName),
    _points(_images->getHierarchyItem().getParent()->getDatasetName()),
    _layerAction(*this)
{
    if (!_images.isValid())
        throw std::runtime_error("The layer images dataset is not valid after initialization");

    if (!_points.isValid())
        throw std::runtime_error("The layer points dataset is not valid after initialization");

    _props << new ImageProp(*this, "ImageProp");
    _props << new SelectionProp(*this, "SelectionProp");

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
                this->getPropByName<ImageProp>("ImageProp")->setChannelScalarData(channelAction.getIndex(), channelAction.getScalarData(), channelAction.getDisplayRange());

                break;
            }

            case ChannelAction::Selection:
            {
                // Get selection channel
                auto& selectionChannel = _layerAction.getImageAction().getChannelSelectionAction();

                // Assign the scalar data to the prop
                this->getPropByName<SelectionProp>("SelectionProp")->setSelectionData(selectionChannel.getSelectionData());

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
    
    auto& selectionAction = _imageViewerPlugin.getSettingsAction().getSelectionAction();

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

const std::uint32_t Layer::getNumberOfPoints() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data points from layer; the images/points dataset is not valid");

    return _points->getNumPoints();
}

const std::uint32_t Layer::getNumberOfDimensions() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data dimensions from layer; the images/points dataset not valid");

    return _points->getNumDimensions();
}

const QStringList Layer::getDimensionNames() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data points from layer; the images/points dataset not valid");

    QStringList dimensionNames;

    if (_points->getDimensionNames().size() == getNumberOfDimensions()) {
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
    /*
    auto& selectionIndices = getSelectionIndices();

    selectionIndices.clear();
    selectionIndices.resize(getNoPixels());

    if (_pointsDataset->isFull()) {
        std::iota(selectionIndices.begin(), selectionIndices.end(), 0);
    }
    else {
        for (const auto& index : _pointsDataset->indices)
            selectionIndices.push_back(index);
    }

    imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());
    */
}

void Layer::selectNone()
{
    /*
    auto& selectionIndices = getSelectionIndices();

    selectionIndices.clear();

    imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());
    */
}

void Layer::invertSelection()
{
    /*
    auto& selectionIndices = getSelectionIndices();

    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    const auto noPixels = getNoPixels();

    selectionIndices.clear();
    selectionIndices.reserve(noPixels - selectionSet.size());

    for (int i = 0; i < noPixels; i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());
    */
}

void Layer::zoomToExtents()
{
    try {
        
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
