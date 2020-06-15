#include "WindowLevelDialog.h"
#include "LayersWidget.h"

#include "ui_WindowLevelDialog.h"

#include <QVBoxLayout>
#include <QDebug>

WindowLevelDialog::WindowLevelDialog(ImageViewerPlugin* imageViewerPlugin) :
	QDialog(),
	_imageViewerPlugin(imageViewerPlugin),
	_ui{ std::make_unique<Ui::WindowLevelDialog>() }
{
	_ui->setupUi(this);
}

WindowLevelDialog::~WindowLevelDialog() = default;