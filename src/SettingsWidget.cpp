#include "SettingsWidget.h"
#include "ImageDatasetsModel.h"
#include "ImageDataset.h"

#include "ui_SettingsWidget.h"

#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

SettingsWidget::SettingsWidget(ImageDatasetsModel* imageDatasetsModel) :
	QWidget(),
	_imageDatasetsModel(imageDatasetsModel),
	_ui{ std::make_unique<Ui::SettingsWidget>() }
{
	_ui->setupUi(this);
	
	_ui->datasetsTableView->setModel(_imageDatasetsModel);

	QObject::connect(&_imageDatasetsModel->selectionModel(), &QItemSelectionModel::currentChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		qDebug() << current << previous;
	});

	//_ui->datasetsComboBox->setse
	//_imageDatasetsModel->

	/*
	connect(_ui->datasetsComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _imageDatasetsModel, &ImageDatasetsModel::setCurrentDatasetName);

	connect(_ui->dimensionsComboBox, QOverload<const int>::of(&QComboBox::currentIndexChanged), [&](const int currentDimensionIndex) {
		_imageDatasetsModel->currentDataset()->setCurrentDimensionIndex(currentDimensionIndex);
	});

	connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, this, [&](int state) {
		_imageDatasetsModel->currentDataset()->setAverage(static_cast<bool>(state));
	});
	
	connect(_ui->selectionOpacitySlider, &QSlider::valueChanged, _imageDatasetsModel, [&](int value) {
		_imageDatasetsModel->currentDataset()->setSelectionOpacity(static_cast<float>(value) / 100.f);
	});
	
	connect(_ui->createSubsetFromSelectionPushButton, &QCheckBox::clicked, [&]() {
		_imageDatasetsModel->currentDataset()->createSubsetFromSelection();
	});
	
	connect(_imageDatasetsModel, &ImageDatasetsModel::namesChanged, this, &SettingsWidget::onDatasetNamesChanged);
	connect(_imageDatasetsModel, &ImageDatasetsModel::currentDatasetChanged, this, &SettingsWidget::onCurrentDatasetChanged);
	*/
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

void SettingsWidget::onCurrentDatasetChanged(ImageDataset* previousImageDataset, ImageDataset* currentImageDataset)
{
	_ui->datasetsComboBox->blockSignals(true);
	_ui->datasetsComboBox->setCurrentText(currentImageDataset->name());
	_ui->datasetsComboBox->blockSignals(false);

	_ui->averageImagesCheckBox->setEnabled(currentImageDataset->canAverage());

	_ui->averageImagesCheckBox->blockSignals(true);
	_ui->averageImagesCheckBox->setChecked(currentImageDataset->average());
	_ui->averageImagesCheckBox->blockSignals(false);

	const auto enable = currentImageDataset->imageNames().size() > 0 && !currentImageDataset->average();

	_ui->imagesComboBox->blockSignals(true);

	_ui->imagesComboBox->clear();
	_ui->imagesComboBox->addItems(currentImageDataset->imageNames());
	_ui->imagesComboBox->setEnabled(enable);
	_ui->imagesComboBox->blockSignals(false);

	const auto dimensionNames = currentImageDataset->dimensionNames();

	_ui->dimensionsComboBox->blockSignals(true);

	_ui->dimensionsComboBox->clear();
	_ui->dimensionsComboBox->addItems(currentImageDataset->dimensionNames());

	switch (currentImageDataset->imageCollectionType())
	{
		case ImageCollectionType::Sequence:
		{
			_ui->dimensionsComboBox->setEnabled(dimensionNames.size() > 0 && !currentImageDataset->average());
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

	_ui->currentDimensionLabel->setEnabled(dimensionNames.size() > 0 && !currentImageDataset->average());

	_ui->selectionOpacitySlider->blockSignals(true);
	_ui->selectionOpacitySlider->setValue(currentImageDataset->selectionOpacity() * 100.0f);
	_ui->selectionOpacitySlider->blockSignals(false);

	if (currentImageDataset->imageCollectionType() == ImageCollectionType::Sequence) {
		_ui->createSubsetFromSelectionPushButton->setEnabled(false);
		_ui->selectionOpacityLabel->setEnabled(false);
		_ui->selectionOpacitySlider->setEnabled(false);
	}
	else {
		const auto hasSelection = currentImageDataset->hasSelection();

		_ui->createSubsetFromSelectionPushButton->setEnabled(hasSelection);
		_ui->selectionOpacityLabel->setEnabled(hasSelection);
		_ui->selectionOpacitySlider->setEnabled(hasSelection);
	}

	if (previousImageDataset != nullptr) {
		disconnect(previousImageDataset, &ImageDataset::currentImageIndexChanged, this, nullptr);
		disconnect(previousImageDataset, &ImageDataset::currentDimensionIndexChanged, this, nullptr);
		disconnect(previousImageDataset, &ImageDataset::averageChanged, this, nullptr);
		disconnect(previousImageDataset, &ImageDataset::selectionOpacityChanged, this, nullptr);
		disconnect(previousImageDataset, &ImageDataset::selectionChanged, this, nullptr);
	}

	if (currentImageDataset != nullptr) {
		connect(currentImageDataset, &ImageDataset::currentImageIndexChanged, this, [&](const std::int32_t& currentImageIndex) {
			_ui->imagesComboBox->blockSignals(true);
			_ui->imagesComboBox->setCurrentIndex(currentImageIndex);
			_ui->imagesComboBox->blockSignals(false);
		});

		connect(currentImageDataset, &ImageDataset::currentDimensionIndexChanged, this, [&](const std::int32_t& currentDimensionIndex) {
			_ui->dimensionsComboBox->blockSignals(true);
			_ui->dimensionsComboBox->setCurrentIndex(currentDimensionIndex);
			_ui->dimensionsComboBox->blockSignals(false);
		});

		connect(currentImageDataset, &ImageDataset::averageChanged, this, [&](const bool& average) {
			_ui->averageImagesCheckBox->blockSignals(true);
			_ui->averageImagesCheckBox->setChecked(average);
			_ui->averageImagesCheckBox->blockSignals(false);
		});

		connect(currentImageDataset, &ImageDataset::selectionOpacityChanged, this, [&](const float& selectionOpacity) {
			_ui->selectionOpacitySlider->blockSignals(true);
			_ui->selectionOpacitySlider->setValue(selectionOpacity * 100.0f);
			_ui->selectionOpacitySlider->blockSignals(false);
		});

		/*
		connect(currentImageDataset, &ImageDataset::selectionChanged, this, [&]() {
			if (_imageDatasetsModel->currentDataset()->imageCollectionType() == ImageCollectionType::Sequence) {
				_ui->createSubsetFromSelectionPushButton->setEnabled(false);
				_ui->selectionOpacityLabel->setEnabled(false);
				_ui->selectionOpacitySlider->setEnabled(false);
			}
			else {
				const auto hasSelection = _imageDatasetsModel->currentDataset()->hasSelection();

				_ui->createSubsetFromSelectionPushButton->setEnabled(hasSelection);
				_ui->selectionOpacityLabel->setEnabled(hasSelection);
				_ui->selectionOpacitySlider->setEnabled(hasSelection);
			}
		});
		*/
	}
}