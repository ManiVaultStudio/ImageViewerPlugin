#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"
#include "DataHierarchyItem.h"
#include "ImageProp.h"
#include "SelectionProp.h"
#include "SelectionToolProp.h"
#include "LayersRenderer.h"

#include <util/Exception.h>
#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>

#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <QMenu>

#include <set>

using namespace hdps::gui;

Layer::Layer(QObject* parent, const QString& title) :
    GroupsAction(parent, title),
    Renderable(),
    _imageViewerPlugin(nullptr),
    _active(false),
    _imagesDataset(),
    _sourceDataset(),
    _selectedIndices(),
    _generalAction(this, "General"),
    _imageSettingsAction(this, "Image"),
    _selectionAction(this, "Selection"),
    _miscellaneousAction(this, "Miscellaneous"),
    _subsetAction(this, "Subset"),
    _selectionData(),
    _imageSelectionRectangle(),
    _maskData()
{
}

void Layer::initialize(ImageViewerPlugin* imageViewerPlugin, const hdps::Dataset<Images>& imagesDataset)
{
    Q_ASSERT(imageViewerPlugin != nullptr);

    if (imageViewerPlugin == nullptr)
        return;

    _imageViewerPlugin = imageViewerPlugin;

    setRenderer(&_imageViewerPlugin->getImageViewerWidget().getRenderer());
        
    _imagesDataset = imagesDataset;
    _sourceDataset = _imagesDataset->getDataHierarchyItem().getParent().getDataset();

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

    this->getPropByName<ImageProp>("ImageProp")->setGeometry(_imagesDataset->getRectangle());
    this->getPropByName<SelectionProp>("SelectionProp")->setGeometry(_imagesDataset->getRectangle());
    this->getPropByName<SelectionToolProp>("SelectionToolProp")->setGeometry(_imagesDataset->getRectangle());

    // Do an initial computation of the selected indices
    computeSelectionIndices();

    // Resize mask data vector
    _maskData.resize(_imagesDataset->getNumberOfPixels());

    // Update the color map scalar data in the image prop
    const auto updateChannelScalarData = [this](ScalarChannelAction& channelAction) {

        switch (channelAction.getIdentifier()) {
            case ScalarChannelAction::Channel1:
            case ScalarChannelAction::Channel2:
            case ScalarChannelAction::Channel3:
            {
                this->getPropByName<ImageProp>("ImageProp")->setChannelScalarData(channelAction.getIdentifier(), channelAction.getScalarData(), channelAction.getDisplayRange());
                break;
            }

            case ScalarChannelAction::Count:
                break;
        }

        // Render
        invalidate();
    };

    // Update the interpolation type in the image prop
    const auto updateInterpolationType = [this]() {

        // Get pointer to image prop
        auto imageProp = this->getPropByName<ImageProp>("ImageProp");

        // Assign the scalar data to the prop
        imageProp->setInterpolationType(static_cast<InterpolationType>(_imageSettingsAction.getInterpolationTypeAction().getCurrentIndex()));

        // Render
        invalidate();
    };

    connect(&_generalAction.getVisibleAction(), &ToggleAction::toggled, this, &Layer::invalidate);
    connect(&_imageSettingsAction, &ImageSettingsAction::channelChanged, this, updateChannelScalarData);
    connect(&_imageSettingsAction.getInterpolationTypeAction(), &OptionAction::currentIndexChanged, this, updateInterpolationType);
    
    // Update prop when selection overlay color and opacity change
    connect(&_selectionAction.getPixelSelectionAction().getOverlayColorAction(), &ColorAction::colorChanged, this, &Layer::invalidate);
    connect(&_selectionAction.getPixelSelectionAction().getOverlayOpacityAction(), &DecimalAction::valueChanged, this, &Layer::invalidate);

    // Update the model matrix and re-render
    const auto updateModelMatrixAndReRender = [this]() {
        updateModelMatrix();
        invalidate();
    };

    connect(&_generalAction.getScaleAction(), &DecimalAction::valueChanged, this, updateModelMatrixAndReRender);
    connect(&_generalAction.getPositionAction(), &PositionAction::changed, this, updateModelMatrixAndReRender);

    updateChannelScalarData(_imageSettingsAction.getScalarChannel1Action());
    updateChannelScalarData(_imageSettingsAction.getScalarChannel2Action());
    updateChannelScalarData(_imageSettingsAction.getScalarChannel3Action());
    updateInterpolationType();
    updateModelMatrixAndReRender();

    // Update the window title when the layer name changes
    connect(&_generalAction.getNameAction(), &StringAction::stringChanged, this, &Layer::updateWindowTitle);

    // Update dataset name action when the images dataset GUI name changes
    connect(&_imagesDataset, &Dataset<Points>::dataGuiNameChanged, this, [this](const QString& oldGuiName, const QString& newGuiName) {
        _generalAction.getDatasetNameAction().setString(newGuiName);
        _generalAction.getNameAction().setDefaultString(newGuiName);
    });

    const auto updateSelectionRoi = [this]() {
        computeSelection();
        publishSelection();
    };

    connect(&_imageViewerPlugin->getImageViewerWidget().getRenderer(), &LayersRenderer::zoomRectangleChanged, this, [this, updateSelectionRoi]() {
        updateRoi();

        if (!_active)
            return;

        if (_selectionAction.getPixelSelectionAction().getPixelSelectionTool()->getType() != PixelSelectionType::ROI)
            return;

        if (!_selectionAction.getPixelSelectionAction().getNotifyDuringSelectionAction().isChecked())
            return;

        updateSelectionRoi();
    });

    connect(&_imageViewerPlugin->getImageViewerWidget(), &ImageViewerWidget::navigationEnded, this, [this, updateSelectionRoi]() {
        if (!_active)
            return;

        if (_selectionAction.getPixelSelectionAction().getPixelSelectionTool()->getType() != PixelSelectionType::ROI)
            return;

        if (_selectionAction.getPixelSelectionAction().getNotifyDuringSelectionAction().isChecked())
            return;

        updateSelectionRoi();
    });

    // Update ROI selection when the pixel selection type changes to ROI
    connect(&_selectionAction.getPixelSelectionAction().getTypeAction(), &OptionAction::currentIndexChanged, this, [this, updateSelectionRoi](const std::int32_t& currentIndex) {
        if (currentIndex == static_cast<std::int32_t>(PixelSelectionType::ROI))
            updateSelectionRoi();
        else
            _imagesDataset->selectNone();
    });

    connect(&_miscellaneousAction.getRoiViewAction(), &DecimalRectangleAction::rectangleChanged, getRenderer(), [this](const QRectF& rectangle) -> void {
        if (rectangle == getRenderer()->getZoomRectangle())
            return;

        const auto animationEnabled = getRenderer()->getAnimationEnabled();

        getRenderer()->setAnimationEnabled(false);
        {
            getRenderer()->setZoomRectangle(rectangle);
        }
        getRenderer()->setAnimationEnabled(animationEnabled);
    });

    _generalAction.initialize(this);
    _imageSettingsAction.initialize(this);
    _selectionAction.initialize(this, &_imageViewerPlugin->getImageViewerWidget(), &_imageViewerPlugin->getImageViewerWidget().getPixelSelectionTool());

    _imagesDataset->getMaskData(_maskData);

    // Apply masking to props
    this->getPropByName<ImageProp>("ImageProp")->setMaskData(_maskData);
    this->getPropByName<SelectionProp>("SelectionProp")->setMaskData(_maskData);
}

