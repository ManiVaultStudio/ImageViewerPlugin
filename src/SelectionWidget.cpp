#include "SelectionWidget.h"
#include "DatasetsModel.h"

#include "ui_SelectionWidget.h"

#include <QStringListModel>
#include <QDebug>

SelectionWidget::SelectionWidget(QWidget* parent, DatasetsModel* datasetsModel) :
	QWidget(parent),
	_datasetsModel(datasetsModel),
	_ui{ std::make_unique<Ui::SelectionWidget>() }
{
	_ui->setupUi(this);
	
	auto selectionModel = _datasetsModel->selectionModel();

	_ui->datasetsTreeView->setModel(datasetsModel);
	_ui->datasetsTreeView->setSelectionModel(_datasetsModel->selectionModel());

	auto headerView = _ui->datasetsTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoDimensions));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoPoints));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImage));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimension));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::DimensionNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageFilePaths));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImageFilepath));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimensionFilepath));

	QObject::connect(datasetsModel, &DatasetsModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(true);
	}, Qt::QueuedConnection);

	QObject::connect(datasetsModel, &DatasetsModel::rowsRemoved, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(_datasetsModel->rowCount(QModelIndex()) > 0);
	}, Qt::QueuedConnection);

	QObject::connect(_ui->currentImageComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_datasetsModel->setCurrentImage(_datasetsModel->selectionModel()->currentIndex().row(), currentIndex);
	});

	QObject::connect(_ui->currentDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_datasetsModel->setCurrentDimension(_datasetsModel->selectionModel()->currentIndex().row(), currentIndex);
	});

	QObject::connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_datasetsModel->setAverageImages(_datasetsModel->selectionModel()->currentIndex().row(), state);
	});

	QObject::connect(selectionModel, &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		const auto currentImageFlags = _datasetsModel->flags(_datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentImage)));

		_ui->currentImageComboBox->blockSignals(true);
		_ui->currentImageLabel->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setModel(new QStringListModel(_datasetsModel->imageNames(current.row()).toStringList()));
		_ui->currentImageComboBox->setCurrentIndex(_datasetsModel->currentImage(current.row()).toInt());
		_ui->currentImageComboBox->blockSignals(false);

		const auto currentDimensionFlags = _datasetsModel->flags(_datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentDimension)));

		_ui->currentDimensionComboBox->blockSignals(true);
		_ui->currentDimensionLabel->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setModel(new QStringListModel(_datasetsModel->dimensionNames(current.row()).toStringList()));
		_ui->currentDimensionComboBox->setCurrentIndex(_datasetsModel->currentDimension(current.row()).toInt());
		_ui->currentDimensionComboBox->blockSignals(false);

		const auto averageImagesFlags = _datasetsModel->flags(_datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::AverageImages)));

		_ui->averageImagesCheckBox->blockSignals(true);
		_ui->averageImagesCheckBox->setEnabled(averageImagesFlags & Qt::ItemIsEditable);
		_ui->averageImagesCheckBox->setChecked(_datasetsModel->averageImages(current.row()).toBool());
		_ui->averageImagesCheckBox->blockSignals(false);
	});

	QObject::connect(datasetsModel, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
		_ui->currentImageComboBox->setToolTip(_datasetsModel->currentImageFilepath(topLeft.row()).toString());
		_ui->currentDimensionComboBox->setToolTip(_datasetsModel->currentDimensionFilepath(topLeft.row()).toString());
		_ui->averageImagesCheckBox->setToolTip(QString("Images will%1be averaged").arg(_datasetsModel->averageImages(topLeft.row()).toBool() ? " " : " not "));
	}, Qt::QueuedConnection);
}

SelectionWidget::~SelectionWidget() = default;