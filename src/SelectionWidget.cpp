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
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImageName));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimension));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimensionName));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::DimensionNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageFilePaths));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImageFilepath));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentDimensionFilepath));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::PointsName));

	QObject::connect(datasetsModel, &DatasetsModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(true);
	}, Qt::QueuedConnection);

	QObject::connect(datasetsModel, &DatasetsModel::rowsRemoved, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(_datasetsModel->rowCount(QModelIndex()) > 0);
	}, Qt::QueuedConnection);

	QObject::connect(_ui->currentImageComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_datasetsModel->setData(_datasetsModel->selectionModel()->currentIndex().row(), DatasetsModel::Columns::CurrentImage, currentIndex);
	});

	QObject::connect(_ui->currentDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_datasetsModel->setData(_datasetsModel->selectionModel()->currentIndex().row(), DatasetsModel::Columns::CurrentDimension, currentIndex);
	});

	QObject::connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_datasetsModel->setData(_datasetsModel->selectionModel()->currentIndex().row(), DatasetsModel::Columns::AverageImages, state);
	});

	QObject::connect(selectionModel, &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		const auto currentImageFlags = _datasetsModel->flags(_datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentImage)));

		_ui->currentImageComboBox->blockSignals(true);
		_ui->currentImageLabel->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setModel(new QStringListModel(_datasetsModel->data(current.row(), DatasetsModel::Columns::ImageNames, Qt::EditRole).toStringList()));
		_ui->currentImageComboBox->setCurrentIndex(_datasetsModel->data(current.row(), DatasetsModel::Columns::CurrentImage, Qt::EditRole).toInt());
		_ui->currentImageComboBox->blockSignals(false);

		const auto currentDimensionFlags = _datasetsModel->flags(_datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentDimension)));

		_ui->currentDimensionComboBox->blockSignals(true);
		_ui->currentDimensionLabel->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setModel(new QStringListModel(_datasetsModel->data(current.row(), DatasetsModel::Columns::DimensionNames, Qt::EditRole).toStringList()));
		_ui->currentDimensionComboBox->setCurrentIndex(_datasetsModel->data(current.row(), DatasetsModel::Columns::CurrentDimension, Qt::EditRole).toInt());
		_ui->currentDimensionComboBox->blockSignals(false);

		const auto averageImagesFlags = _datasetsModel->flags(_datasetsModel->index(current.row(), static_cast<int>(DatasetsModel::Columns::AverageImages)));

		_ui->averageImagesCheckBox->blockSignals(true);
		_ui->averageImagesCheckBox->setEnabled(averageImagesFlags & Qt::ItemIsEditable);
		_ui->averageImagesCheckBox->setChecked(_datasetsModel->data(current.row(), DatasetsModel::Columns::AverageImages, Qt::EditRole).toBool());
		_ui->averageImagesCheckBox->blockSignals(false);
	});

	QObject::connect(datasetsModel, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
		_ui->currentImageComboBox->setToolTip(_datasetsModel->data(topLeft.row(), DatasetsModel::Columns::CurrentImageFilepath).toString());
		_ui->currentDimensionComboBox->setToolTip(_datasetsModel->data(topLeft.row(), DatasetsModel::Columns::CurrentDimensionFilepath).toString());
		_ui->averageImagesCheckBox->setToolTip(QString("Images will%1be averaged").arg(_datasetsModel->data(topLeft.row(), DatasetsModel::Columns::AverageImages, Qt::EditRole).toBool() ? " " : " not "));
	}, Qt::QueuedConnection);
}

SelectionWidget::~SelectionWidget() = default;