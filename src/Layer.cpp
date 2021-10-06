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
    hdps::EventListener(),
    _imageViewerPlugin(imageViewerPlugin),
    _active(false),
    _images(datasetName),
    _points(_images->getHierarchyItem().getParent()->getDatasetName()),
    _layerAction(*this, imageViewerPlugin.getSettingsAction().getLayersAction()),
    _selectedPixels()
{
    setEventCore(hdps::Application::core());

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

    // Update selected pixels
    const auto updateSelectedPixels = [this]() {
        auto& selectionIndices = dynamic_cast<Points&>(getPoints().getSourceData().getSelection()).indices;

        if (getPoints()->isFull()) {
            _selectedPixels = selectionIndices;
        }
        else {
            QSet<std::uint32_t> indicesSet(getPoints()->indices.begin(), getPoints()->indices.end());

            _selectedPixels.clear();
            _selectedPixels.reserve(_images->getNumberOfPixels());

            // Cache source image width
            const auto sourceImageWidth = _images->getSourceRectangle().width();
            const auto targetImageWidth = _images->getImageSize().width();

            for (const auto& selectionIndex : selectionIndices) {
                if (indicesSet.contains(selectionIndex)) {
                   
                    // Convert global selection index to local pixel coordinate
                    const auto localPixelX = selectionIndex % sourceImageWidth;
                    const auto localPixelY = static_cast<std::int32_t>(floorf(selectionIndex / static_cast<float>(sourceImageWidth)));

                    _selectedPixels.push_back(localPixelY * targetImageWidth + localPixelX);
                }
            }
        }
    };

    // Update selected pixels when the selection changes
    registerDataEventByType(PointType, [this, updateSelectedPixels](hdps::DataEvent* dataEvent) {
        if (dataEvent->getType() == hdps::EventType::SelectionChanged) {
            auto selectionChangedEvent = static_cast<hdps::SelectionChangedEvent*>(dataEvent);

            if (selectionChangedEvent->dataSetName == _layerAction.getLayer().getPoints().getSourceData().getName())
                updateSelectedPixels();
        }
    });

    // Update the window title when the layer name changes
    connect(&_layerAction.getGeneralAction().getNameAction(), &StringAction::stringChanged, this, &Layer::updateWindowTitle);
}

Layer::~Layer()
{
    qDebug() << "Delete" << _layerAction.getGeneralAction().getNameAction().getString();
}

