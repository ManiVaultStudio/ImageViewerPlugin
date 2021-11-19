#include "ConvertToImagesDatasetDialog.h"
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

ConvertToImagesDatasetDialog::ConvertToImagesDatasetDialog(ImageViewerPlugin& imageViewerPlugin, Dataset<DatasetImpl>& dataset) :
    QDialog(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _sourceDataset(dataset),
    _sourceImagesDataset(),
    _targetImagesDataset(),
    _datasetNameAction(this, "Dataset name"),
    _imageWidthAction(this, "Image width", 1, 10000, 100, 100),
    _imageHeightAction(this, "Image height", 1, 10000, 100, 100),
    _numberOfImagesAction(this, "Number of images", 1, 10000, 1, 1),
    _numberOfPixelsAction(this, "Number of pixels"),
    _groupAction(this)
{
    // Update window title and icon
    setWindowTitle(QString("Load %1 as images").arg(_sourceDataset->getGuiName()));
    setWindowIcon(_sourceDataset->getIcon());
    
    // Set widget flags for image width and height actions
    _imageWidthAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _imageHeightAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    // Find the source images dataset (if any)
    if (_sourceDataset->getDataHierarchyItem().hasParent())
        findSourceImagesDataset(_sourceDataset->getDataHierarchyItem().getParent());

    // Set image resolution if valid
    if (_sourceImagesDataset.isValid()) {

        // Get source images dataset image size
        const auto imageSize = _sourceImagesDataset->getImageSize();

        // Initialize image width and height action
        _imageWidthAction.initialize(0, 10000, imageSize.width(), imageSize.width());
        _imageHeightAction.initialize(0, 10000, imageSize.height(), imageSize.height());
    }

    const auto defaultDatasetName = QString("%1_img").arg(_sourceDataset->getGuiName());

    // Configure name action
    _datasetNameAction.initialize(defaultDatasetName, defaultDatasetName);

    // Configure number of images action
    _numberOfImagesAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numberOfImagesAction.setMayReset(false);
    _numberOfImagesAction.setEnabled(false);

    if (_sourceDataset->getDataType() == PointType)
        _numberOfImagesAction.setValue(_sourceDataset.get<Points>()->getNumDimensions());

    // Number of pixels and notes may not be reset by the user
    _numberOfPixelsAction.setMayReset(false);

    // Enable/disable actions
    _numberOfPixelsAction.setEnabled(false);
    _numberOfPixelsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    auto layout = new QVBoxLayout();

    // Add actions to the group
    _groupAction << _datasetNameAction;
    _groupAction << _imageWidthAction;
    _groupAction << _imageHeightAction;
    _groupAction << _numberOfImagesAction;
    _groupAction << _numberOfPixelsAction;

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
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, [this]() {

        // Add images dataset
        auto images = Application::core()->addDataset<Images>("Images", _datasetNameAction.getString(), _sourceDataset);

        const auto sourceImageSize   = _sourceImagesDataset.isValid() ? _sourceImagesDataset->getSourceRectangle().size() : QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());
        const auto targetImageSize   = _sourceImagesDataset.isValid() ? _sourceImagesDataset->getTargetRectangle().size() : QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());
        const auto imageOffset       = _sourceImagesDataset.isValid() ? _sourceImagesDataset->getTargetRectangle().topLeft() : QPoint();

        images->setType(ImageData::Type::Stack);
        images->setNumberOfImages(_numberOfImagesAction.getValue());
        images->setImageGeometry(sourceImageSize, targetImageSize, imageOffset);
        images->setNumberOfComponentsPerPixel(1);

        // Notify others that an images dataset was added
        Application::core()->notifyDataAdded(*images);

        // Assign target images dataset reference
        _targetImagesDataset.set(*images);

        // Exit the dialog
        accept();
    });

    // Handle when rejected
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ConvertToImagesDatasetDialog::reject);

    // Update the number of pixels and note action
    const auto updateActions = [this, dialogButtonBox]() {

        // Update state of the dataset name action
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!_datasetNameAction.getString().isEmpty());

        // Compute the number of pixels
        const auto numberOfPixels = _imageWidthAction.getValue() * _imageHeightAction.getValue();

        // Update the number of pixels action
        _numberOfPixelsAction.setString(QString("%1").arg(QString::number(numberOfPixels)));
    };

    connect(&_datasetNameAction, &StringAction::stringChanged, this, updateActions);
    connect(&_imageWidthAction, &IntegralAction::valueChanged, this, updateActions);
    connect(&_imageHeightAction, &IntegralAction::valueChanged, this, updateActions);

    // Initial update of the actions
    updateActions();
}

Dataset<Images> ConvertToImagesDatasetDialog::getTargetImagesDataset() const
{
    return _targetImagesDataset;
}

void ConvertToImagesDatasetDialog::findSourceImagesDataset(hdps::DataHierarchyItem& dataHierarchyItem)
{
    // Iterate over each child of the source dataset
    for (auto childHierarchyItem : dataHierarchyItem.getChildren()) {

        // Get image dimensions in case of an images dataset
        if (childHierarchyItem->getDataType() == ImageType) {
            _sourceImagesDataset = childHierarchyItem->getDataset();
            return;
        }
    }

    // Source images dataset not found yet, so try the parent (if it exists)
    if (dataHierarchyItem.hasParent())
        findSourceImagesDataset(dataHierarchyItem.getParent());
}
