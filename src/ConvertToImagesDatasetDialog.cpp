#include "ConvertToImagesDatasetDialog.h"
#include "ImageViewerPlugin.h"

#include <Application.h>
#include <DataHierarchyItem.h>
#include <ImageData/Images.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <stdexcept>

using namespace mv;

ConvertToImagesDatasetDialog::ConvertToImagesDatasetDialog(ImageViewerPlugin& imageViewerPlugin, Dataset<DatasetImpl>& dataset) :
    QDialog(&imageViewerPlugin.getWidget()),
    _imageViewerPlugin(imageViewerPlugin),
    _sourceDataset(dataset),
    _sourceImagesDataset(),
    _targetImagesDataset(),
    _datasetNameAction(this, "Dataset name"),
    _imageWidthAction(this, "Image width", 1, 10000, 100),
    _imageHeightAction(this, "Image height", 1, 10000, 100),
    _numberOfImagesAction(this, "Number of images", 1, 10000, 1),
    _numberOfPixelsAction(this, "Number of pixels"),
    _useLinkedDataAction(this, "Use linked data", true),
    _groupAction(this, "Group")
{
    setWindowTitle(QString("Load %1 as images").arg(_sourceDataset->getLocation()));
    setWindowIcon(_sourceDataset->getIcon());
    
    _groupAction.addAction(&_datasetNameAction);
    _groupAction.addAction(&_imageWidthAction);
    _groupAction.addAction(&_imageHeightAction);
    _groupAction.addAction(&_numberOfImagesAction);
    _groupAction.addAction(&_numberOfPixelsAction);
    _groupAction.addAction(&_useLinkedDataAction);

    _imageWidthAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _imageHeightAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    if (_sourceDataset->getDataHierarchyItem().hasParent())
        findSourceImagesDataset(_sourceDataset->getDataHierarchyItem().getParent());

    if (_sourceImagesDataset.isValid()) {
        const auto imageSize = _sourceImagesDataset->getImageSize();

        _imageWidthAction.initialize(0, 10000, imageSize.width());
        _imageHeightAction.initialize(0, 10000, imageSize.height());
    }

    const auto defaultDatasetName = QString("%1_img").arg(_sourceDataset->text());

    _datasetNameAction.setString(defaultDatasetName);

    _numberOfImagesAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numberOfImagesAction.setEnabled(false);

    if (_sourceDataset->getDataType() == PointType)
        _numberOfImagesAction.setValue(_sourceDataset.get<Points>()->getNumDimensions());

    _numberOfPixelsAction.setEnabled(false);
    _numberOfPixelsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    _useLinkedDataAction.setToolTip("Use linked data to generate the image");

    auto layout = new QVBoxLayout();

    auto groupActionWidget = _groupAction.createWidget(this);

    groupActionWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(groupActionWidget);

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Convert");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    layout->addStretch(1);
    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, [this]() {
        auto images = mv::data().createDataset<Images>("Images", _datasetNameAction.getString(), _sourceDataset);

        const auto imageSize = _sourceImagesDataset.isValid() ? _sourceImagesDataset->getImageSize() : QSize(_imageWidthAction.getValue(), _imageHeightAction.getValue());

        images->setType(ImageData::Type::Stack);
        images->setNumberOfImages(_numberOfImagesAction.getValue());
        images->setImageSize(imageSize);
        images->setNumberOfComponentsPerPixel(1);
        images->setLinkedDataFlag(DatasetImpl::LinkedDataFlag::Receive, _useLinkedDataAction.isChecked());

        _targetImagesDataset = images;

        accept();
    });

    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ConvertToImagesDatasetDialog::reject);

    const auto updateActions = [this, dialogButtonBox]() {
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!_datasetNameAction.getString().isEmpty());

        const auto numberOfPixels = _imageWidthAction.getValue() * _imageHeightAction.getValue();

        _numberOfPixelsAction.setString(QString("%1").arg(QString::number(numberOfPixels)));
    };

    connect(&_datasetNameAction, &StringAction::stringChanged, this, updateActions);
    connect(&_imageWidthAction, &IntegralAction::valueChanged, this, updateActions);
    connect(&_imageHeightAction, &IntegralAction::valueChanged, this, updateActions);

    updateActions();
}

Dataset<Images> ConvertToImagesDatasetDialog::getTargetImagesDataset() const
{
    return _targetImagesDataset;
}

void ConvertToImagesDatasetDialog::findSourceImagesDataset(mv::DataHierarchyItem* dataHierarchyItem)
{
    Q_ASSERT(dataHierarchyItem != nullptr);

    if (dataHierarchyItem == nullptr)
        return;

    for (auto childHierarchyItem : dataHierarchyItem->getChildren()) {
        if (childHierarchyItem->getDataType() == ImageType) {
            _sourceImagesDataset = childHierarchyItem->getDataset();
            return;
        }
    }

    if (dataHierarchyItem->hasParent())
        findSourceImagesDataset(dataHierarchyItem->getParent());
}
