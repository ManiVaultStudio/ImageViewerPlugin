#include "PointsToImagesDialog.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "ImageData/Images.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

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
    setWindowTitle("Convert points dataset to images");
    setWindowIcon(hdps::Application::getIconFont("FontAwesome").getIcon("database"));

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

                // Set 
                _imageWidthAction.initialize(0, 10000, imageSize.width(), imageSize.width());
                _imageHeightAction.initialize(0, 10000, imageSize.height(), imageSize.height());
            }
        }
    }

    _numberOfImagesAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numberOfImagesAction.setMayReset(false);
    _numberOfImagesAction.setEnabled(false);
    _numberOfImagesAction.setValue(_points->getNumDimensions());

    _numberOfPixelsAction.setMayReset(false);
    _notesAction.setMayReset(false);

    _numberOfPixelsAction.setEnabled(false);
    _notesAction.setEnabled(false);

    _numberOfPixelsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    auto layout = new QVBoxLayout();

    _groupAction << _imageWidthAction;
    _groupAction << _imageHeightAction;
    _groupAction << _numberOfImagesAction;
    _groupAction << _numberOfPixelsAction;
    _groupAction << _notesAction;

    layout->addWidget(_groupAction.createWidget(this));

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Convert");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, [this]() {
        accept();
    });

    connect(dialogButtonBox, &QDialogButtonBox::rejected, [this]() {
        reject();
    });

    const auto updateNumberOfPixels = [this]() {

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
    connect(&_imageWidthAction, &IntegralAction::valueChanged, this, updateNumberOfPixels);
    connect(&_imageHeightAction, &IntegralAction::valueChanged, this, updateNumberOfPixels);

    // Compute initial number of pixels
    updateNumberOfPixels();


}

QSize PointsToImagesDialog::getImageSize() const
{
    return QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());
}
