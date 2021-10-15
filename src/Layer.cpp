#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"
#include "DataHierarchyItem.h"
#include "ImageProp.h"
#include "SelectionProp.h"
#include "SelectionToolProp.h"

#include "util/Exception.h"

#include "PointData.h"
#include "ClusterData.h"

#include <set>

using namespace hdps;

Layer::Layer(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName) :
    QObject(&imageViewerPlugin),
    Renderable(imageViewerPlugin.getImageViewerWidget().getRenderer()),
    _imageViewerPlugin(imageViewerPlugin),
    _active(false),
    _imagesDataset(datasetName),
    _sourceDataset(_imagesDataset->getHierarchyItem().getParent()->getDatasetName()),
    _selectedIndices(),
    _generalAction(*this),
    _imageAction(*this),
    _selectionAction(*this, &_imageViewerPlugin.getImageViewerWidget(), _imageViewerPlugin.getImageViewerWidget().getPixelSelectionTool()),
    _selectionData(),
    _selectionBoundaries()
{
    if (!_sourceDataset.isValid())
        throw std::runtime_error("The layer source dataset is not valid after initialization");

    if (!_imagesDataset.isValid())
        throw std::runtime_error("The layer images dataset is not valid after initialization");

    // Resize selection data with number of pixels
    _selectionData.resize(_imagesDataset->getNumberOfPixels());

    // Create layer render props
    _props << new ImageProp(*this, "ImageProp");
    _props << new SelectionProp(*this, "SelectionProp");
    _props << new SelectionToolProp(*this, "SelectionToolProp");

    this->getPropByName<ImageProp>("ImageProp")->setGeometry(_imagesDataset->getSourceRectangle(), _imagesDataset->getTargetRectangle());
    this->getPropByName<SelectionProp>("SelectionProp")->setGeometry(_imagesDataset->getSourceRectangle(), _imagesDataset->getTargetRectangle());
    this->getPropByName<SelectionToolProp>("SelectionToolProp")->setGeometry(_imagesDataset->getSourceRectangle(), _imagesDataset->getTargetRectangle());

    // Do an initial computation of the selected indices
    computeSelectionIndices();

    // Update the color map scalar data in the image prop
    const auto updateChannelScalarData = [this](ScalarChannelAction& channelAction) {

        switch (channelAction.getIdentifier()) {
            case ScalarChannelAction::Channel1:
            case ScalarChannelAction::Channel2:
            case ScalarChannelAction::Channel3:
            case ScalarChannelAction::Mask:
            {
                // Assign color data to the prop in case of points dataset
                this->getPropByName<ImageProp>("ImageProp")->setChannelScalarData(channelAction.getIdentifier(), channelAction.getScalarData(), channelAction.getDisplayRange());

                break;
            }
        }

        // Render
        invalidate();
    };

    // Update the interpolation type in the image prop
    const auto updateInterpolationType = [this]() {

        // Get pointer to image prop
        auto imageProp = this->getPropByName<ImageProp>("ImageProp");

        // Assign the scalar data to the prop
        imageProp->setInterpolationType(static_cast<InterpolationType>(_imageAction.getInterpolationTypeAction().getCurrentIndex()));
        

        // Render
        invalidate();
    };

    connect(&_generalAction.getVisibleAction(), &ToggleAction::toggled, this, &Layer::invalidate);
    connect(&_imageAction, &ImageAction::channelChanged, this, updateChannelScalarData);
    connect(&_imageAction.getInterpolationTypeAction(), &OptionAction::currentIndexChanged, this, updateInterpolationType);
    
    // Update prop when selection overlay color and opacity change
    connect(&_selectionAction.getOverlayColor(), &ColorAction::colorChanged, this, &Layer::invalidate);
    connect(&_selectionAction.getOverlayOpacity(), &DecimalAction::valueChanged, this, &Layer::invalidate);

    // Update the model matrix and re-render
    const auto updateModelMatrixAndReRender = [this]() {
        updateModelMatrix();
        invalidate();
    };

    connect(&_generalAction.getScaleAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);
    connect(&_generalAction.getXPositionAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);
    connect(&_generalAction.getYPositionAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);

    updateChannelScalarData(_imageAction.getScalarChannel1Action());
    updateInterpolationType();
    updateModelMatrixAndReRender();

    // Update the window title when the layer name changes
    connect(&_generalAction.getNameAction(), &StringAction::stringChanged, this, &Layer::updateWindowTitle);

    _imageAction.init();
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

void Layer::setColorMapImage(const QImage& colorMapImage, const InterpolationType& interpolationType)
{
    // Get pointer to image prop
    auto imageProp = this->getPropByName<ImageProp>("ImageProp");

    // Set the color map image and interpolation type in the image prop
    imageProp->setColorMapImage(colorMapImage);
    imageProp->setColorMapInterpolationType(interpolationType);

    // Render
    invalidate();
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
    _imageViewerPlugin.getImageViewerWidget().update();
}

const QString Layer::getImagesDatasetName() const
{
    return _imagesDataset.getDatasetName();
}

const std::uint32_t Layer::getNumberOfImages() const
{
    if (!_imagesDataset.isValid())
        return 0;

    return _imagesDataset->getNumberOfImages();
}

const QSize Layer::getImageSize() const
{
    if (!_imagesDataset.isValid())
        return QSize();

    return _imagesDataset->getImageSize();
}

const QStringList Layer::getDimensionNames() const
{
    if (!_imagesDataset.isValid() || !_sourceDataset.isValid())
        return QStringList();

    QStringList dimensionNames;

    if (_sourceDataset->getDataType() == PointType) {

        // Get reference to points dataset
        auto points = getSourceDataset<Points>();

        // Populate dimension names
        if (points->getDimensionNames().size() == points->getNumDimensions()) {
            for (const auto& dimensionName : points->getDimensionNames())
                dimensionNames << dimensionName;
        }
        else {
            for (std::uint32_t dimensionIndex = 0; dimensionIndex < points->getNumDimensions(); dimensionIndex++)
                dimensionNames << QString("Dim %1").arg(QString::number(dimensionIndex));
        }
    }

    if (_sourceDataset->getDataType() == ClusterType) {
        dimensionNames << "Cluster index";
    }

    return dimensionNames;
}

void Layer::selectAll()
{
    if (_sourceDataset->getDataType() == PointType)
        getSourceDataset<Points>()->selectAll();
}

void Layer::selectNone()
{
    if (_sourceDataset->getDataType() == PointType)
        getSourceDataset<Points>()->selectNone();
}

void Layer::selectInvert()
{
    if (_sourceDataset->getDataType() == PointType)
        getSourceDataset<Points>()->selectInvert();
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
        if (!_sourceDataset.isValid())
            throw std::runtime_error("The layer points dataset is not valid after initialization");

        // Make sure we have a valid images dataset
        if (!_imagesDataset.isValid())
            throw std::runtime_error("The layer images dataset is not valid after initialization");

        // Get reference to the pixel selection tool
        auto& pixelSelectionTool = getImageViewerPlugin().getImageViewerWidget().getPixelSelectionTool();

        // Get current selection image
        const auto selectionImage   = getPropByName<SelectionToolProp>("SelectionToolProp")->getSelectionImage().mirrored(true, true);
        const auto noComponents     = 4;
        const auto width            = static_cast<float>(getImageSize().width());
        const auto noPixels         = _imagesDataset->getNumberOfPixels();
        const auto sourceRectangle  = _imagesDataset->getSourceRectangle();
        const auto targetRectangle  = _imagesDataset->getTargetRectangle();

        // Get source pixel index from two-dimensional pixel coordinates
        const auto getSourcePixelIndex = [sourceRectangle](const std::int32_t& pixelX, const std::int32_t& pixelY) -> std::int32_t {
            return pixelY * sourceRectangle.width() + pixelX;
        };

        // Get target pixel index from two-dimensional pixel coordinates
        const auto getTargetPixelIndex = [targetRectangle](const std::int32_t& pixelX, const std::int32_t& pixelY) -> std::int32_t {
            return (pixelY - targetRectangle.top()) * targetRectangle.width() + (pixelX - targetRectangle.left());
        };

        if (_sourceDataset->getDataType() == PointType) {
            
            // Get reference to points selection indices
            std::vector<std::uint32_t>& selectionIndices = _sourceDataset->getSelection<Points>().indices;

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
        }

        if (_sourceDataset->getDataType() == ClusterType) {

            // Get reference to clusters selection indices
            std::vector<std::uint32_t>& selectionIndices = _sourceDataset->getSelection<Clusters>().indices;

            // Convert floating point scalars to unsigned integer scalars
            std::vector<std::uint32_t> integerScalarData(_imageAction.getScalarChannel1Action().getScalarData().begin(), _imageAction.getScalarChannel1Action().getScalarData().end());

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
                                selectionIndices.push_back(integerScalarData[getSourcePixelIndex(pixelX, pixelY)]);
                        }
                    }

                    // Convert selection indices vector to set to remove duplicates
                    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

                    // Convert the set back to a vector
                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());

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
                                selectionSet.insert(integerScalarData[getSourcePixelIndex(pixelX, pixelY)]);
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
                                selectionSet.erase(integerScalarData[getSourcePixelIndex(pixelX, pixelY)]);
                        }
                    }

                    // Convert the set back to a vector
                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());

                    break;
                }

                default:
                    break;
            }
        }

        // Notify listeners of the selection change
        Application::core()->notifySelectionChanged(_sourceDataset.getSourceData().getName());

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

        // Get selection image, selected indices and selection boundaries from the image dataset
        _imagesDataset->getSelectionData(_selectionData, _selectedIndices, _selectionBoundaries);

        // Assign the scalar data to the prop
        this->getPropByName<SelectionProp>("SelectionProp")->setSelectionData(_selectionData);

        // Notify others that the selection changed
        emit selectionChanged(_selectedIndices);

        // Render layer
        invalidate();
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Unable to compute selected indices for layer %1: %2").arg(_generalAction.getNameAction().getString(), e.what());
    }
    catch (...) {
        qDebug() << QString("Unable to compute selected indices for layer %1 due to an unhandled exception").arg(_generalAction.getNameAction().getString());
    }
}

