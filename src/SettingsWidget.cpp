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
	connect(_ui->imagesComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentImageId);
	connect(_ui->dimensionsComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDimensionId);
	connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, _imageViewerPlugin, [=](int state) { qDebug() << state;  _imageViewerPlugin->setAverageImages(static_cast<bool>(state)); });
	
	connect(_imageViewerPlugin, &ImageViewerPlugin::datasetNamesChanged, this, &SettingsWidget::onDatasetNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &SettingsWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::imageNamesChanged, this, &SettingsWidget::onImageNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &SettingsWidget::onCurrentImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::dimensionNamesChanged, this, &SettingsWidget::onDimensionNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDimensionIdChanged, this, &SettingsWidget::onCurrentDimensionChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::averageImagesChanged, this, &SettingsWidget::onAverageImagesChanged);

	update();
}

void SettingsWidget::onDatasetNamesChanged(const QStringList& datasetNames)
{
	const auto datasetAvailable = datasetNames.size() > 0;

	_ui->datasetsComboBox->blockSignals(true);

	_ui->datasetsComboBox->clear();
	_ui->datasetsComboBox->addItems(datasetNames);
	_ui->datasetsComboBox->setEnabled(datasetAvailable);
	
	_ui->datasetsComboBox->blockSignals(false);

	_ui->currentDatasetLabel->setEnabled(datasetAvailable);
}

void SettingsWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	_ui->datasetsComboBox->blockSignals(true);

	_ui->datasetsComboBox->setCurrentText(currentDataset);
	
	_ui->datasetsComboBox->blockSignals(false);

	_ui->averageImagesCheckBox->setEnabled(_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Sequence);
}

void SettingsWidget::onImageNamesChanged(const QStringList& imageNames)
{
	const auto enable = imageNames.size() > 0 && !_imageViewerPlugin->averageImages();

	_ui->imagesComboBox->blockSignals(true);

	_ui->imagesComboBox->clear();
	_ui->imagesComboBox->addItems(imageNames);
	_ui->imagesComboBox->setEnabled(enable);
	
	_ui->imagesComboBox->blockSignals(false);

	_ui->currentImageLabel->setEnabled(enable);
	
}

void SettingsWidget::onCurrentImageChanged(const int& currentImage)
{
	_ui->imagesComboBox->blockSignals(true);

	_ui->imagesComboBox->setCurrentIndex(currentImage);
	_ui->imagesComboBox->setToolTip(_imageViewerPlugin->currentImageFilePath());
	
	_ui->imagesComboBox->blockSignals(false);
}

void SettingsWidget::onDimensionNamesChanged(const QStringList& dimensionNames)
{
	const auto enable = dimensionNames.size() > 0 && !_imageViewerPlugin->averageImages();

	_ui->dimensionsComboBox->blockSignals(true);
	
	_ui->dimensionsComboBox->clear();
	_ui->dimensionsComboBox->addItems(dimensionNames);
	_ui->dimensionsComboBox->setEnabled(enable);
	
	_ui->dimensionsComboBox->blockSignals(false);

	_ui->currentDimensionLabel->setEnabled(enable);
}

void SettingsWidget::onCurrentDimensionChanged(const int& currentDimension)
{
	_ui->dimensionsComboBox->blockSignals(true);

	_ui->dimensionsComboBox->setCurrentIndex(currentDimension);
	_ui->dimensionsComboBox->setToolTip(_imageViewerPlugin->currentDimensionName());
	
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
}