void Layer::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    try {

        // Don't render if invisible
        if (!_layerAction.getGeneralAction().getVisibleAction().isChecked())
            return;

        // Render props
        for (auto prop : _props)
            prop->render(modelViewProjectionMatrix);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to render layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to render layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::updateWindowTitle()
{
    try {

        qDebug() << "Update the window title for layer:" << _layerAction.getGeneralAction().getNameAction().getString();

        // Get layer name
        const auto name = getLayerAction().getGeneralAction().getNameAction().getString();

        // Update the window title
        _imageViewerPlugin.setWindowTitle(QString("%1%2").arg(_imageViewerPlugin.getGuiName(), _active ? QString(": %1").arg(name) : ""));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to update the window title for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to update the window title for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

ImageViewerPlugin& Layer::getImageViewerPlugin()
{
    return _imageViewerPlugin;
}

void Layer::activate()
{
    try {

        qDebug() << "Activate layer:" << _layerAction.getGeneralAction().getNameAction().getString();

        // Set active
        _active = true;

        // Enable shortcuts for the layer
        _layerAction.getSelectionAction().setShortcutsEnabled(true);

        // Update the view plugin window tile
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to activate layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to activate layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::deactivate()
{
    try {

        qDebug() << "Deactivate layer:" << _layerAction.getGeneralAction().getNameAction().getString();

        // Set active
        _active = false;

        // Disable shortcuts for the layer
        _layerAction.getSelectionAction().setShortcutsEnabled(false);

        // Update the view plugin window tile
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to deactivate layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to deactivate layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::invalidate()
{
    _imageViewerPlugin.getImageViewerWidget()->update();
}

void Layer::updateModelMatrix()
{
    qDebug() << "Update model matrix for layer:" << _layerAction.getGeneralAction().getNameAction().getString();

    try {

        // Get reference to general action for getting the layer position and scale
        auto& generalAction = _layerAction.getGeneralAction();

        QMatrix4x4 invertMatrix, translateMatrix, scaleMatrix;

        // Invert along the x-axis
        invertMatrix.scale(-1.0f, 1.0f, 1.0f);

        // Compute the translation matrix
        translateMatrix.translate(generalAction.getXPositionAction().getValue(), generalAction.getYPositionAction().getValue(), 0.0f);

        // Get the scale factor
        const auto scaleFactor = 0.01f * generalAction.getScaleAction().getValue();

        // And compute the scale factor
        scaleMatrix.scale(scaleFactor, scaleFactor, scaleFactor);

        // Assign model matrix
        setModelMatrix(invertMatrix * translateMatrix * scaleMatrix);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to update the model matrix for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to update the model matrix for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

const QString Layer::getImagesDatasetName() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images.getDatasetName();
}

std::vector<std::uint32_t>& Layer::getSelectedPixels()
{
    if (!_points.isValid())
        throw std::runtime_error("The points dataset is not valid");

    return _selectedPixels;
}

const std::vector<std::uint32_t>& Layer::getSelectedPixels() const
{
    return const_cast<Layer*>(this)->getSelectedPixels();
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

        qDebug() << "Start the pixel selection for layer:" << _layerAction.getGeneralAction().getNameAction().getString();;

        // Compute the selection in the selection tool prop
        this->getPropByName<SelectionToolProp>("SelectionToolProp")->resetOffScreenSelectionBuffer();

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to start the layer pixel selection for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to start the layer pixel selection for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::computeSelection(const QVector<QPoint>& mousePositions)
{
    try {

        qDebug() << "Compute the pixel selection for layer:" << _layerAction.getGeneralAction().getNameAction().getString();;

        // Compute the selection in the selection tool prop
        this->getPropByName<SelectionToolProp>("SelectionToolProp")->compute(mousePositions);

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to compute layer selection for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to compute layer selection for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::resetSelectionBuffer()
{
    try {

        qDebug() << "Reset the selection buffer for layer:" << _layerAction.getGeneralAction().getNameAction().getString();

        // Reset the off-screen selection buffer
        getPropByName<SelectionToolProp>("SelectionToolProp")->resetOffScreenSelectionBuffer();

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to reset the off-screen selection buffer for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to reset the off-screen selection buffer for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::publishSelection()
{
    try {

        qDebug() << "Publish pixel selection for layer:" << _layerAction.getGeneralAction().getNameAction().getString();;

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
        exceptionMessageBox(QString("Unable to publish selection change for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to publish selection change for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::zoomToExtents()
{
    try {

        qDebug() << "Zoom to the extents of layer:" << _layerAction.getGeneralAction().getNameAction().getString();

        // Get pointer to image prop
        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        // Zoom to layer extents
        _imageViewerPlugin.getImageViewerWidget()->getRenderer().zoomToObject(*this);

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to zoom to extents of layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to zoom to extents of layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

void Layer::zoomToSelection()
{
    try {

        qDebug() << "Zoom to the pixel selection of layer:" << _layerAction.getGeneralAction().getNameAction().getString();;

        // Get pointer to image prop
        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        // Get selection boundaries
        const auto selectionBoundingRectangle = QRectF(_layerAction.getImageAction().getChannelSelectionAction().getSelectionBoundaries());

        if (!selectionBoundingRectangle.isValid())
            throw std::runtime_error("Selection boundaries are invalid");

        auto rectangle = layerImageProp->getWorldBoundingRectangle();

        // Compute composite matrix
        const auto matrix = getModelMatrix() * layerImageProp->getModelMatrix();

        // Compute rectangle extents in world coordinates
        const auto worldTopLeft     = matrix * selectionBoundingRectangle.topLeft();
        const auto worldBottomRight = matrix * selectionBoundingRectangle.bottomRight();

        const auto rectangleFromPoints = [](const QPointF& first, const QPointF& second) -> QRectF {
            QRectF rectangle;

            rectangle.setLeft(std::min(first.x(), second.x()));
            rectangle.setRight(std::max(first.x(), second.x()));
            rectangle.setTop(std::min(first.y(), second.y()));
            rectangle.setBottom(std::max(first.y(), second.y()));

            return rectangle;
        };

        // Zoom to layer selection
        _imageViewerPlugin.getImageViewerWidget()->getRenderer().zoomToWorldRectangle(rectangleFromPoints(worldTopLeft, worldBottomRight));

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to zoom to the pixel selection for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to zoom to the pixel selection for layer: %1").arg(_layerAction.getGeneralAction().getNameAction().getString()));
    }
}

QRectF Layer::getWorldBoundingRectangle() const
{
    return getPropByName<ImageProp>("ImageProp")->getWorldBoundingRectangle();
}
