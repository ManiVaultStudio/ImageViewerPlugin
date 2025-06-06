#include "ExportToImageDialog.h"

#include <Application.h>
#include <DataHierarchyItem.h>
#include <ImageData/Images.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <stdexcept>

using namespace mv;
using namespace mv::util;

ExportToImageDialog::ExportToImageDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _imageScaleFactorAction(this, "Image scale", 1.0f, 1000.0f, 100.0f, 100.0f),
    _imageResolutionAction(this, "Image resolution"),
    _groupAction(this, "Group")
{
    setWindowTitle("Export layer(s) to image");
    setWindowIcon(StyledIcon("camera"));
    
    _imageScaleFactorAction.setSuffix("%");

    auto layout = new QVBoxLayout();

    addAction(&_imageScaleFactorAction);
    addAction(&_imageResolutionAction);

    auto groupActionWidget = _groupAction.createWidget(this);

    groupActionWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(groupActionWidget);

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Export");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    layout->addStretch(1);
    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ExportToImageDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ExportToImageDialog::reject);

    /*
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
    */

    adjustSize();
}
