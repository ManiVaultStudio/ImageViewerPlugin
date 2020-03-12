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

	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::Type));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::Name));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoImages));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::Size));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoPoints));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::NoDimensions));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageNames));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::ImageFilePaths));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImage));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImageName));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::CurrentImageFilepath));
	//headerView->hideSection(static_cast<int>(DatasetsModel::Columns::Average));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::PointsName));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::Selection));
	headerView->hideSection(static_cast<int>(DatasetsModel::Columns::SelectionSize));
	
	headerView->setSectionResizeMode(DatasetsModel::Columns::Name, QHeaderView::Interactive);

	QObject::connect(datasetsModel, &DatasetsModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(true);
	}, Qt::QueuedConnection);

	QObject::connect(datasetsModel, &DatasetsModel::rowsRemoved, this, [this](const QModelIndex& parent, int first, int last) {
		_ui->selectionGroupBox->setEnabled(_datasetsModel->rowCount(QModelIndex()) > 0);
	}, Qt::QueuedConnection);

	QObject::connect(_ui->currentImageComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_datasetsModel->setData(_datasetsModel->selectionModel()->currentIndex().row(), DatasetsModel::Columns::CurrentImage, currentIndex);
	});

	QObject::connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_datasetsModel->setData(_datasetsModel->selectionModel()->currentIndex().row(), DatasetsModel::Columns::Average, state);
	});

	QObject::connect(_datasetsModel->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		updateData(_datasetsModel->index(current.row(), 0), _datasetsModel->index(current.row(), _datasetsModel->columnCount()));
	});

	QObject::connect(datasetsModel, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
		_ui->currentImageComboBox->setToolTip(_datasetsModel->data(topLeft.row(), DatasetsModel::Columns::CurrentImageFilepath, Qt::ToolTipRole).toString());
		_ui->averageImagesCheckBox->setToolTip(QString("Images will%1be averaged").arg(_datasetsModel->data(topLeft.row(), DatasetsModel::Columns::Average, Qt::EditRole).toBool() ? " " : " not "));
	}, Qt::QueuedConnection);

	QObject::connect(_datasetsModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection &selected, const QItemSelection &deselected) {
		const auto selectedRows = _datasetsModel->selectionModel()->selectedRows();
		updateData(_datasetsModel->index(selectedRows.first().row(), 0), _datasetsModel->index(selectedRows.first().row(), _datasetsModel->columnCount() - 1));
	});

	QObject::connect(_datasetsModel, &DatasetsModel::dataChanged, this, &SelectionWidget::updateData);
}

SelectionWidget::~SelectionWidget() = default;

void SelectionWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	const auto selectedRows	= _datasetsModel->selectionModel()->selectedRows();
	const auto mightEdit	= selectedRows.size() == 1;
	const auto average		= _datasetsModel->data(topLeft.row(), DatasetsModel::Columns::Average, Qt::EditRole).toBool();

	const auto currentImageFlags	= mightEdit ? _datasetsModel->flags(_datasetsModel->index(topLeft.row(), static_cast<int>(DatasetsModel::Columns::CurrentImage))) : 0;
	const auto averageImagesFlags	= _datasetsModel->flags(_datasetsModel->index(topLeft.row(), static_cast<int>(DatasetsModel::Columns::Average)));

	_ui->currentImageLabel->setEnabled(mightEdit);
	_ui->currentImageComboBox->setEnabled(mightEdit && !average);

	const auto currentImageChanged	= topLeft.column() <= DatasetsModel::Columns::CurrentImage && bottomRight.column() >= DatasetsModel::Columns::CurrentImage;
	const auto imageNamesChanged	= topLeft.column() <= DatasetsModel::Columns::ImageNames && bottomRight.column() >= DatasetsModel::Columns::ImageNames;

	const auto type = _datasetsModel->data(topLeft.row(), DatasetsModel::Columns::Type, Qt::EditRole).toInt();

	if (currentImageChanged || imageNamesChanged) {
		_ui->currentImageComboBox->blockSignals(true);
		_ui->currentImageComboBox->setModel(new QStringListModel(mightEdit ? _datasetsModel->data(topLeft.row(), DatasetsModel::Columns::ImageNames, Qt::EditRole).toStringList() : QStringList()));
		_ui->currentImageComboBox->setCurrentText(mightEdit ? _datasetsModel->data(topLeft.row(), DatasetsModel::Columns::CurrentImageName, Qt::EditRole).toString() : "");
		_ui->currentImageComboBox->blockSignals(false);
	}

	_ui->averageImagesCheckBox->setEnabled(mightEdit && averageImagesFlags & Qt::ItemIsEditable);

	const auto averageImagesChanged = topLeft.column() <= DatasetsModel::Columns::Average && bottomRight.column() >= DatasetsModel::Columns::Average;

	if (averageImagesChanged) {
		_ui->averageImagesCheckBox->blockSignals(true);
		_ui->averageImagesCheckBox->setChecked(average);
		_ui->averageImagesCheckBox->blockSignals(false);
	}
}