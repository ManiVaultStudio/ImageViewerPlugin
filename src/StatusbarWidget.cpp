#include "StatusbarWidget.h"
#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "CanvasWidget.h"

#include "ui_StatusbarWidget.h"

#include <QCheckBox>
#include <QDebug>

StatusbarWidget::StatusbarWidget(QWidget* parent) :
	QWidget(parent),
	_ui{ std::make_unique<Ui::StatusbarWidget>() }
{
	_ui->setupUi(this);

	auto imageViewerPlugin = dynamic_cast<ImageViewerPlugin*>(parent->parent());

	QObject::connect(_ui->zoomExtentsPushButton, &QPushButton::clicked, [imageViewerPlugin]() {
		imageViewerPlugin->getViewerWidget()->getCanvasWidget()->zoomExtents();
	});

	_ui->showHintsCheckBox->setChecked(imageViewerPlugin->getViewerWidget()->getShowHints());

	QObject::connect(_ui->showHintsCheckBox, &QCheckBox::stateChanged, [imageViewerPlugin](int state) {
		imageViewerPlugin->getViewerWidget()->setShowHints(state);
	});
}