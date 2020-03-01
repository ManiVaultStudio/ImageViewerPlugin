#include "SettingsWidget.h"
#include "SelectionWidget.h"
#include "LayersWidget.h"

#include <QVBoxLayout>
#include <QDebug>

SettingsWidget::SettingsWidget(QWidget* parent, MainModel* mainModel) :
	QWidget(parent)
{
	auto layout = new QVBoxLayout();

	layout->addWidget(new SelectionWidget(this, mainModel));
	layout->addWidget(new LayersWidget(this, mainModel));

	setLayout(layout);
}

SettingsWidget::~SettingsWidget() = default;