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
	connect(_ui->imagesComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentImageId);
	connect(_ui->dimensionsComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), _imageViewerPlugin, &ImageViewerPlugin::setCurrentDimensionId);
	connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, _imageViewerPlugin, [=](int state) { _imageViewerPlugin->setAverageImages(static_cast<bool>(state)); });
	connect(_ui->selectionOpacitySlider, &QSlider::valueChanged, _imageViewerPlugin, [=](int value) { _imageViewerPlugin->setSelectionOpacity(static_cast<float>(value) / 100.f); });
	connect(_ui->createSubsetFromSelectionPushButton, &QCheckBox::clicked, _imageViewerPlugin, &ImageViewerPlugin::createSubsetFromSelection);
	
	connect(_imageViewerPlugin, &ImageViewerPlugin::datasetNamesChanged, this, &SettingsWidget::onDatasetNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &SettingsWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::imageNamesChanged, this, &SettingsWidget::onImageNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &SettingsWidget::onCurrentImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::dimensionNamesChanged, this, &SettingsWidget::onDimensionNamesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDimensionIdChanged, this, &SettingsWidget::onCurrentDimensionChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::averageImagesChanged, this, &SettingsWidget::onAverageImagesChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &SettingsWidget::onSelectionImageChanged);

	const auto hasDatasets = _ui->datasetsComboBox->count() > 0;

	if (!hasDatasets) {
		_ui->currentDatasetLabel->setEnabled(false);
		_ui->datasetsComboBox->setEnabled(false);
		_ui->currentImageLabel->setEnabled(false);
		_ui->imagesComboBox->setEnabled(false);
		_ui->currentDimensionLabel->setEnabled(false);
		_ui->dimensionsComboBox->setEnabled(false);
		_ui->averageImagesCheckBox->setEnabled(false);

		updateSelectionOpacityUI();
	}
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

	updateSelectionOpacityUI();
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
	
	_ui->imagesComboBox->blockSignals(false);
}

void SettingsWidget::onDimensionNamesChanged(const QStringList& dimensionNames)
{
	const auto enable = dimensionNames.size() > 0 && !_imageViewerPlugin->averageImages();

	_ui->dimensionsComboBox->blockSignals(true);
	
	_ui->dimensionsComboBox->clear();
	_ui->dimensionsComboBox->addItems(dimensionNames);
	
	switch (_imageViewerPlugin->imageCollectionType())
	{
		case ImageCollectionType::Sequence:
		{
			_ui->dimensionsComboBox->setEnabled(dimensionNames.size() > 0 && !_imageViewerPlugin->averageImages());
			break;
		}

		case ImageCollectionType::Stack:
		case ImageCollectionType::MultiPartSequence:
		{
			_ui->dimensionsComboBox->setEnabled(dimensionNames.size() > 0);
			break;
		}

		default:
			break;
	}
	
	_ui->dimensionsComboBox->blockSignals(false);

	_ui->currentDimensionLabel->setEnabled(enable);
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

void SettingsWidget::onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds)
{
	_ui->averageImagesCheckBox->blockSignals(true);

	updateSelectionOpacityUI();

	_ui->averageImagesCheckBox->blockSignals(false);
}

void SettingsWidget::updateSelectionOpacityUI()
{
	const auto hasSelection = _imageViewerPlugin->hasSelection();

	_ui->selectionOpacitySlider->setValue(_imageViewerPlugin->selectionOpacity() * 100.0f);

	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();

	if (_imageViewerPlugin->datasetNames().size() == 0 || imageCollectionType == ImageCollectionType::Sequence)
	{
		_ui->createSubsetFromSelectionPushButton->setEnabled(false);
		_ui->selectionOpacityLabel->setEnabled(false);
		_ui->selectionOpacitySlider->setEnabled(false);
	}

	if (imageCollectionType == ImageCollectionType::Stack || imageCollectionType == ImageCollectionType::MultiPartSequence)
	{
		_ui->createSubsetFromSelectionPushButton->setEnabled(hasSelection);
		_ui->selectionOpacityLabel->setEnabled(hasSelection);
		_ui->selectionOpacitySlider->setEnabled(hasSelection);
	}
}
