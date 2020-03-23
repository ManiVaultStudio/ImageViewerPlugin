#include "ImagesLayerWidget.h"
#include "LayersModel.h"
#include "ImagesLayer.h"

#include "ui_ImagesLayerWidget.h"

#include <QStringListModel>
#include <QDebug>

ImagesLayerWidget::ImagesLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::ImagesLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void ImagesLayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImagesLayerWidget::onDataChanged);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection &selected, const QItemSelection &deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (!selectedRows.isEmpty())
			onDataChanged(_layersModel->index(selectedRows.first().row(), 0), _layersModel->index(selectedRows.first().row(), _layersModel->columnCount() - 1));
	});
}

void ImagesLayerWidget::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	/*
	const auto selectedRows			= _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows		= selectedRows.size();
	const auto singleRowSelection	= noSelectedRows == 1;

	auto columns = QSet<int>();

	for (int c = topLeft.column(); c <= bottomRight.column(); c++) {
		columns.insert(c);
	}

	if (columns.contains(to_underlying(ImagesLayer::Column::FilteredImageNames))) {
		const auto filteredImageNames = _layersModel->data(row, );
	}
	*/
}