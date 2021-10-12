#include "PointsToImagesDialog.h"
#include "ImageViewerPlugin.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "ImageData/Images.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <stdexcept>

using namespace hdps;

PointsToImagesDialog::PointsToImagesDialog(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName) :
    QDialog(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _points(datasetName),
    _images(),
    _imageWidthAction(this, "Image width", 1, 10000, 100, 100),
    _imageHeightAction(this, "Image height", 1, 10000, 100, 100),
    _numberOfImagesAction(this, "Number of images", 1, 10000, 10, 10),
    _numberOfPixelsAction(this, "Number of pixels"),
    _notesAction(this, "Notes"),
    _groupAction(this)
{
    // Throw exception is dataset name is empty
    if (datasetName.isEmpty())
        throw std::runtime_error("Points dataset name is empty");

    // Update window title and icon
    setWindowTitle(QString("Load %1 as images").arg(datasetName));
    setWindowIcon(_points->getIcon());
    
    // Set widget flags for image width and height actions
    _imageWidthAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _imageHeightAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    // Try to guess the image dimensions if the points dataset is derived
    if (_points->isDerivedData()) {

        // Get the source points dataset
        auto pointsParentHierarchyItem = _points->getHierarchyItem().getParent();

        // Iterate over each child of the source dataset
        for (auto childHierarchyItem : pointsParentHierarchyItem->getChildren()) {

            // Get image dimensions in case of an images dataset
            if (childHierarchyItem->getDataType() == ImageType) {

                // Get reference to images dataset
                _images.setDatasetName(childHierarchyItem->getDatasetName());

                // Get image size
                const auto imageSize = _images->getImageSize();

                // Set image resolution
                _imageWidthAction.initialize(0, 10000, imageSize.width(), imageSize.width());
                _imageHeightAction.initialize(0, 10000, imageSize.height(), imageSize.height());
            }
        }
    }

    // Configure number of images action
    _numberOfImagesAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numberOfImagesAction.setMayReset(false);
    _numberOfImagesAction.setEnabled(false);
    _numberOfImagesAction.setValue(_points->getNumDimensions());

    // Number of pixels and notes may not be reset by the user
    _numberOfPixelsAction.setMayReset(false);
    _notesAction.setMayReset(false);

    // Enable/disable actions
    _numberOfPixelsAction.setEnabled(false);
    _notesAction.setEnabled(false);

    _numberOfPixelsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    auto layout = new QVBoxLayout();

    // Add actions to the group
    _groupAction << _imageWidthAction;
    _groupAction << _imageHeightAction;
    _groupAction << _numberOfImagesAction;
    _groupAction << _numberOfPixelsAction;
    _groupAction << _notesAction;

    // Create group action widget
    auto groupActionWidget = _groupAction.createWidget(this);

    // Adjust margins of group action widget
    groupActionWidget->layout()->setMargin(0);

    // Add the widget to the layout
    layout->addWidget(groupActionWidget);

    setLayout(layout);

    // Create dialog button box so that the user can proceed or cancel with the conversion
    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Convert");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    // Add buttons to the layout
    layout->addStretch(1);
    layout->addWidget(dialogButtonBox);

    // Handle when accepted
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, [this, datasetName]() {

        // Get references to input points and create images dataset
        DatasetRef<Images> images(Application::core()->addData("Images", "images", datasetName));
        DatasetRef<Points> points(datasetName);

        if (!images.isValid())
            throw std::runtime_error("Unable to create images dataset");

        const auto sourceImageSize   = _images.isValid() ? _images->getSourceRectangle().size() : QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());
        const auto targetImageSize   = _images.isValid() ? _images->getTargetRectangle().size() : QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());
        const auto imageOffset       = _images.isValid() ? _images->getTargetRectangle().topLeft() : QPoint();

        images->setType(ImageData::Type::Stack);
        images->setNumberOfImages(_numberOfImagesAction.getValue());
        images->setImageGeometry(sourceImageSize, targetImageSize, imageOffset);
        images->setNumberOfComponentsPerPixel(1);

        // Notify others that an images dataset was added
        Application::core()->notifyDataAdded(images->getName());

        // Add new layer to the model
        _imageViewerPlugin.getModel().addLayer(new Layer(_imageViewerPlugin, images.getDatasetName()));

        // Update bounds
        _imageViewerPlugin.getImageViewerWidget()->updateWorldBoundingRectangle();

        // Exit the dialog
        accept();
    });

    // Handle when rejected
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &PointsToImagesDialog::reject);

    // Update the number of pixels and note action
    const auto updateActions = [this]() {

        // Compute the number of pixels
        const auto numberOfPixels = _imageWidthAction.getValue() * _imageHeightAction.getValue();

        // Update the number of pixels action
        _numberOfPixelsAction.setString(QString("%1").arg(QString::number(numberOfPixels)));

        // Establish whether the number of elements match
        const auto numberOfElementsMatch = numberOfPixels == _points->getNumPoints();

        // Update the notes action
        _notesAction.setString(numberOfElementsMatch ? "" : "Inconsistent number of pixels");
    };

    // Compute the number of pixels when the image width or height changes
    connect(&_imageWidthAction, &IntegralAction::valueChanged, this, updateActions);
    connect(&_imageHeightAction, &IntegralAction::valueChanged, this, updateActions);

    // Initial update of the actions
    updateActions();
}

QSize PointsToImagesDialog::getImageSize() const
{
    return QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());
}