std::vector<std::uint32_t>& Layer::getSelectedIndices()
{
    return _selectedIndices;
}

const std::vector<std::uint32_t>& Layer::getSelectedIndices() const
{
    return const_cast<Layer*>(this)->getSelectedIndices();
}

QRect Layer::getSelectionBoundaries() const
{
    return _selectionBoundaries;
}

void Layer::zoomToExtents()
{
    try {

        qDebug() << "Zoom to the extents of layer:" << _generalAction.getNameAction().getString();

        // Get pointer to image prop
        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        // Zoom to layer extents
        _imageViewerPlugin.getImageViewerWidget().getRenderer().setZoomRectangle(getWorldBoundingRectangle());

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

        // Get target rectangle (needed for image offset)
        const auto targetRectangle = _imagesDataset->getTargetRectangle();

        // Add the target image offset
        _selectionBoundaries.translate(targetRectangle.topLeft());

        // Ensure selection boundaries are valid
        if (!_selectionBoundaries.isValid())
            throw std::runtime_error("Selection boundaries are invalid");

        auto rectangle = layerImageProp->getWorldBoundingRectangle();

        // Compute composite matrix
        const auto matrix = getModelMatrix() * layerImageProp->getModelMatrix();

        // Compute rectangle extents in world coordinates
        const auto worldTopLeft     = matrix * _selectionBoundaries.topLeft();
        const auto worldBottomRight = matrix * _selectionBoundaries.bottomRight();

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
        _imageViewerPlugin.getImageViewerWidget().getRenderer().setZoomRectangle(zoomRectangleWorld);

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
