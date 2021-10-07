#include "PointsToImagesDialog.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "ImageData/Images.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <stdexcept>

PointsToImagesDialog::PointsToImagesDialog(const QString& datasetName, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _points(datasetName),
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
                auto& images = childHierarchyItem->getDataset<Images>();

                // Get image size
                const auto imageSize = images.getImageSize();

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

    // Handle when accepted and rejected buttons are clicked
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &PointsToImagesDialog::accept);
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
