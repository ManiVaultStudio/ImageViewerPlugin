#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"
#include "DataHierarchyItem.h"
#include "ImageProp.h"
#include "SelectionProp.h"
#include "SelectionToolProp.h"

#include "util/Exception.h"

#include <set>

using namespace hdps;

Layer::Layer(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName) :
    QObject(&imageViewerPlugin),
    Renderable(imageViewerPlugin.getImageViewerWidget()->getRenderer()),
    _imageViewerPlugin(imageViewerPlugin),
    _active(false),
    _images(datasetName),
    _points(_images->getHierarchyItem().getParent()->getDatasetName()),
    _selectedIndices(),
    _generalAction(*this),
    _imageAction(*this),
    _selectionAction(*this, _imageViewerPlugin.getImageViewerWidget(), _imageViewerPlugin.getImageViewerWidget()->getPixelSelectionTool()),
    _subsetAction(*this)
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

    // Do an initial computation of the selected indices
    computeSelectionIndices();

    // Update the color map image in the image prop
    const auto updateColorMap = [this]() {

        // Set the color map image in the prop
        this->getPropByName<ImageProp>("ImageProp")->setColorMapImage(_imageAction.getColorMapAction().getColorMapImage());

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
        }

        // Render
        invalidate();
    };

    // Update the interpolation type in the image prop
    const auto updateInterpolationType = [this]() {

        // Assign the scalar data to the prop
        this->getPropByName<ImageProp>("ImageProp")->setInterpolationType(static_cast<InterpolationType>(_imageAction.getInterpolationTypeAction().getCurrentIndex()));

        // Render
        invalidate();
    };

    // Update the image prop
    const auto updateProp = [this]() {
        invalidate();
    };

    connect(&_generalAction.getVisibleAction(), &ToggleAction::toggled, this, updateProp);
    connect(&_imageAction.getColorMapAction(), &ColorMapAction::imageChanged, this, updateColorMap);
    connect(&_imageAction, &ImageAction::channelChanged, this, updateChannelScalarData);
    connect(&_imageAction.getInterpolationTypeAction(), &OptionAction::currentIndexChanged, this, updateInterpolationType);
    
    // Update prop when selection overlay color and opacity change
    connect(&_selectionAction.getOverlayColor(), &ColorAction::colorChanged, this, updateProp);
    connect(&_selectionAction.getOverlayOpacity(), &DecimalAction::valueChanged, this, updateProp);

    // Update the model matrix and re-render
    const auto updateModelMatrixAndReRender = [this]() {
        updateModelMatrix();
        invalidate();
    };

    // Update model matrix when layer scale changes
    connect(&_generalAction.getScaleAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);

    // Update model matrix when layer position changes
    connect(&_generalAction.getXPositionAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);
    connect(&_generalAction.getYPositionAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);

    updateColorMap();
    updateChannelScalarData(_imageAction.getChannel1Action());
    updateChannelScalarData(_imageAction.getChannelSelectionAction());
    updateInterpolationType();
    updateModelMatrixAndReRender();

    // Update the window title when the layer name changes
    connect(&_generalAction.getNameAction(), &StringAction::stringChanged, this, &Layer::updateWindowTitle);
}

Layer::~Layer()
{
    qDebug() << "Delete layer" << _generalAction.getNameAction().getString();
}

