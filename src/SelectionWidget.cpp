#include "SelectionWidget.h"
#include "MainModel.h"
#include "DatasetsModel.h"
#include "LayersModel.h"
#include "ImageDataset.h"

#include "ui_SelectionWidget.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QDebug>

SelectionWidget::SelectionWidget(QWidget* parent, MainModel* mainModel) :
	QWidget(parent),
	_mainModel(mainModel),
	_ui{ std::make_unique<Ui::SelectionWidget>() }
{
	_ui->setupUi(this);
	
	auto datasetsModel	= _mainModel->datasetsModel();
	auto selectionModel = _mainModel->selectionModel();

	_ui->datasetsTreeView->setModel(datasetsModel);
	_ui->datasetsTreeView->setSelectionModel(_mainModel->selectionModel());

	auto headerView = _ui->datasetsTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoDimensions));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoPoints));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImage));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimension));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::DimensionNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageFilePaths));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImageFilepath));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimensionFilepath));

	QObject::connect(datasetsModel, &MainModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(true);
	}, Qt::QueuedConnection);

	QObject::connect(datasetsModel, &MainModel::rowsRemoved, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(_mainModel->rowCount(QModelIndex()) > 0);
	}, Qt::QueuedConnection);

	QObject::connect(_ui->currentImageComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_mainModel->datasetsModel()->setCurrentImage(_mainModel->selectionModel()->currentIndex().row(), currentIndex);
	});

	QObject::connect(_ui->currentDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_mainModel->datasetsModel()->setCurrentDimension(_mainModel->selectionModel()->currentIndex().row(), currentIndex);
	});

	QObject::connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_mainModel->datasetsModel()->setAverageImages(_mainModel->selectionModel()->currentIndex().row(), state);
	});

	QObject::connect(selectionModel, &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		auto datasetsModel = _mainModel->datasetsModel();

		const auto currentImageFlags = datasetsModel->flags(datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentImage)));

		_ui->currentImageComboBox->blockSignals(true);
		_ui->currentImageLabel->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setModel(new QStringListModel(datasetsModel->imageNames(current.row()).toStringList()));
		_ui->currentImageComboBox->setCurrentIndex(datasetsModel->currentImage(current.row()).toInt());
		_ui->currentImageComboBox->blockSignals(false);

		const auto currentDimensionFlags = datasetsModel->flags(datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentDimension)));

		_ui->currentDimensionComboBox->blockSignals(true);
		_ui->currentDimensionLabel->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setModel(new QStringListModel(datasetsModel->dimensionNames(current.row()).toStringList()));
		_ui->currentDimensionComboBox->setCurrentIndex(datasetsModel->currentDimension(current.row()).toInt());
		_ui->currentDimensionComboBox->blockSignals(false);

		const auto averageImagesFlags = datasetsModel->flags(datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::AverageImages)));

		_ui->averageImagesCheckBox->blockSignals(true);
		_ui->averageImagesCheckBox->setEnabled(averageImagesFlags & Qt::ItemIsEditable);
		_ui->averageImagesCheckBox->setChecked(datasetsModel->averageImages(current.row()).toBool());
		_ui->averageImagesCheckBox->blockSignals(false);
	});

	QObject::connect(datasetsModel, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
		auto datasetsModel = _mainModel->datasetsModel();

		_ui->currentImageComboBox->setToolTip(datasetsModel->currentImageFilepath(topLeft.row()).toString());
		_ui->currentDimensionComboBox->setToolTip(datasetsModel->currentDimensionFilepath(topLeft.row()).toString());
		_ui->averageImagesCheckBox->setToolTip(QString("Images will%1be averaged").arg(datasetsModel->averageImages(topLeft.row()).toBool() ? " " : " not "));
	}, Qt::QueuedConnection);
}

SelectionWidget::~SelectionWidget() = default;