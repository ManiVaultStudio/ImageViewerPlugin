#include "SettingsWidget.h"
#include "ImageViewerPlugin.h"

#include "ui_SettingsWidget.h"

#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

SettingsWidget::SettingsWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin),
	_ui{ std::make_unique<Ui::SettingsWidget>() }
{
	_ui->setupUi(this);
	
	connect(_ui->datasetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDatasetName);
	connect(_imageViewerPlugin, QOverload<const QString&>::of(&ImageViewerPlugin::currentDatasetNameChanged), this, &SettingsWidget::update);

	/*
	connect(_ui->imagesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentImageId);
	
	
	
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &SettingsWidget::onSelectedPointsChanged);
	connect(_imageViewerPlugin, QOverload<const bool&>::of(&ImageViewerPlugin::averageImagesChanged), this, &SettingsWidget::onAverageImagesChanged);
	connect(_ui->averageCheckBox, &QCheckBox::stateChanged, _imageViewerPlugin, QOverload<const bool&>::of(&ImageViewerPlugin::setAverageImages));
	*/

	update();
}

void SettingsWidget::addDataSet(const QString & name)
{
	qDebug() << "addDataSet";
	_ui->datasetsComboBox->addItem(name);

	update();
}

void SettingsWidget::removeDataSet(const QString & name)
{
	qDebug() << "Removing data set:" << name;

	const auto index = _ui->datasetsComboBox->findText(name);

	if (index >= 0)
		_ui->datasetsComboBox->removeItem(index);
}

void SettingsWidget::onAverageImagesChanged(const bool& averageImages)
{
	//_ui->averageCheckBox->setChecked(averageImages);
	
	//update();
}

void SettingsWidget::onSelectedPointsChanged()
{
	update();
}

void SettingsWidget::update()
{
	const auto hasDatasets = _ui->datasetsComboBox->count() > 0;

	if (!hasDatasets) {
		_ui->currentDatasetLabel->setEnabled(false);
		_ui->datasetsComboBox->setEnabled(false);
		_ui->currentImageLabel->setEnabled(false);
		_ui->imagesComboBox->setEnabled(false);
		_ui->currentDimensionLabel->setEnabled(false);
		_ui->dimensionsComboBox->setEnabled(false);
		_ui->averageImagesCheckBox->setEnabled(false);

		return;
	}

	_ui->currentDatasetLabel->setEnabled(true);
	_ui->datasetsComboBox->setEnabled(true);

	/*
	_imagesAverageCheckBox->setChecked(_imageViewerPlugin->averageImages());
	
	_imagesComboBox->clear();
	_imagesComboBox->setEnabled(!_imageViewerPlugin->averageImages());
	_imagesComboBox->setToolTip("");
	*/

	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();
	
	/*
	const auto imageFileNames = _imageViewerPlugin->imageFileNames();
	*/

	if (imageCollectionType == "SEQUENCE") {
		_ui->currentImageLabel->setEnabled(true);
		_ui->imagesComboBox->setEnabled(true);
		_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);
		_ui->currentDimensionLabel->setEnabled(false);
		_ui->dimensionsComboBox->setEnabled(false);
	

		/*
		auto imageList = QStringList();

		if (_imageViewerPlugin->hasSelection()) {
			auto images = QStringList();

			for (unsigned int index : _imageViewerPlugin->selection())
			{
				images << QString("%1").arg(images[index]);
			}

			const auto imagesString = images.join(", ");

			imageList << imagesString;

			_imagesComboBox->setToolTip(imagesString);
		}
		else {
			if (_imageViewerPlugin->averageImages()) {
				auto images = QStringList();

				for (int i = 0; i < _imageViewerPlugin->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageList << imagesString;

				_imagesComboBox->setToolTip(imagesString);
			}
			else {
				for (int i = 0; i < _imageViewerPlugin->noImages(); i++) {
					imageList << QString("%1").arg(imageFileNames[i]);
				}
			}
		}

		_imagesComboBox->addItems(imageList);
		*/
	}
	
	if (imageCollectionType == "STACK") {
		_ui->currentImageLabel->setEnabled(false);
		_ui->imagesComboBox->setEnabled(false);
		_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);
		_ui->currentDimensionLabel->setEnabled(true);
		_ui->dimensionsComboBox->setEnabled(true);

		/*
		const auto dataSetDimensionNames = _imageViewerPlugin->dimensionNames();

		_imagesAverageCheckBox->setEnabled(dataSetDimensionNames.size() > 1);

		auto imageList = QStringList();

		if (_imageViewerPlugin->averageImages()) {
			const auto imagesString = _imageViewerPlugin->imageFileNames().join(", ");

			imageList << imagesString;

			_imagesComboBox->setToolTip(imagesString);
		}
		else {
			imageList = imageFileNames;
		}

		_imagesComboBox->blockSignals(true);

		_imagesComboBox->addItems(imageList);
		_imagesComboBox->setCurrentIndex(_imageViewerPlugin->currentImageId());

		_imagesComboBox->blockSignals(false);
		*/
	}
	

	if (imageCollectionType == "MULTIPART") {
		_ui->currentImageLabel->setEnabled(true);
		_ui->imagesComboBox->setEnabled(true);
		_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);
		_ui->currentDimensionLabel->setEnabled(true);
		_ui->dimensionsComboBox->setEnabled(true);

		/*
		_imagesAverageCheckBox->setToolTip("Average channels");

		const auto dataSetDimensionNames = _imageViewerPlugin->dimensionNames();

		_imagesAverageCheckBox->setEnabled(dataSetDimensionNames.size() > 1);

		auto imageList = QStringList();

		if (_imageViewerPlugin->averageImages()) {
			const auto imagesString = _imageViewerPlugin->imageFileNames().join(", ");

			imageList << imagesString;

			_imagesComboBox->setToolTip(imagesString);
		}
		else {
			imageList = imageFileNames;
		}

		_imagesComboBox->blockSignals(true);

		_imagesComboBox->addItems(imageList);
		_imagesComboBox->setCurrentIndex(_imageViewerPlugin->currentImageId());

		_imagesComboBox->blockSignals(false);
		*/
	}
}
