#include "SettingsWidget.h"
#include "SelectionWidget.h"
#include "LayersWidget.h"
#include "MainModel.h"

#include <QVBoxLayout>
#include <QDebug>

SettingsWidget::SettingsWidget(QWidget* parent, MainModel* mainModel) :
	QWidget(parent)
{
	auto layout = new QVBoxLayout();

	layout->addWidget(new SelectionWidget(this, mainModel->datasetsModel()));
	layout->addWidget(new LayersWidget(this, mainModel->datasetsModel()));
	layout->addStretch(1);

	setLayout(layout);
}

SettingsWidget::~SettingsWidget() = default;