void Layer::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    try {

        // Don't render if invisible
        if (!_generalAction.getVisibleAction().isChecked())
            return;

        // Render props
        for (auto prop : _props)
            prop->render(modelViewProjectionMatrix);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to render layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to render layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::updateWindowTitle()
{
    try {

        qDebug() << "Update the window title for layer:" << _generalAction.getNameAction().getString();

        // Get layer name
        const auto name = _generalAction.getNameAction().getString();

        // Update the window title
        _imageViewerPlugin.setWindowTitle(QString("%1%2").arg(_imageViewerPlugin.getGuiName(), _active ? QString(": %1").arg(name) : ""));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to update the window title for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to update the window title for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

LayersAction& Layer::getLayersAction()
{
    return _imageViewerPlugin.getSettingsAction().getLayersAction();
}

ImageViewerPlugin& Layer::getImageViewerPlugin()
{
    return _imageViewerPlugin;
}

QMenu* Layer::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(_generalAction.getNameAction().getString(), parent);

    menu->addAction(&_generalAction.getVisibleAction());
    menu->addAction(&_imageAction.getOpacityAction());

    return menu;
}

void Layer::activate()
{
    try {

        qDebug() << "Activate layer:" << _generalAction.getNameAction().getString();

        // Set active
        _active = true;

        // Enable shortcuts for the layer
        _selectionAction.setShortcutsEnabled(true);

        // Enable the pixel selection tool
        _selectionAction.getPixelSelectionTool().setEnabled(true);

        // Update the view plugin window tile
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to activate layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to activate layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::deactivate()
{
    try {

        qDebug() << "Deactivate layer:" << _generalAction.getNameAction().getString();

        // Set active
        _active = false;

        // Disable shortcuts for the layer
        _selectionAction.setShortcutsEnabled(false);

        // Disable the pixel selection tool
        _selectionAction.getPixelSelectionTool().setEnabled(false);

        // Update the view plugin window tile
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to deactivate layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to deactivate layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::invalidate()
{
    _imageViewerPlugin.getImageViewerWidget()->update();
}

void Layer::updateModelMatrix()
{
    qDebug() << "Update model matrix for layer:" << _generalAction.getNameAction().getString();

    try {

        // Get reference to general action for getting the layer position and scale
        auto& generalAction = _generalAction;

        QMatrix4x4 translateMatrix, scaleMatrix;

        // Compute the translation matrix
        translateMatrix.translate(generalAction.getXPositionAction().getValue(), generalAction.getYPositionAction().getValue(), 0.0f);

        // Get the scale factor
        const auto scaleFactor = 0.01f * generalAction.getScaleAction().getValue();

        // And compute the scale factor
        scaleMatrix.scale(scaleFactor, scaleFactor, scaleFactor);

        // Assign model matrix
        setModelMatrix(translateMatrix * scaleMatrix);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to update the model matrix for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to update the model matrix for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

const QString Layer::getImagesDatasetName() const
{
    if (!_images.isValid())
        return "";

    return _images.getDatasetName();
}

const std::uint32_t Layer::getNumberOfImages() const
{
    if (!_images.isValid())
        return 0;

    return _images->getNumberOfImages();
}

const QSize Layer::getImageSize() const
{
    if (!_images.isValid())
        return QSize();

    return _images->getImageSize();
}

const QStringList Layer::getDimensionNames() const
{
    if (!_images.isValid() || !_points.isValid())
        return QStringList();

    QStringList dimensionNames;

    if (_points->getDimensionNames().size() == _points->getNumDimensions()) {
        for (const auto& dimensionName : _points->getDimensionNames())
            dimensionNames << dimensionName;
    }
    else {
        for (std::uint32_t dimensionIndex = 0; dimensionIndex < _points->getNumDimensions(); dimensionIndex++)
            dimensionNames << QString("Dim %1").arg(QString::number(dimensionIndex));
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

        qDebug() << "Start the pixel selection for layer:" << _generalAction.getNameAction().getString();;

        // Compute the selection in the selection tool prop
        this->getPropByName<SelectionToolProp>("SelectionToolProp")->resetOffScreenSelectionBuffer();

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to start the layer pixel selection for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to start the layer pixel selection for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::computeSelection(const QVector<QPoint>& mousePositions)
{
    try {

        qDebug() << "Compute the pixel selection for layer:" << _generalAction.getNameAction().getString();;

        // Compute the selection in the selection tool prop
        this->getPropByName<SelectionToolProp>("SelectionToolProp")->compute(mousePositions);

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to compute layer selection for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to compute layer selection for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::resetSelectionBuffer()
{
    try {

        qDebug() << "Reset the selection buffer for layer:" << _generalAction.getNameAction().getString();

        // Reset the off-screen selection buffer
        getPropByName<SelectionToolProp>("SelectionToolProp")->resetOffScreenSelectionBuffer();

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to reset the off-screen selection buffer for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to reset the off-screen selection buffer for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::publishSelection()
{
    //return;
    try {

        //qDebug() << "Publish pixel selection for layer:" << _generalAction.getNameAction().getString();

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
                // Get current selection image
                const auto selectionImage   = getPropByName<SelectionToolProp>("SelectionToolProp")->getSelectionImage().mirrored(true, true);
                const auto noComponents     = 4;
                const auto width            = static_cast<float>(getImageSize().width());
                const auto noPixels         = _images->getNumberOfPixels();
                const auto sourceRectangle  = _images->getSourceRectangle();
                const auto targetRectangle  = _images->getTargetRectangle();

                // Get source pixel index from two-dimensional pixel coordinates
                const auto getSourcePixelIndex = [sourceRectangle] (const std::int32_t& pixelX, const std::int32_t& pixelY) -> std::int32_t {
                    return pixelY * sourceRectangle.width() + pixelX;
                };

                // Get target pixel index from two-dimensional pixel coordinates
                const auto getTargetPixelIndex = [targetRectangle] (const std::int32_t& pixelX, const std::int32_t& pixelY) -> std::int32_t {
                    return (pixelY - targetRectangle.top()) * targetRectangle.width() + (pixelX - targetRectangle.left());
                };

                // Establish new selection indices depending on the type of modifier
                switch (pixelSelectionTool.getModifier())
                {
                    // Replace pixels with new selection
                    case PixelSelectionModifierType::Replace:
                    {
                        selectionIndices.clear();
                        selectionIndices.reserve(noPixels);

                        // Loop over all the pixels in the selection image in row-column order and add to the selection indices if the pixel is non-zero
                        for (std::int32_t pixelY = targetRectangle.top(); pixelY <= targetRectangle.bottom(); pixelY++) {
                            for (std::int32_t pixelX = targetRectangle.left(); pixelX <= targetRectangle.right(); pixelX++) {
                                if (selectionImage.bits()[getTargetPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                    selectionIndices.push_back(getSourcePixelIndex(pixelX, pixelY));
                            }
                        }

                        break;
                    }

                    // Add pixels to current selection
                    case PixelSelectionModifierType::Add:
                    {
                        // Create selection set of current selection indices
                        auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                        // Loop over all the pixels in the selection image in row-column order and insert the selection index into the set if the pixel is non-zero
                        for (std::int32_t pixelY = targetRectangle.top(); pixelY <= targetRectangle.bottom(); pixelY++) {
                            for (std::int32_t pixelX = targetRectangle.left(); pixelX <= targetRectangle.right(); pixelX++) {
                                if (selectionImage.bits()[getTargetPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                    selectionSet.insert(getSourcePixelIndex(pixelX, pixelY));
                            }
                        }

                        // Convert the set back to a vector
                        selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                        break;
                    }

                    // Remove pixels from current selection
                    case PixelSelectionModifierType::Remove:
                    {
                        // Create selection set of current selection indices
                        auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                        // Loop over all the pixels in the selection image in row-column order and remove the selection index from the set if the pixel is non-zero
                        for (std::int32_t pixelY = targetRectangle.top(); pixelY <= targetRectangle.bottom(); pixelY++) {
                            for (std::int32_t pixelX = targetRectangle.left(); pixelX <= targetRectangle.right(); pixelX++) {
                                if (selectionImage.bits()[getTargetPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                    selectionSet.erase(getSourcePixelIndex(pixelX, pixelY));
                            }
                        }

                        // Convert the set back to a vector
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
        Application::core()->notifySelectionChanged(_points.getSourceData().getName());

        // Render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to publish selection change for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to publish selection change for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::computeSelectionIndices()
{
    try {

        // Get selection indices from points dataset
        auto& selectionIndices = dynamic_cast<Points&>(getPoints().getSourceData().getSelection()).indices;

        if (getPoints()->isFull()) {

            // Since the dataset is full, we can copy the indices without any intermediate steps
            _selectedIndices = selectionIndices;
        }
        else {

            // Clear the selected
            _selectedIndices.clear();
            _selectedIndices.reserve(_images->getNumberOfPixels());

            const auto sourceImageWidth = _images->getSourceRectangle().width();
            const auto targetImageWidth = _images->getTargetRectangle().width();

            // Iterate over all candidate selection indices
            for (const auto& selectionIndex : selectionIndices) {

                // Compute source pixel coordinate
                const auto sourcePixelCoordinate = QPoint(selectionIndex % sourceImageWidth, static_cast<std::int32_t>(floorf(selectionIndex / static_cast<float>(sourceImageWidth))));

                // Move to next selection index if the the selected pixel is beyond the target boundaries
                if (!_images->getTargetRectangle().contains(sourcePixelCoordinate))
                    continue;

                // The selection index is valid so we can compute the target pixel coordinate and index
                const auto targetPixelCoordinate    = sourcePixelCoordinate - _images->getTargetRectangle().topLeft();
                const auto targetPixelIndex         = targetPixelCoordinate.y() * targetImageWidth + targetPixelCoordinate.x();

                // Except in case of invalid target pixel index
                if (static_cast<std::uint32_t>(targetPixelIndex) >= _images->getNumberOfPixels())
                    throw std::runtime_error("Invalid pixel index");

                // And add the target pixel index to the list of selected pixels
                _selectedIndices.push_back(targetPixelIndex);
            }
        }

        // Get selection channel
        auto& selectionChannel = _imageAction.getChannelSelectionAction();

        // Recompute the scalar data of the selection channel
        _imageAction.getChannelSelectionAction().computeScalarData();

        // Assign the scalar data to the prop
        this->getPropByName<SelectionProp>("SelectionProp")->setSelectionData(selectionChannel.getSelectionData());

        // Notify others that the selection changed
        emit selectionChanged(_selectedIndices);

        // Render layer
        invalidate();
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Unable to compute selected pixels for layer %1: %2").arg(_generalAction.getNameAction().getString(), e.what());
    }
    catch (...) {
        qDebug() << QString("Unable to compute selected pixels for layer %1 due to an unhandled exception").arg(_generalAction.getNameAction().getString());
    }
}

std::vector<std::uint32_t>& Layer::getSelectedIndices()
{
    if (!_points.isValid())
        throw std::runtime_error("The points dataset is not valid");

    return _selectedIndices;
}

const std::vector<std::uint32_t>& Layer::getSelectedIndices() const
{
    return const_cast<Layer*>(this)->getSelectedIndices();
}

void Layer::zoomToExtents()
{
    try {

        qDebug() << "Zoom to the extents of layer:" << _generalAction.getNameAction().getString();

        // Get pointer to image prop
        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        // Zoom to layer extents
        _imageViewerPlugin.getImageViewerWidget()->getRenderer().zoomToObject(*this);

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to zoom to extents of layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to zoom to extents of layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::zoomToSelection()
{
    try {

        qDebug() << "Zoom to the pixel selection of layer:" << _generalAction.getNameAction().getString();;

        // Get pointer to image prop
        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        // Get selection boundaries
        const auto selectionBoundingRectangle = QRectF(_imageAction.getChannelSelectionAction().getSelectionBoundaries());

        // Ensure selection boundaries are valid
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

        // Compute the zoom rectangle in world coordinates
        const auto zoomRectangleWorld = rectangleFromPoints(worldTopLeft, worldBottomRight);

        // Zoom to layer selection
        _imageViewerPlugin.getImageViewerWidget()->getRenderer().zoomToWorldRectangle(zoomRectangleWorld);

        // Trigger render
        invalidate();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to zoom to the pixel selection for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to zoom to the pixel selection for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

QRectF Layer::getWorldBoundingRectangle() const
{
    return getPropByName<ImageProp>("ImageProp")->getWorldBoundingRectangle();
}
