#include "WindowLevelWidget.h"
#include "LayersWidget.h"
#include "ImageViewerPlugin.h"

#include "ui_WindowLevelWidget.h"

#include <QDebug>
#include <QFrame>

WindowLevelWidget::WindowLevelWidget(QWidget* parent, ImageViewerPlugin* imageViewerPlugin, const QModelIndex& windowIndex, const QModelIndex& levelIndex) :
	QWidget(parent),
	_imageViewerPlugin(imageViewerPlugin),
	_ui{ std::make_unique<Ui::WindowLevelWidget>() },
	_windowIndex(windowIndex),
	_levelIndex(levelIndex)
{
	setWindowFlags(Qt::Popup);
	
	setObjectName("WindowLevelWidget");
	setStyleSheet("QWidget#WindowLevelWidget { border: 1px solid grey; }");

	_ui->setupUi(this);

	move(parent->mapToGlobal(parent->rect().bottomRight()) - QPoint(width(), 0));

	QObject::connect(&_imageViewerPlugin->getLayersModel(), &LayersModel::dataChanged, this, &WindowLevelWidget::updateData);

	auto& layersModel = _imageViewerPlugin->getLayersModel();

	QObject::connect(_ui->windowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [&](double value) {
		layersModel.setData(_windowIndex, value);
	});

	QObject::connect(_ui->windowHorizontalSlider, &QSlider::valueChanged, [&](int value) {
		layersModel.setData(_windowIndex, static_cast<float>(value) / 100.0f);
	});

	QObject::connect(_ui->levelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [&](double value) {
		layersModel.setData(_levelIndex, value);
	});

	QObject::connect(_ui->levelHorizontalSlider, &QSlider::valueChanged, [&](int value) {
		layersModel.setData(_levelIndex, static_cast<float>(value) / 100.0f);
	});

	QObject::connect(_ui->resetPushButton, &QPushButton::clicked, [&]() {
		layersModel.setData(_windowIndex, 1.0f);
		layersModel.setData(_levelIndex, 0.5f);
	});

	updateData(_windowIndex, _windowIndex);
	updateData(_levelIndex, _levelIndex);
}

WindowLevelWidget::~WindowLevelWidget() = default;

void WindowLevelWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	auto& layersModel = _imageViewerPlugin->getLayersModel();

	const auto window	= layersModel.data(_windowIndex, Qt::EditRole).toFloat();
	const auto level	= layersModel.data(_levelIndex, Qt::EditRole).toFloat();

	_ui->resetPushButton->setEnabled(window != 1.0f || level != 0.5f);

	if (topLeft == _windowIndex) {
		_ui->windowDoubleSpinBox->blockSignals(true);
		_ui->windowDoubleSpinBox->setValue(window);
		_ui->windowDoubleSpinBox->blockSignals(false);

		_ui->windowHorizontalSlider->blockSignals(true);
		_ui->windowHorizontalSlider->setValue(100.0f * window);
		_ui->windowHorizontalSlider->blockSignals(false);
	}

	if (topLeft == _levelIndex) {
		_ui->levelDoubleSpinBox->blockSignals(true);
		_ui->levelDoubleSpinBox->setValue(level);
		_ui->levelDoubleSpinBox->blockSignals(false);

		_ui->levelHorizontalSlider->blockSignals(true);
		_ui->levelHorizontalSlider->setValue(100.0f * level);
		_ui->levelHorizontalSlider->blockSignals(false);
	}
}