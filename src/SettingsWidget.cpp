#include "SettingsWidget.h"
#include "LayersWidget.h"

#include "ui_SettingsWidget.h"

#include <QVBoxLayout>
#include <QDebug>

SettingsWidget::SettingsWidget(ImageViewerPlugin* imageViewerPlugin) :
	QWidget(),
	_imageViewerPlugin(imageViewerPlugin),
	_ui{ std::make_unique<Ui::SettingsWidget>() }
{
	_ui->setupUi(this);
	
	_ui->layersWidget->initialize(_imageViewerPlugin);
	_ui->showHintsCheckBox->hide();

	setFixedWidth(300);
}

SettingsWidget::~SettingsWidget() = default;