#include "SettingsWidget.h"
#include "ImageViewerPlugin.h"

#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

SettingsWidget::SettingsWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin)
{
	_dataSetsComboBox		= new QComboBox();
	_imagesLabel			= new QLabel("Image");
	_imagesComboBox			= new QComboBox();
	_imagesAverageCheckBox	= new QCheckBox("Average images");
	_zoomToSelection		= new QCheckBox("Zoom to selection");

	_imagesComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	auto layout = new QVBoxLayout();

	layout->addWidget(_dataSetsComboBox);

	auto imagesLayout = new QHBoxLayout();

	imagesLayout->addWidget(_imagesLabel);
	imagesLayout->addWidget(_imagesComboBox);
	imagesLayout->addWidget(_imagesAverageCheckBox);

	layout->addLayout(imagesLayout);
	layout->addWidget(_zoomToSelection);

	setLayout(layout);

	// connect(_dataSetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDataSetName);
	connect(_imagesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentImageId);

	connect(_dataSetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDataSetName);
	connect(_imageViewerPlugin, QOverload<const QString&>::of(&ImageViewerPlugin::currentDataSetNameChanged), this, &SettingsWidget::onCurrentDataSetNameChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &SettingsWidget::onSelectedPointsChanged);
	connect(_imageViewerPlugin, QOverload<const bool&>::of(&ImageViewerPlugin::averageImagesChanged), this, &SettingsWidget::update);
	connect(_imagesAverageCheckBox, &QCheckBox::stateChanged, _imageViewerPlugin, QOverload<const bool&>::of(&ImageViewerPlugin::setAverageImages));

	update();
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

	if (_imageViewerPlugin->imageCollectionType() == "STACK") {
		_imageViewerPlugin->setAverageImages(false);
	}

	updateImagesComboBox();

	update();
}

void SettingsWidget::onAverageImagesChanged(const bool& averageImages)
{
	_imagesAverageCheckBox->setChecked(averageImages);
}

void SettingsWidget::onSelectedPointsChanged()
{
	updateImagesComboBox();

	update();
}

void SettingsWidget::updateImagesComboBox()
{
	qDebug() << "updateImagesComboBox";
	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();

	_imagesComboBox->clear();

	if (imageCollectionType == "SEQUENCE") {
		auto imageNames = QStringList();

		if (_imageViewerPlugin->hasSelection()) {
			for (unsigned int index : _imageViewerPlugin->selection())
			{
				imageNames << QString("%1").arg(index);
			}
		}
		else {
			for (int i = 1; i <= _imageViewerPlugin->noImages(); i++) {
				imageNames << QString("%1").arg(i);
			}
		}

		_imagesComboBox->addItems(imageNames);
	}

	if (imageCollectionType == "STACK") {
		const auto dataSetDimensionNames = _imageViewerPlugin->dimensionNames();
		
		_imagesComboBox->addItems(dataSetDimensionNames);
	}
}

void SettingsWidget::update()
{
	_dataSetsComboBox->setEnabled(_dataSetsComboBox->count() > 0);

	const auto name = _imageViewerPlugin->currentDataSetName();

	if (name.isEmpty()) {
		_imagesLabel->setEnabled(false);
		_imagesComboBox->setEnabled(false);
		_imagesAverageCheckBox->setEnabled(false);
		_zoomToSelection->setEnabled(false);
		return;
	}

	_imagesLabel->setEnabled(true);
	_imagesAverageCheckBox->setChecked(_imageViewerPlugin->averageImages());
	_imagesComboBox->setEnabled(!_imageViewerPlugin->averageImages());

	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();

	if (imageCollectionType == "SEQUENCE") {
		_imagesLabel->setText("Image");
		_imagesLabel->setToolTip("Image from an image sequence");
		
		_imagesAverageCheckBox->setText("Average images");
		_imagesAverageCheckBox->setToolTip("Average images");

		_imagesAverageCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);
		_zoomToSelection->setEnabled(false);
	}

	if (imageCollectionType == "STACK") {
		_imagesLabel->setText("Channel");
		_imagesLabel->setToolTip("Image channel from an image stack");

		_imagesAverageCheckBox->setText("Average channels");
		_imagesAverageCheckBox->setToolTip("Average channels");

		const auto dataSetDimensionNames = _imageViewerPlugin->dimensionNames();

		_imagesAverageCheckBox->setEnabled(dataSetDimensionNames.size() > 1);
		_zoomToSelection->setEnabled(true);
	}
}
