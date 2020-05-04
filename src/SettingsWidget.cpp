#include "SettingsWidget.h"
#include "LayersWidget.h"

#include <QVBoxLayout>
#include <QDebug>

SettingsWidget::SettingsWidget(ImageViewerPlugin* imageViewerPlugin) :
	QWidget()
{
	auto layout = new QVBoxLayout();

	layout->addWidget(new LayersWidget(imageViewerPlugin));
	layout->addStretch(1);

	setLayout(layout);
	setFixedWidth(300);
}

SettingsWidget::~SettingsWidget() = default;