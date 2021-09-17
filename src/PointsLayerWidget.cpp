//#include "PointsLayerWidget.h"
//#include "LayersModel.h"
//#include "PointsLayer.h"
//#include "ImageViewerPlugin.h"
//#include "WindowLevelWidget.h"
//
//#include "ui_PointsLayerWidget.h"
//
//#include "Application.h"
//
//#include <QDebug>
//#include <QStringListModel>
//#include <QWidgetAction>
//#include <QListView>
//
//PointsLayerWidget::PointsLayerWidget(QWidget* parent) :
//    QWidget(parent),
//    _ui{ std::make_unique<Ui::PointsLayerWidget>() },
//    _layersModel(nullptr)
//{
//    _ui->setupUi(this);
//}
//
//void PointsLayerWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
//{
//    _imageViewerPlugin = imageViewerPlugin;
//    _layersModel = &_imageViewerPlugin->getLayersModel();
//    
//    const auto fontAwesome = hdps::Application::getIconFont("FontAwesome").getFont(9);
//    
//    _ui->channel1WindowLevelPushButton->setFont(fontAwesome);
//    _ui->channel2WindowLevelPushButton->setFont(fontAwesome);
//    _ui->channel3WindowLevelPushButton->setFont(fontAwesome);
//
//    const auto windowLevelIconCharacter = hdps::Application::getIconFont("FontAwesome").getIconCharacter("adjust");
//
//    _ui->channel1WindowLevelPushButton->setText(windowLevelIconCharacter);
//    _ui->channel2WindowLevelPushButton->setText(windowLevelIconCharacter);
//    _ui->channel3WindowLevelPushButton->setText(windowLevelIconCharacter);
//
//    _ui->channel1ProbePushButton->setFont(fontAwesome);
//    _ui->channel2ProbePushButton->setFont(fontAwesome);
//    _ui->channel3ProbePushButton->setFont(fontAwesome);
//
//    const auto eyeDropperIconCharacter = hdps::Application::getIconFont("FontAwesome").getIconCharacter("eye-dropper");
//    
//    _ui->channel1ProbePushButton->setText(eyeDropperIconCharacter);
//    _ui->channel2ProbePushButton->setText(eyeDropperIconCharacter);
//    _ui->channel3ProbePushButton->setText(eyeDropperIconCharacter);
//
//    _ui->channel1ProbePushButton->setVisible(false);
//    _ui->channel2ProbePushButton->setVisible(false);
//    _ui->channel3ProbePushButton->setVisible(false);
//
//    QObject::connect(_layersModel, &LayersModel::dataChanged, this, &PointsLayerWidget::updateData);
//
//    QObject::connect(&_layersModel->getSelectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
//        const auto selectedRows = _layersModel->getSelectionModel().selectedRows();
//
//        //_ui->channel1Label->setToolTip();
//
//        if (selectedRows.isEmpty())
//            updateData(QModelIndex(), QModelIndex());
//        else {
//            const auto first = selectedRows.first();
//            
//            if (selectedRows.count() == 1 && _layersModel->data(first.siblingAtColumn(ult(Layer::Column::Type)), Qt::EditRole) == ult(Layer::Type::Points))
//                updateData(first.siblingAtColumn(ult(PointsLayer::Column::Start)), first.siblingAtColumn(ult(PointsLayer::Column::End)));
//        }
//    });
//
//    const auto showWindowLevelWidget = [&, this](QWidget* parent, const QModelIndex& windowIndex, const QModelIndex& levelIndex) {
//        auto windowLevelWidget = new WindowLevelWidget(parent, _imageViewerPlugin, windowIndex, levelIndex);
//
//        windowLevelWidget->show();
//    };
//
//    QObject::connect(_ui->channel1WindowLevelPushButton, &QPushButton::clicked, [&, this, showWindowLevelWidget]() {
//        const auto selectedRow  = _layersModel->getSelectionModel().selectedRows().first();
//        const auto windowIndex  = selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel1Window));
//        const auto levelIndex   = selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel1Level));
//
//        showWindowLevelWidget(_ui->channel1WindowLevelPushButton, windowIndex, levelIndex);
//    });
//
//    QObject::connect(_ui->channel2WindowLevelPushButton, &QPushButton::clicked, [&, this, showWindowLevelWidget]() {
//        const auto selectedRow  = _layersModel->getSelectionModel().selectedRows().first();
//        const auto windowIndex  = selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel2Window));
//        const auto levelIndex   = selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel2Level));
//
//        showWindowLevelWidget(_ui->channel2WindowLevelPushButton, windowIndex, levelIndex);
//    });
//
//    QObject::connect(_ui->channel3WindowLevelPushButton, &QPushButton::clicked, [&, this, showWindowLevelWidget]() {
//        const auto selectedRow  = _layersModel->getSelectionModel().selectedRows().first();
//        const auto windowIndex  = selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel3Window));
//        const auto levelIndex   = selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel3Level));
//
//        showWindowLevelWidget(_ui->channel3WindowLevelPushButton, windowIndex, levelIndex);
//    });
//
//    QObject::connect(_ui->channel2CheckBox, &QCheckBox::stateChanged, [this](int state) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2Enabled)), state == Qt::Checked);
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)), _ui->channel2ComboBox->currentIndex());
//    });
//
//    QObject::connect(_ui->channel3CheckBox, &QCheckBox::stateChanged, [this](int state) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3Enabled)), state == Qt::Checked);
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)), _ui->channel3ComboBox->currentIndex());
//    });
//
//    QObject::connect(_ui->channel1ComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel1DimensionId)), index);
//    });
//
//    QObject::connect(_ui->channel2ComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)), index);
//    });
//
//    QObject::connect(_ui->channel3ComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)), index);
//    });
//
//	QObject::connect(_ui->interpolationTypeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
//		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::InterpolationType)), index);
//	});
//
//    QObject::connect(_ui->constantColorCheckBox, &QCheckBox::stateChanged, [this](int state) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::UseConstantColor)), _ui->constantColorCheckBox->isChecked());
//    });
//
//    QObject::connect(_ui->colorSpaceComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::ColorSpace)), index);
//    });
//
//    QObject::connect(_ui->colorPickerPushButton, &ColorPickerPushButton::colorChanged, [this](const QColor& currentColor) {
//        _layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::ConstantColor)), currentColor);
//    });
//}
//
//void PointsLayerWidget::updateData(const QModelIndex& begin, const QModelIndex& end, const QVector<int>& roles /*= QVector<int>()*/)
//{
//    const auto selectedRows = _layersModel->getSelectionModel().selectedRows();
//    const auto noSelectedRows = selectedRows.size();
//
//    if (noSelectedRows != 1)
//        return;
//
//    if (selectedRows.first().row() != begin.row())
//        return;
//
//    const auto enabled = begin.siblingAtColumn(ult(Layer::Column::Name)).data(Qt::CheckStateRole).toInt() == Qt::Checked;
//
//    for (int column = begin.column(); column <= end.column(); column++) {
//        const auto index = begin.siblingAtColumn(column);
//
//        auto validSelection = false;
//        auto flags = 0;
//
//        if (index.isValid() && noSelectedRows == 1) {
//            validSelection = true;
//            flags = begin.siblingAtColumn(ult(Layer::Column::Flags)).data(Qt::EditRole).toInt();
//        }
//
//        const auto mightEdit = validSelection && enabled;
//
//        _ui->groupBox->setEnabled(enabled);
//
//        if (column == ult(PointsLayer::Column::DimensionNames)) {
//            const auto dimensionNames = begin.siblingAtColumn(ult(PointsLayer::Column::DimensionNames)).data(Qt::EditRole).toStringList();
//
//            auto dimensionNamesModel = new QStringListModel(dimensionNames, this);
//
//            _ui->channel1ComboBox->blockSignals(true);
//            _ui->channel1ComboBox->setModel(dimensionNamesModel);
//            _ui->channel1ComboBox->blockSignals(false);
//
//            _ui->channel2ComboBox->blockSignals(true);
//            _ui->channel2ComboBox->setModel(dimensionNamesModel);
//            _ui->channel2ComboBox->blockSignals(false);
//
//            _ui->channel3ComboBox->blockSignals(true);
//            _ui->channel3ComboBox->setModel(dimensionNamesModel);
//            _ui->channel3ComboBox->blockSignals(false);
//        }
//
//        if (column == ult(PointsLayer::Column::Channel1Name)) {
//            const auto channel1Name = begin.siblingAtColumn(ult(PointsLayer::Column::Channel1Name));
//
//            _ui->channel1Label->setEnabled(channel1Name.flags() & Qt::ItemIsEditable);
//            _ui->channel1Label->setText(channel1Name.data(Qt::EditRole).toString());
//        }
//
//        if (column == ult(PointsLayer::Column::Channel2Name)) {
//            const auto channel2Name = begin.siblingAtColumn(ult(PointsLayer::Column::Channel2Name));
//            
//            _ui->channel2Label->setEnabled(channel2Name.flags() & Qt::ItemIsEditable);
//            _ui->channel2Label->setText(channel2Name.data(Qt::EditRole).toString());
//        }
//
//        if (column == ult(PointsLayer::Column::Channel3Name)) {
//            const auto channel3Name = begin.siblingAtColumn(ult(PointsLayer::Column::Channel3Name));
//            
//            _ui->channel3Label->setEnabled(channel3Name.flags() & Qt::ItemIsEditable);
//            _ui->channel3Label->setText(channel3Name.data(Qt::EditRole).toString());
//        }
//
//        if (column == ult(PointsLayer::Column::Channel1DimensionId)) {
//            const auto channel1DimensionId  = begin.siblingAtColumn(ult(PointsLayer::Column::Channel1DimensionId));
//            const auto channelEnabled       = channel1DimensionId.flags() & Qt::ItemIsEditable;
//
//            _ui->channel1Label->setEnabled(channelEnabled);
//            _ui->channel1ComboBox->setEnabled(channelEnabled);
//            _ui->channel1ComboBox->blockSignals(true);
//            _ui->channel1ComboBox->setCurrentIndex(channel1DimensionId.data(Qt::EditRole).toInt());
//            _ui->channel1ComboBox->blockSignals(false);
//            _ui->channel1WindowLevelPushButton->setEnabled(channelEnabled);
//            _ui->channel1ProbePushButton->setEnabled(channelEnabled);
//        }
//
//        if (column == ult(PointsLayer::Column::Channel2DimensionId)) {
//            const auto channel2DimensionId  = begin.siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId));
//            const auto channelEnabled       = channel2DimensionId.flags() & Qt::ItemIsEditable;
//            const auto value = channel2DimensionId.data(Qt::EditRole).toInt();
//
//            _ui->channel2Label->setEnabled(channelEnabled);
//            _ui->channel2ComboBox->setEnabled(channelEnabled);
//            _ui->channel2ComboBox->blockSignals(true);
//            _ui->channel2ComboBox->setCurrentIndex(value);
//            _ui->channel2ComboBox->blockSignals(false);
//            _ui->channel2WindowLevelPushButton->setEnabled(channelEnabled);
//            _ui->channel2ProbePushButton->setEnabled(channelEnabled);
//        }
//
//        if (column == ult(PointsLayer::Column::Channel3DimensionId)) {
//            const auto channel3DimensionId  = begin.siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId));
//            const auto channelEnabled       = channel3DimensionId.flags() & Qt::ItemIsEditable;
//
//            _ui->channel3Label->setEnabled(channelEnabled);
//            _ui->channel3ComboBox->setEnabled(channelEnabled);
//            _ui->channel3ComboBox->blockSignals(true);
//            _ui->channel3ComboBox->setCurrentIndex(channel3DimensionId.data(Qt::EditRole).toInt());
//            _ui->channel3ComboBox->blockSignals(false);
//            _ui->channel3WindowLevelPushButton->setEnabled(channelEnabled);
//            _ui->channel3ProbePushButton->setEnabled(channelEnabled);
//        }
//
//        if (column == ult(PointsLayer::Column::Channel1Enabled)) {
//            const auto channel1Enabled = begin.siblingAtColumn(ult(PointsLayer::Column::Channel1Enabled));
//
//            _ui->channel1CheckBox->setEnabled(channel1Enabled.flags() & Qt::ItemIsEditable);
//            _ui->channel1CheckBox->blockSignals(true);
//            _ui->channel1CheckBox->setChecked(channel1Enabled.data(Qt::EditRole).toBool());
//            _ui->channel1CheckBox->blockSignals(false);
//        }
//
//        if (column == ult(PointsLayer::Column::Channel2Enabled)) {
//            const auto channel2Enabled = begin.siblingAtColumn(ult(PointsLayer::Column::Channel2Enabled));
//
//            _ui->channel2CheckBox->setEnabled(channel2Enabled.flags() & Qt::ItemIsEditable);
//            _ui->channel2CheckBox->blockSignals(true);
//            _ui->channel2CheckBox->setChecked(channel2Enabled.data(Qt::EditRole).toBool());
//            _ui->channel2CheckBox->blockSignals(false);
//        }
//
//        if (column == ult(PointsLayer::Column::Channel3Enabled)) {
//            const auto channel3Enabled = begin.siblingAtColumn(ult(PointsLayer::Column::Channel3Enabled));
//
//            _ui->channel3CheckBox->setEnabled(channel3Enabled.flags() & Qt::ItemIsEditable);
//            _ui->channel3CheckBox->blockSignals(true);
//            _ui->channel3CheckBox->setChecked(channel3Enabled.data(Qt::EditRole).toBool());
//            _ui->channel3CheckBox->blockSignals(false);
//        }
//
//        if (column == ult(PointsLayer::Column::ColorSpace)) {
//            const auto colorSpace = begin.siblingAtColumn(ult(PointsLayer::Column::ColorSpace));
//
//            _ui->colorSpaceLabel->setEnabled(colorSpace.flags() & Qt::ItemIsEditable);
//            _ui->colorSpaceComboBox->setEnabled(colorSpace.flags() & Qt::ItemIsEditable);
//
//            _ui->colorSpaceComboBox->blockSignals(true);
//            _ui->colorSpaceComboBox->setCurrentText(colorSpace.data(Qt::DisplayRole).toString());
//            _ui->colorSpaceComboBox->blockSignals(false);
//        }
//
//        if (column == ult(PointsLayer::Column::ColorMap)) {
//            const auto colorMapFlags = begin.siblingAtColumn(ult(PointsLayer::Column::ColorMap)).flags();
//
//            _ui->colormapLabel->setEnabled(colorMapFlags & Qt::ItemIsEditable);
//            _ui->colorMapComboBox->setEnabled(colorMapFlags & Qt::ItemIsEditable);
//
//            switch (begin.siblingAtColumn(ult(PointsLayer::Column::NoChannels)).data(Qt::EditRole).toInt())
//            {
//                case 1:
//                {
//                    if (begin.siblingAtColumn(ult(PointsLayer::Column::UseConstantColor)).data(Qt::EditRole).toBool()) {
//                        _ui->colormapLabel->setText("Constant color");
//                    }
//                    else {
//                        _ui->colormapLabel->setText("1D color map");
//                        _ui->colorMapComboBox->setType(ColorMap::Type::OneDimensional);
//                    }
//
//                    break;
//                }
//
//                case 2:
//                {
//                    _ui->colormapLabel->setText("2D Colormap");
//                    _ui->colorMapComboBox->setType(ColorMap::Type::TwoDimensional);
//                    break;
//                }
//
//                case 3:
//                {
//                    _ui->colormapLabel->setText("Colormap");
//                    break;
//                }
//
//                default:
//                    break;
//            }
//        }
//
//		if (column == ult(PointsLayer::Column::InterpolationType)) {
//			const auto interpolationType = begin.siblingAtColumn(ult(PointsLayer::Column::InterpolationType));
//
//			QSignalBlocker blocker(_ui->interpolationTypeComboBox);
//
//			_ui->interpolationTypeComboBox->setEnabled(interpolationType.flags() & Qt::ItemIsEditable);
//			_ui->interpolationTypeComboBox->setCurrentIndex(interpolationType.data(Qt::EditRole).toInt());
//		}
//
//        if (column == ult(PointsLayer::Column::UseConstantColor)) {
//            const auto useConstantColor = begin.siblingAtColumn(ult(PointsLayer::Column::UseConstantColor));
//
//            _ui->constantColorCheckBox->setEnabled(useConstantColor.flags() & Qt::ItemIsEditable);
//            _ui->constantColorCheckBox->blockSignals(true);
//            _ui->constantColorCheckBox->setChecked(useConstantColor.data(Qt::EditRole).toBool());
//            _ui->constantColorCheckBox->blockSignals(false);
//
//            _ui->colorStackedWidget->setCurrentIndex(useConstantColor.data(Qt::EditRole).toBool() ? 1 : 0);
//        }
//
//        if (column == ult(PointsLayer::Column::ConstantColor)) {
//            const auto constantColor = begin.siblingAtColumn(ult(PointsLayer::Column::ConstantColor));
//
//            _ui->colorPickerPushButton->blockSignals(true);
//            _ui->colorPickerPushButton->setColor(constantColor.data(Qt::EditRole).value<QColor>());
//            _ui->colorPickerPushButton->blockSignals(false);
//        }
//    }
//}