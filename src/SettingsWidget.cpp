#include "SettingsWidget.h"
#include "ImageViewerPlugin.h"

#include <QDebug>
#include <QComboBox>
#include <QLabel>

SettingsWidget::SettingsWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin)
{
	_dataSetsComboBox	= new QComboBox();
	_imagesLabel		= new QLabel("Image");
	_imagesComboBox		= new QComboBox();

	auto layout = new QVBoxLayout();

	layout->addWidget(_dataSetsComboBox);

	auto imagesLayout = new QHBoxLayout();

	imagesLayout->addWidget(_imagesLabel, 1);
	imagesLayout->addWidget(_imagesComboBox, 4);

	layout->addLayout(imagesLayout);

	setLayout(layout);

	connect(_dataSetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), this, &SettingsWidget::onCurrentDataSetChanged);
}

void SettingsWidget::addDataSet(const QString & name)
{
	qDebug() << "Adding data set:" << name;

	_dataSetsComboBox->addItem(name);
}

void SettingsWidget::removeDataSet(const QString & name)
{
	qDebug() << "Removing data set:" << name;

	const auto index = _dataSetsComboBox->findText(name);

	if (index >= 0)
		_dataSetsComboBox->removeItem(index);
}

void SettingsWidget::onCurrentDataSetChanged(const QString& name)
{
	const auto dataSetType = _imageViewerPlugin->dataSetType(name);

	qDebug() << "Changed data set to:" << name << "which is of type" << dataSetType;

	if (dataSetType == "SEQUENCE") {
		_imagesLabel->setText("Image");
		_imagesLabel->setToolTip("Image from an image sequence");


	}

	if (dataSetType == "STACK") {
		_imagesLabel->setText("Channel");
		_imagesLabel->setToolTip("Image channel from an image stack");

		const auto dataSetDimensionNames = _imageViewerPlugin->dataSetDimensionNames(name);

		qDebug() << "Image stack channels: " << dataSetDimensionNames;

		_imagesComboBox->addItems(dataSetDimensionNames);
	}
}