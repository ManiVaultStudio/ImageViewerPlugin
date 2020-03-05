#include "SettingsWidget.h"
#include "SelectionWidget.h"
#include "LayersWidget.h"
#include "DatasetsModel.h"

#include <QVBoxLayout>
#include <QDebug>

SettingsWidget::SettingsWidget(QWidget* parent, DatasetsModel* datasetsModel) :
	QWidget(parent)
{
	auto layout = new QVBoxLayout();

	layout->addWidget(new SelectionWidget(this, datasetsModel));
	layout->addWidget(new LayersWidget(this, datasetsModel));
	layout->addStretch(1);

	setLayout(layout);
}

SettingsWidget::~SettingsWidget() = default;