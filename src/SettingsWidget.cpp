#include "SettingsWidget.h"
#include "ImageDatasetsModel.h"
#include "ImageDataset.h"

#include "ui_SettingsWidget.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QDebug>

SettingsWidget::SettingsWidget(ImageDatasetsModel* imageDatasetsModel) :
	QWidget(),
	_imageDatasetsModel(imageDatasetsModel),
	_ui{ std::make_unique<Ui::SettingsWidget>() },
	_dataWidgetMapper(new QDataWidgetMapper(this))
{
	_ui->setupUi(this);
	
	_ui->datasetsComboBox->setModel(_imageDatasetsModel);

	_dataWidgetMapper->setModel(_imageDatasetsModel);
	_dataWidgetMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

	_dataWidgetMapper->addMapping(_ui->currentImageComboBox, static_cast<int>(ImageDatasetsModel::Columns::ImageNames), "currentIndex");
	_dataWidgetMapper->addMapping(_ui->averageImagesCheckBox, static_cast<int>(ImageDatasetsModel::Columns::AverageImages));// , "checked");
	_dataWidgetMapper->toFirst();

	QObject::connect(_ui->previousPushButton, &QAbstractButton::clicked, _dataWidgetMapper, &QDataWidgetMapper::toPrevious);
	QObject::connect(_ui->nextPushButton, &QAbstractButton::clicked, _dataWidgetMapper, &QDataWidgetMapper::toNext);
	
	QObject::connect(_dataWidgetMapper, &QDataWidgetMapper::currentIndexChanged, this, [this](int row) {
		_ui->previousPushButton->setEnabled(row > 0);
		_ui->nextPushButton->setEnabled(row < _imageDatasetsModel->rowCount(QModelIndex()) - 1);

		const auto index	= _imageDatasetsModel->index(row, static_cast<int>(ImageDatasetsModel::Columns::ImageNames));;
		const auto data		= _imageDatasetsModel->data(index, Qt::EditRole);

		//_ui->currentImageComboBox->setModel(new QStringListModel(data.toStringList()));

		_ui->currentImageComboBox->setModel(_imageDatasetsModel);

		const auto id = _imageDatasetsModel->index(row, static_cast<int>(ImageDatasetsModel::Columns::ImageNames));
		_ui->currentImageComboBox->setRootModelIndex(id);
	});

	QObject::connect(_ui->datasetsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), _dataWidgetMapper, &QDataWidgetMapper::setCurrentIndex);
	QObject::connect(_dataWidgetMapper, &QDataWidgetMapper::currentIndexChanged, _ui->datasetsComboBox, &QComboBox::setCurrentIndex);
}

SettingsWidget::~SettingsWidget() = default;

void SettingsWidget::onCurrentDatasetChanged(ImageDataset* previousImageDataset, ImageDataset* currentImageDataset)
{
	/*
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
	}
	*/
}