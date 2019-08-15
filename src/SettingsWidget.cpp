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

	// connect(_dataSetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDataSetName);
	// connect(_imagesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWidget::onCurrentImageIndexChanged);

	connect(_dataSetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDataSetName);
	connect(_imageViewerPlugin, QOverload<const QString&>::of(&ImageViewerPlugin::currentDataSetNameChanged), this, &SettingsWidget::onCurrentDataSetNameChanged);
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

void SettingsWidget::onCurrentDataSetNameChanged(const QString& name)
{
	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();

	qDebug() << "Changed data set to:" << name << "which is of type" << imageCollectionType;

	// updateImagesComboBox();
}

void SettingsWidget::onCurrentImageIndexChanged(int index)
{
	// emit currentImageChanged(_dataSetsComboBox->currentText(), index);
}

void SettingsWidget::onSelectedPointsChanged()
{
	qDebug() << "Selection changed!";

	updateImagesComboBox();
}

void SettingsWidget::updateImagesComboBox()
{
	const auto name = _imageViewerPlugin->currentDataSetName();

	if (name.isEmpty())
		return;

	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();

	_imagesComboBox->clear();

	if (imageCollectionType == "SEQUENCE") {
		_imagesLabel->setText("Image");
		_imagesLabel->setToolTip("Image from an image sequence");

		auto imageNames = QStringList();

		if (_imageViewerPlugin->hasSelection(name)) {
			for (unsigned int index : _imageViewerPlugin->selection(name))
			{
				imageNames << QString("Image %1").arg(index);
			}
		}
		else {
			for (int i = 1; i <= _imageViewerPlugin->noImages(name); i++) {
				imageNames << QString("Image %1").arg(i);
			}
		}

		_imagesComboBox->addItems(imageNames);
	}

	if (imageCollectionType == "STACK") {
		_imagesLabel->setText("Channel");
		_imagesLabel->setToolTip("Image channel from an image stack");

		const auto dataSetDimensionNames = _imageViewerPlugin->dimensionNames(name);

		qDebug() << "Image stack channels: " << dataSetDimensionNames;

		_imagesComboBox->addItems(dataSetDimensionNames);
	}
}
