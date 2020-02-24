#include "SettingsWidget.h"
#include "ImageDatasets.h"

#include "ui_SettingsWidget.h"

#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

SettingsWidget::SettingsWidget(ImageDatasets* datasets) :
	QWidget(),
	_datasets(datasets),
	_ui{ std::make_unique<Ui::SettingsWidget>() }
{
	_ui->setupUi(this);
	
	connect(_ui->datasetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _datasets, &ImageDatasets::setCurrentDatasetName);

	connect(_ui->dimensionsComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), [&](const int currentDimensionIndex) {
		_datasets->currentDataset()->setCurrentDimensionIndex(currentDimensionIndex);
	});

	connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, this, [&](int state) {
		_datasets->currentDataset()->setAverage(static_cast<bool>(state));
	});
	
	connect(_ui->selectionOpacitySlider, &QSlider::valueChanged, _datasets, [&](int value) {
		_datasets->currentDataset()->setSelectionOpacity(static_cast<float>(value) / 100.f);
	});
	
	connect(_ui->createSubsetFromSelectionPushButton, &QCheckBox::clicked, [&]() {
		_datasets->currentDataset()->createSubsetFromSelection();
	});
	
	connect(_datasets, &ImageDatasets::namesChanged, this, &SettingsWidget::onDatasetNamesChanged);
	connect(_datasets, &ImageDatasets::currentDatasetChanged, this, &SettingsWidget::onCurrentDatasetChanged);
}

SettingsWidget::~SettingsWidget() = default;

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

void SettingsWidget::onCurrentDatasetChanged(ImageDataset* previousDataset, ImageDataset* currentDataset)
{
	_ui->datasetsComboBox->blockSignals(true);
	_ui->datasetsComboBox->setCurrentText(currentDataset->name());
	_ui->datasetsComboBox->blockSignals(false);

	_ui->averageImagesCheckBox->setEnabled(currentDataset->canAverage());

	_ui->averageImagesCheckBox->blockSignals(true);
	_ui->averageImagesCheckBox->setChecked(currentDataset->average());
	_ui->averageImagesCheckBox->blockSignals(false);

	const auto enable = currentDataset->imageNames().size() > 0 && !currentDataset->average();

	_ui->imagesComboBox->blockSignals(true);

	_ui->imagesComboBox->clear();
	_ui->imagesComboBox->addItems(currentDataset->imageNames());
	_ui->imagesComboBox->setEnabled(enable);
	_ui->imagesComboBox->blockSignals(false);

	const auto dimensionNames = currentDataset->dimensionNames();

	_ui->dimensionsComboBox->blockSignals(true);

	_ui->dimensionsComboBox->clear();
	_ui->dimensionsComboBox->addItems(currentDataset->dimensionNames());

	switch (currentDataset->imageCollectionType())
	{
		case ImageCollectionType::Sequence:
		{
			_ui->dimensionsComboBox->setEnabled(dimensionNames.size() > 0 && !currentDataset->average());
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

	_ui->currentDimensionLabel->setEnabled(dimensionNames.size() > 0 && !currentDataset->average());

	_ui->selectionOpacitySlider->blockSignals(true);
	_ui->selectionOpacitySlider->setValue(currentDataset->selectionOpacity() * 100.0f);
	_ui->selectionOpacitySlider->blockSignals(false);

	if (currentDataset->imageCollectionType() == ImageCollectionType::Sequence) {
		_ui->createSubsetFromSelectionPushButton->setEnabled(false);
		_ui->selectionOpacityLabel->setEnabled(false);
		_ui->selectionOpacitySlider->setEnabled(false);
	}
	else {
		const auto hasSelection = currentDataset->hasSelection();

		_ui->createSubsetFromSelectionPushButton->setEnabled(hasSelection);
		_ui->selectionOpacityLabel->setEnabled(hasSelection);
		_ui->selectionOpacitySlider->setEnabled(hasSelection);
	}

	if (previousDataset) {
		disconnect(previousDataset, &ImageDataset::currentImageIndexChanged, this, nullptr);
		disconnect(previousDataset, &ImageDataset::currentDimensionIndexChanged, this, nullptr);
		disconnect(previousDataset, &ImageDataset::averageChanged, this, nullptr);
		disconnect(previousDataset, &ImageDataset::selectionOpacityChanged, this, nullptr);
		disconnect(previousDataset, &ImageDataset::selectionChanged, this, nullptr);
	}

	if (currentDataset) {
		connect(currentDataset, &ImageDataset::currentImageIndexChanged, this, [&](const std::int32_t& currentImageIndex) {
			_ui->imagesComboBox->blockSignals(true);
			_ui->imagesComboBox->setCurrentIndex(currentImageIndex);
			_ui->imagesComboBox->blockSignals(false);
		});

		connect(currentDataset, &ImageDataset::currentDimensionIndexChanged, this, [&](const std::int32_t& currentDimensionIndex) {
			_ui->dimensionsComboBox->blockSignals(true);
			_ui->dimensionsComboBox->setCurrentIndex(currentDimensionIndex);
			_ui->dimensionsComboBox->blockSignals(false);
		});

		connect(currentDataset, &ImageDataset::averageChanged, this, [&](const bool& average) {
			_ui->averageImagesCheckBox->blockSignals(true);
			_ui->averageImagesCheckBox->setChecked(average);
			_ui->averageImagesCheckBox->blockSignals(false);
		});

		connect(currentDataset, &ImageDataset::selectionOpacityChanged, this, [&](const float& selectionOpacity) {
			_ui->selectionOpacitySlider->blockSignals(true);
			_ui->selectionOpacitySlider->setValue(selectionOpacity * 100.0f);
			_ui->selectionOpacitySlider->blockSignals(false);
		});

		connect(currentDataset, &ImageDataset::selectionChanged, this, [&]() {
			if (_datasets->currentDataset()->imageCollectionType() == ImageCollectionType::Sequence) {
				_ui->createSubsetFromSelectionPushButton->setEnabled(false);
				_ui->selectionOpacityLabel->setEnabled(false);
				_ui->selectionOpacitySlider->setEnabled(false);
			}
			else {
				const auto hasSelection = _datasets->currentDataset()->hasSelection();

				_ui->createSubsetFromSelectionPushButton->setEnabled(hasSelection);
				_ui->selectionOpacityLabel->setEnabled(hasSelection);
				_ui->selectionOpacitySlider->setEnabled(hasSelection);
			}
		});
	}
}