Layer::~Layer()
{
#if _DEBUG
    qDebug() << "Delete layer" << _generalAction.getNameAction().getString();
#endif
}

void Layer::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    try {

        // Don't render if invisible
        if (!_generalAction.getVisibleAction().isChecked())
            return;

        // Render props
        for (auto prop : _props) {

            // Do not render the selection prop and selection tool prop in ROI selection mode
            if (prop->getName() == "SelectionProp" || prop->getName() == "SelectionToolProp")
                if (_selectionAction.getPixelSelectionAction().getTypeAction().getCurrentIndex() == static_cast<std::int16_t>(PixelSelectionType::ROI))
                    continue;

            // Render the prop
            prop->render(modelViewProjectionMatrix);
        }
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
#if _DEBUG
    qDebug() << "Update model matrix for layer:" << _generalAction.getNameAction().getString();
#endif

    try {

        // Get reference to general action for getting the layer position and scale
        auto& generalAction = _generalAction;

        QMatrix4x4 translateMatrix, scaleMatrix;

        // Compute the translation matrix
        translateMatrix.translate(generalAction.getPositionAction().getXAction().getValue(), generalAction.getPositionAction().getYAction().getValue(), 0.0f);

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

#if _DEBUG
        qDebug() << "Update the window title for layer:" << _generalAction.getNameAction().getString();
#endif

        // Get layer name
        const auto name = _generalAction.getNameAction().getString();

        // Update the window title
        _imageViewerPlugin->getWidget().setWindowTitle(QString("%1%2").arg(_imageViewerPlugin->getGuiName(), _active ? QString(": %1").arg(name) : ""));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to update the window title for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to update the window title for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::updateRoi()
{
    const auto modelViewMatrix      = getRenderer()->getViewMatrix() * getModelMatrix() *getPropByName<SelectionToolProp>("SelectionToolProp")->getModelMatrix();
    const auto roiTopLeft           = getRenderer()->getScreenPointToWorldPosition(modelViewMatrix, QPoint(0, getRenderer()->getParentWidgetSize().height()));
    const auto roiBottomRight       = getRenderer()->getScreenPointToWorldPosition(modelViewMatrix, QPoint(getRenderer()->getParentWidgetSize().width(), 0));
    const auto inputImageSize       = _imagesDataset->getImageSize();

    QRect imageRoi;

    imageRoi.setBottomLeft(QPoint(std::clamp(static_cast<int>(std::round(roiTopLeft.x())), 0, inputImageSize.width()), std::clamp(static_cast<int>(std::round(roiTopLeft.y())), 0, inputImageSize.height())));
    imageRoi.setTopRight(QPoint(std::clamp(static_cast<int>(std::round(roiBottomRight.x())), 0, inputImageSize.width()), std::clamp(static_cast<int>(std::round(roiBottomRight.y())), 0, inputImageSize.height())));

    _miscellaneousAction.getRoiLayerAction().setRectangle(imageRoi);
    _miscellaneousAction.getRoiViewAction().setRectangle(getRenderer()->getZoomRectangle());
}

QRectF Layer::getWorldBoundingRectangle() const
{
    // Compute composite matrix and rectangle extents in world coordinates
    const auto matrix           = getModelMatrix() * getPropByName<ImageProp>("ImageProp")->getModelMatrix();
    const auto visibleRectangle = QRectF(_imagesDataset->getVisibleRectangle());
    const auto worldTopLeft     = matrix * visibleRectangle.topLeft();
    const auto worldBottomRight = matrix * visibleRectangle.bottomRight();

    //qDebug() << _imagesDataset->getGuiName() << visibleRectangle;

    const auto rectangleFromPoints = [](const QPointF& first, const QPointF& second) -> QRectF {
        QRectF rectangle;

        rectangle.setLeft(std::min(first.x(), second.x()));
        rectangle.setRight(std::max(first.x(), second.x()));
        rectangle.setTop(std::min(first.y(), second.y()));
        rectangle.setBottom(std::max(first.y(), second.y()));

        return rectangle;
    };

    return rectangleFromPoints(worldTopLeft, worldBottomRight);
}

QRectF Layer::getScreenBoundingRectangle() const
{
    return getRenderer()->getScreenRectangleFromWorldRectangle(getWorldBoundingRectangle());
}

ImageViewerPlugin& Layer::getImageViewerPlugin()
{
    return *_imageViewerPlugin;
}

QMenu* Layer::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(_generalAction.getNameAction().getString(), parent);

    menu->addAction(&_generalAction.getVisibleAction());
    menu->addAction(&_imageSettingsAction.getOpacityAction());

    return menu;
}

void Layer::activate()
{
    try {

#if _DEBUG
        qDebug() << "Activate layer:" << _generalAction.getNameAction().getString();
#endif

        // Set active
        _active = true;

        // Enable shortcuts for the layer
        _selectionAction.getPixelSelectionAction().setShortcutsEnabled(true);

        // Enable the pixel selection tool
        _selectionAction.getPixelSelectionAction().getPixelSelectionTool()->setEnabled(true);

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

#if _DEBUG
        qDebug() << "Deactivate layer:" << _generalAction.getNameAction().getString();
#endif

        // Set active
        _active = false;

        // Disable shortcuts for the layer
        _selectionAction.getPixelSelectionAction().setShortcutsEnabled(false);

        // Disable the pixel selection tool
        _selectionAction.getPixelSelectionAction().getPixelSelectionTool()->setEnabled(false);

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

bool Layer::isActive() const
{
    return _active;
}

void Layer::invalidate()
{
    _imageViewerPlugin->getImageViewerWidget().update();
}

void Layer::scaleToFit(const QRectF& layersRectangle)
{
    // Only fit into valid rectangle
    if (!layersRectangle.isValid())
        return;

    // Get target rectangle center and size
    const auto rectangleCenter = layersRectangle.center();

    // Position layer at center
    _generalAction.getPositionAction().getXAction().setValue(rectangleCenter.x());
    _generalAction.getPositionAction().getYAction().setValue(rectangleCenter.y());

    // Compute composite matrix and fetch image size
    const auto matrix               = getModelMatrix() * getPropByName<ImageProp>("ImageProp")->getModelMatrix();
    const auto imageRectangleSize   = QSizeF(_imagesDataset->getRectangle().size());

    // Compute x- and y scale
    const auto scale = QVector2D(layersRectangle.width() / imageRectangleSize.width(), layersRectangle.height() / imageRectangleSize.height());
   
    // Assign scale
    _generalAction.getScaleAction().setValue(100.0f * std::min(scale.x(), scale.y()));
}

void Layer::paint(QPainter& painter, const PaintFlag& paintFlags)
{
    try {

        // Don't draw if the layer is invisible
        if (!_generalAction.getVisibleAction().isChecked())
            return;

        // Get image prop visible screen bounding rectangle
        const auto propRectangle = getScreenBoundingRectangle();

        // Get pixel selection color
        const auto pixelSelectionColor = _imageViewerPlugin->getImageViewerWidget().getPixelSelectionTool().getMainColor();

        // Draw layer bounds
        if (paintFlags & Layer::Bounds) {

            // Configure pen and brush
            painter.setPen(QPen(QBrush(_generalAction.getColorAction().getColor()), _active ? 2.0f : 1.0f, _active ? Qt::SolidLine : Qt::DashLine));
            painter.setBrush(Qt::transparent);

            // Draw the bounding rectangle
            painter.drawRect(propRectangle);
        }

        // Draw layer selection rectangle (if not in ROI selection mode)
        if ((paintFlags & Layer::SelectionRectangle) && _imageSelectionRectangle.isValid() && _selectionAction.getPixelSelectionAction().getTypeAction().getCurrentIndex() != static_cast<std::int16_t>(PixelSelectionType::ROI)) {

            // Create perimeter pen
            auto perimeterPen = QPen(QBrush(_imageViewerPlugin->getImageViewerWidget().getPixelSelectionTool().getMainColor()), 0.7f);

            // Custom dash pattern
            perimeterPen.setDashPattern(QVector<qreal>{ 7, 7 });

            // Configure pen and brush
            painter.setPen(perimeterPen);
            painter.setBrush(Qt::transparent);

            // Draw the bounding rectangle
            painter.drawRect(getRenderer()->getScreenRectangleFromWorldRectangle(getWorldSelectionRectangle()));
        }

        // Draw layer label
        if ((paintFlags & Layer::Label) && _active) {

            // Establish label text
            const auto labelText = QString("%1").arg(_imagesDataset->getDataHierarchyItem().getFullPathName());

            // Configure pen and brush
            painter.setPen(QPen(QBrush(_generalAction.getColorAction().getColor()), _active ? 2.0f : 1.0f));
            painter.setBrush(Qt::transparent);

            // Upper margin in pixels
            const auto margin = 5;

            // Draw the text
            painter.setFont(QApplication::font());
            painter.drawText(QRectF(propRectangle.bottomLeft() + QPoint(0.0f, margin), QSize(500, 100)), labelText, QTextOption(Qt::AlignTop | Qt::AlignLeft));
        }

        // Draw sample information
        if (paintFlags & Layer::Sample) {

            // Get mouse position in widget coordinates
            const auto mousePositionWidget = _imageViewerPlugin->getImageViewerWidget().mapFromGlobal(QCursor::pos());

            // Get mouse position in image coordinates
            const auto mousePositionImage = _renderer->getScreenPointToWorldPosition(getModelViewMatrix() * getPropByName<ImageProp>("ImageProp")->getModelMatrix(), mousePositionWidget).toPoint();

            // Establish label prefix text
            QString labelText = QString("Pixel ID\t: [%1, %2]\n").arg(QString::number(mousePositionImage.x()), QString::number(mousePositionImage.y()));
            
            // Compute pixel index
            const auto pixelIndex = static_cast<std::uint32_t>(mousePositionImage.y() * _imagesDataset->getImageSize().width() + mousePositionImage.x());

            if (pixelIndex >= 0 && pixelIndex < _imagesDataset->getNumberOfPixels()) {

                // Show scalar(s) data if hovering over an image that originates from points data
                if (getSourceDataset()->getDataType() == PointType) {

                    if (_imageSettingsAction.getScalarChannel1Action().getEnabledAction().isChecked())
                        labelText += "Scalar 1\t: " + QString::number(_imageSettingsAction.getScalarChannel1Action().getScalarData()[pixelIndex], 'f', 2);

                    if (_imageSettingsAction.getScalarChannel2Action().getEnabledAction().isChecked())
                        labelText += "\nScalar 2\t: " + QString::number(_imageSettingsAction.getScalarChannel2Action().getScalarData()[pixelIndex], 'f', 2);

                    if (_imageSettingsAction.getScalarChannel3Action().getEnabledAction().isChecked())
                        labelText += "\nScalar 3\t: " + QString::number(_imageSettingsAction.getScalarChannel3Action().getScalarData()[pixelIndex], 'f', 2);
                }

                // Show cluster name if hovering over an image that originates from clusters data
                if (getSourceDataset()->getDataType() == ClusterType) {

                    // Get cluster index from channel scalar data
                    const auto clusterIndex = static_cast<std::int32_t>(_imageSettingsAction.getScalarChannel1Action().getScalarData()[pixelIndex]);

                    // Get cluster name from cluster index
                    const auto clusterName = _sourceDataset.get<Clusters>()->getClusters()[clusterIndex].getName();

                    // Add cluster name to the label text
                    labelText += "Cluster\t: " + clusterName;
                }

                // Configure pen and brush
                painter.setPen(QPen(QBrush(_imageViewerPlugin->getImageViewerWidget().getPixelSelectionTool().getMainColor()), _active ? 2.0f : 1.0f));
                painter.setBrush(Qt::transparent);

                // Upper margin in pixels
                const auto margin = 15;

                // Text font from application
                auto font = QApplication::font();

                // Adjust font size
                font.setPointSizeF(7.0f);
                
                // Create font metrics for establishing the text rectangle size
                QFontMetrics fontMetrics(font);

                // Text rectangle size
                const auto textRectangle = fontMetrics.boundingRect(QRect(QPoint(), QSize(512, 512)), Qt::TextExpandTabs, labelText);

                // Create text options for more control over text layout
                QTextOption textOption(Qt::AlignLeft);

                // Adjust tab stop distance so that the values align
                textOption.setTabStopDistance(50);

                // Draw text rectangle
                painter.setPen(QPen(QBrush(pixelSelectionColor), 0.7f));
                painter.setBrush(QBrush(QColor::fromHsl(pixelSelectionColor.hslHue(), pixelSelectionColor.hsvSaturation(), 80, 200)));
                painter.drawRoundedRect(textRectangle.translated(mousePositionWidget - QPoint(0, textRectangle.height()) + QPoint(margin, -margin)).marginsAdded(QMargins(5, 5, 5, 5)), 2.5f, 2.5f);

                // Draw text
                painter.setFont(font);
                painter.setPen(QPen(pixelSelectionColor, 1.0f));
                painter.drawText(QRectF(mousePositionWidget - QPoint(0, textRectangle.height()) + QPoint(margin, -margin), QSize(512, textRectangle.height())), labelText, textOption);
            }
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to native paint layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to native paint layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

const QString Layer::getImagesDatasetId() const
{
    return _imagesDataset->getGuid();
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
        auto points = Dataset<Points>(const_cast<Layer*>(this)->getSourceDataset());

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
        Dataset<Points>(getSourceDataset())->selectAll();
}

void Layer::selectNone()
{
    if (_sourceDataset->getDataType() == PointType)
        Dataset<Points>(getSourceDataset())->selectNone();
}

void Layer::selectInvert()
{
    if (_sourceDataset->getDataType() == PointType)
        Dataset<Points>(getSourceDataset())->selectInvert();
}

void Layer::startSelection()
{
    try {

#if _DEBUG
        qDebug() << "Start the pixel selection for layer:" << _generalAction.getNameAction().getString();;
#endif

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

void Layer::computeSelection(const QVector<QPoint>& mousePositions /*= QVector<QPoint>()*/)
{
    try {

#if _DEBUG
        qDebug() << "Compute the pixel selection for layer:" << _generalAction.getNameAction().getString();;
#endif

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

#if _DEBUG
        qDebug() << "Reset the selection buffer for layer:" << _generalAction.getNameAction().getString();
#endif

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
    try {
#if _DEBUG
        qDebug() << "Publish pixel selection for layer:" << _generalAction.getNameAction().getString();
#endif

        if (!_generalAction.getVisibleAction().isChecked())
            return;

        // Make sure we have a valid points dataset
        if (!_sourceDataset.isValid())
            throw std::runtime_error("The layer points dataset is not valid after initialization");

        // Make sure we have a valid images dataset
        if (!_imagesDataset.isValid())
            throw std::runtime_error("The layer images dataset is not valid after initialization");

        // Get reference to the pixel selection tool
        auto& pixelSelectionTool = getImageViewerPlugin().getImageViewerWidget().getPixelSelectionTool();

        // Get current selection image
        auto selectionImage = getPropByName<SelectionToolProp>("SelectionToolProp")->getSelectionImage().mirrored(true, true);

        const auto noComponents     = 4;
        const auto width            = static_cast<float>(getImageSize().width());
        const auto noPixels         = _imagesDataset->getNumberOfPixels();
        const auto imageRectangle   = _imagesDataset->getRectangle();

        // Get pixel index from two-dimensional pixel coordinates
        const auto getPixelIndex = [&imageRectangle](const std::int32_t& pixelX, const std::int32_t& pixelY) -> std::int32_t {
            return pixelY * imageRectangle.width() + pixelX;
        };

        if (_sourceDataset->getDataType() == PointType) {
            
            // Get reference to points selection indices
            std::vector<std::uint32_t> selectionIndices;

            // Establish new selection indices depending on the type of modifier
            switch (pixelSelectionTool.getModifier())
            {
                // Replace pixels with new selection
                case PixelSelectionModifierType::Replace:
                {
                    selectionIndices.clear();
                    selectionIndices.reserve(noPixels);

                    // Loop over all the pixels in the selection image in row-column order and add to the selection indices if the pixel is non-zero
                    for (std::int32_t pixelY = 0; pixelY < imageRectangle.height(); pixelY++) {
                        for (std::int32_t pixelX = 0; pixelX < imageRectangle.width(); pixelX++) {
                            if (_maskData[getPixelIndex(pixelX, pixelY)] > 0u && selectionImage.bits()[getPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                selectionIndices.push_back(getPixelIndex(pixelX, pixelY));
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
                    for (std::int32_t pixelY = 0; pixelY < imageRectangle.height(); pixelY++) {
                        for (std::int32_t pixelX = 0; pixelX < imageRectangle.width(); pixelX++) {
                            if (_maskData[getPixelIndex(pixelX, pixelY)] > 0u && selectionImage.bits()[getPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                selectionSet.insert(getPixelIndex(pixelX, pixelY));
                        }
                    }

                    // Convert the set back to a vector
                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                    break;
                }

                // Remove pixels from current selection
                case PixelSelectionModifierType::Subtract:
                {
                    // Create selection set of current selection indices
                    auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                    // Loop over all the pixels in the selection image in row-column order and remove the selection index from the set if the pixel is non-zero
                    for (std::int32_t pixelY = 0; pixelY < imageRectangle.height(); pixelY++) {
                        for (std::int32_t pixelX = 0; pixelX < imageRectangle.width(); pixelX++) {
                            if (_maskData[getPixelIndex(pixelX, pixelY)] > 0u && selectionImage.bits()[getPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                selectionSet.erase(getPixelIndex(pixelX, pixelY));
                        }
                    }

                    // Convert the set back to a vector
                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                    break;
                }

                default:
                    break;
            }

            _sourceDataset->setSelectionIndices(selectionIndices);
        }

        if (_sourceDataset->getDataType() == ClusterType) {

            // Get reference to clusters selection indices
            auto& selectionIndices = _sourceDataset->getSelection<Clusters>()->indices;

            // Convert floating point scalars to unsigned integer scalars
            std::vector<std::uint32_t> integerScalarData(_imageSettingsAction.getScalarChannel1Action().getScalarData().begin(), _imageSettingsAction.getScalarChannel1Action().getScalarData().end());

            // Establish new selection indices depending on the type of modifier
            switch (pixelSelectionTool.getModifier())
            {
                // Replace pixels with new selection
                case PixelSelectionModifierType::Replace:
                {
                    selectionIndices.clear();
                    selectionIndices.reserve(noPixels);

                    // Loop over all the pixels in the selection image in row-column order and add to the selection indices if the pixel is non-zero
                    for (std::int32_t pixelY = 0; pixelY < imageRectangle.height(); pixelY++) {
                        for (std::int32_t pixelX = 0; pixelX < imageRectangle.width(); pixelX++) {
                            if (_maskData[getPixelIndex(pixelX, pixelY)] > 0u && selectionImage.bits()[getPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                selectionIndices.push_back(integerScalarData[getPixelIndex(pixelX, pixelY)]);
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
                    for (std::int32_t pixelY = 0; pixelY < imageRectangle.height(); pixelY++) {
                        for (std::int32_t pixelX = 0; pixelX < imageRectangle.width(); pixelX++) {
                            if (_maskData[getPixelIndex(pixelX, pixelY)] > 0u && selectionImage.bits()[getPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                selectionSet.insert(integerScalarData[getPixelIndex(pixelX, pixelY)]);
                        }
                    }

                    // Convert the set back to a vector
                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());

                    break;
                }

                // Remove pixels from current selection
                case PixelSelectionModifierType::Subtract:
                {
                    // Create selection set of current selection indices
                    auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                    // Loop over all the pixels in the selection image in row-column order and remove the selection index from the set if the pixel is non-zero
                    for (std::int32_t pixelY = 0; pixelY < imageRectangle.height(); pixelY++) {
                        for (std::int32_t pixelX = 0; pixelX < imageRectangle.width(); pixelX++) {
                            if (_maskData[getPixelIndex(pixelX, pixelY)] > 0u && selectionImage.bits()[getPixelIndex(pixelX, pixelY) * noComponents] > 0)
                                selectionSet.erase(integerScalarData[getPixelIndex(pixelX, pixelY)]);
                        }
                    }

                    // Convert the set back to a vector
                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());

                    break;
                }

                default:
                    break;
            }

            // Get reference to the clusters dataset
            auto clusters = Dataset<Clusters>(_sourceDataset);

            // Get selected indices
            const auto selectedIndices = clusters->getSelectedIndices();
            
            // Get reference to clusters input points and its selection
            auto points     = _sourceDataset->getDataHierarchyItem().getParent().getDataset<Points>();
            auto selection  = points->getSelection<Points>();

            // Reserve enough space for selection
            selection->indices.clear();
            selection->indices.reserve(selectedIndices.size());

            std::vector<std::uint32_t> globalIndices;

            // Get global indices for mapping selection
            points->getGlobalIndices(globalIndices);

            // Translate selection indices and add them
            for (auto selectedIndex : selectedIndices)
                selection->indices.push_back(globalIndices[selectedIndex]);

            // Notify others that the point selection changed
            events().notifyDatasetSelectionChanged(points);
        }

        // Notify listeners of the selection change
        events().notifyDatasetSelectionChanged(_sourceDataset->getSourceDataset<DatasetImpl>());

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
        _imagesDataset->getSelectionData(_selectionData, _selectedIndices, _imageSelectionRectangle);

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

QRect Layer::getImageSelectionRectangle() const
{
    return _imageSelectionRectangle;
}

QRectF Layer::getWorldSelectionRectangle() const
{
    // Get pointer to image prop
    auto layerImageProp = getPropByName<ImageProp>("ImageProp");

    // Ensure selection boundaries are valid
    if (!_imageSelectionRectangle.isValid())
        throw std::runtime_error("Selection boundaries are invalid");

    // Compute composite matrix
    const auto matrix = getModelMatrix() * layerImageProp->getModelMatrix();

    // Compute rectangle extents in world coordinates
    return QRectF(matrix * QPointF(_imageSelectionRectangle.topLeft()), matrix * QPointF(_imageSelectionRectangle.bottomRight()));
}

void Layer::zoomToExtents()
{
    try {

#if _DEBUG
        qDebug() << "Zoom to the extents of layer:" << _generalAction.getNameAction().getString();
#endif

        auto layerImageProp = getPropByName<ImageProp>("ImageProp");

        _imageViewerPlugin->getImageViewerWidget().getRenderer().setZoomRectangle(getWorldBoundingRectangle());

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

#if _DEBUG
        qDebug() << "Zoom to the pixel selection of layer:" << _generalAction.getNameAction().getString();;
#endif

        _imageViewerPlugin->getImageViewerWidget().getRenderer().setZoomRectangle(getWorldSelectionRectangle().marginsAdded(QMarginsF(0.0f, 0.0f, 1.0f, 1.0f)));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to zoom to the pixel selection for layer: %1").arg(_generalAction.getNameAction().getString()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to zoom to the pixel selection for layer: %1").arg(_generalAction.getNameAction().getString()));
    }
}

void Layer::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Dataset");
    variantMapMustContain(variantMap, "Title");

    _imagesDataset = hdps::data().getSet(variantMap["Dataset"].toString());

    setText(variantMap["Title"].toString());

    _generalAction.fromParentVariantMap(variantMap);
    _imageSettingsAction.fromParentVariantMap(variantMap);
    _selectionAction.fromParentVariantMap(variantMap);
    _miscellaneousAction.fromParentVariantMap(variantMap);
    _subsetAction.fromParentVariantMap(variantMap);
}

QVariantMap Layer::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Dataset", _imagesDataset->getGuid() },
        { "Title", text() }
    });

    _generalAction.insertIntoVariantMap(variantMap);
    _imageSettingsAction.insertIntoVariantMap(variantMap);
    _selectionAction.insertIntoVariantMap(variantMap);
    _miscellaneousAction.insertIntoVariantMap(variantMap);
    _subsetAction.insertIntoVariantMap(variantMap);

    return variantMap;
}