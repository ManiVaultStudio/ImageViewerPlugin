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
	
	connect(_ui->datasetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDataset);
	connect(_ui->imagesComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentImage);
	connect(_ui->dimensionsComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDimension);
	connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, _imageViewerPlugin, [=](int state) { qDebug() << state;  _imageViewerPlugin->setAverageImages(static_cast<bool>(state)); });
	
	connect(_imageViewerPlugin, &ImageViewerPlugin::datasetNamesChanged, this, &SettingsWidget::onDatasetNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &SettingsWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::imageNamesChanged, this, &SettingsWidget::onImageNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageChanged, this, &SettingsWidget::onCurrentImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::dimensionNamesChanged, this, &SettingsWidget::onDimensionNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDimensionChanged, this, &SettingsWidget::onCurrentDimensionChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::averageImagesChanged, this, &SettingsWidget::onAverageImagesChanged);

	update();
}

void SettingsWidget::onDatasetNamesChanged(const QStringList& datasetNames)
{
	_ui->datasetsComboBox->blockSignals(true);

	_ui->datasetsComboBox->clear();
	_ui->datasetsComboBox->addItems(datasetNames);
	
	const auto datasetAvailable = datasetNames.size() > 0;

	_ui->currentDatasetLabel->setEnabled(datasetAvailable);
	_ui->datasetsComboBox->setEnabled(datasetAvailable);
	_ui->averageImagesCheckBox->setEnabled(datasetAvailable);

	_ui->datasetsComboBox->blockSignals(false);
}

void SettingsWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	_ui->datasetsComboBox->blockSignals(true);

	_ui->datasetsComboBox->setCurrentText(currentDataset);

	_ui->datasetsComboBox->blockSignals(false);
}

void SettingsWidget::onImageNamesChanged(const QStringList& imageNames)
{
	_ui->imagesComboBox->blockSignals(true);

	_ui->imagesComboBox->clear();
	_ui->imagesComboBox->addItems(imageNames);

	const auto enable = imageNames.size() > 0 && !_imageViewerPlugin->averageImages();

	_ui->currentImageLabel->setEnabled(enable);
	_ui->imagesComboBox->setEnabled(enable);

	_ui->imagesComboBox->blockSignals(false);
}

void SettingsWidget::onCurrentImageChanged(const int& currentImage)
{
	_ui->imagesComboBox->blockSignals(true);

	_ui->imagesComboBox->setCurrentIndex(currentImage);

	_ui->imagesComboBox->blockSignals(false);
}

void SettingsWidget::onDimensionNamesChanged(const QStringList& dimensionNames)
{
	_ui->dimensionsComboBox->blockSignals(true);

	_ui->dimensionsComboBox->clear();
	_ui->dimensionsComboBox->addItems(dimensionNames);

	const auto enable = dimensionNames.size() > 0 && !_imageViewerPlugin->averageImages();

	_ui->currentDimensionLabel->setEnabled(enable);
	_ui->dimensionsComboBox->setEnabled(enable);

	_ui->dimensionsComboBox->blockSignals(false);
}

void SettingsWidget::onCurrentDimensionChanged(const int& currentDimension)
{
	_ui->dimensionsComboBox->blockSignals(true);

	_ui->dimensionsComboBox->setCurrentIndex(currentDimension);

	_ui->dimensionsComboBox->blockSignals(false);
}

void SettingsWidget::onAverageImagesChanged(const bool& averageImages)
{
	_ui->averageImagesCheckBox->blockSignals(true);

	_ui->averageImagesCheckBox->setChecked(averageImages);

	_ui->averageImagesCheckBox->blockSignals(false);
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

	//_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);

	/*
	_ui->currentDatasetLabel->setEnabled(true);
	_ui->datasetsComboBox->setEnabled(true);


	const auto imageCollectionType	= _imageViewerPlugin->imageCollectionType();
	const auto imageFileNames		= _imageViewerPlugin->imageFileNames();

	if (imageCollectionType == "SEQUENCE") {
		/*
		_ui->currentImageLabel->setEnabled(true);
		_ui->imagesComboBox->setEnabled(true);
		_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);
		_ui->currentDimensionLabel->setEnabled(false);
		_ui->dimensionsComboBox->setEnabled(false);
	
		auto imageList = QStringList();

		if (_imageViewerPlugin->hasSelection()) {
			auto images = QStringList();

			for (unsigned int index : _imageViewerPlugin->selection())
			{
				images << QString("%1").arg(imageFileNames[index]);
			}

			const auto imagesString = images.join(", ");

			imageList << imagesString;

			_ui->imagesComboBox->setToolTip(imagesString);
		}
		else {
			if (_imageViewerPlugin->averageImages()) {
				auto images = QStringList();

				for (int i = 0; i < _imageViewerPlugin->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageList << imagesString;

				_ui->imagesComboBox->setToolTip(imagesString);
			}
			else {
				for (int i = 0; i < _imageViewerPlugin->noImages(); i++) {
					imageList << QString("%1").arg(imageFileNames[i]);
				}
			}
		}

		_ui->imagesComboBox->addItems(imageList);
	}
	

	if (imageCollectionType == "MULTIPART") {
		_ui->currentImageLabel->setEnabled(true);
		_ui->imagesComboBox->setEnabled(true);
		_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->noImages() > 1);
		_ui->currentDimensionLabel->setEnabled(true);
		_ui->dimensionsComboBox->setEnabled(true);

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
	}
	*/
}
