#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "StatusbarWidget.h"
#include "SettingsWidget.h"
#include "LayersModel.h"
#include "Layer.h"
#include "PointsLayer.h"
#include "Renderer.h"

#include "PointData.h"

#include <QFontDatabase>
#include <QItemSelectionModel>
#include <QFileInfo>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

using namespace hdps;

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_viewerWidget(),
	_settingsWidget(),
	_layersModel(this),
	_colorMapModel(this, ColorMap::Type::OneDimensional),
	_pointsDatasets()
{
	Layer::imageViewerPlugin = this;
	
	_viewerWidget		= new ViewerWidget(this);
	_statusbarWidget	= new StatusbarWidget(this);
	_settingsWidget		= new SettingsWidget(this);
}

void ImageViewerPlugin::init()
{
	auto layout = new QHBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);
	
	setMainLayout(layout);

	auto viewerLayout = new QVBoxLayout();

	viewerLayout->addWidget(_viewerWidget);
	viewerLayout->addWidget(_statusbarWidget);

	layout->addLayout(viewerLayout);
	layout->addWidget(_settingsWidget);

	layout->setStretchFactor(_viewerWidget, 1);

	_layersModel.initialize();

	_statusbarWidget->initialize(this);
}

void ImageViewerPlugin::dataAdded(const QString dataset)
{
	auto pointsDataset = dynamic_cast<Points*>(&_core->requestData(dataset));

	if (pointsDataset != nullptr) {
		_pointsDatasets.insert(0, dataset);
		emit pointsDatasetsChanged(_pointsDatasets);
	}
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	const auto hits = _layersModel.match(_layersModel.index(0, ult(Layer::Column::DataName)), Qt::DisplayRole, dataset, -1, Qt::MatchExactly);

	for (auto hit : hits) {
		auto selection = dynamic_cast<Points&>(_core->requestSelection(dataset));
		const auto indices = hdps::fromStdVector<QVector<uint>>(selection.indices);
		_layersModel.setData(hit.siblingAtColumn(ult(Layer::Column::Selection)), QVariant::fromValue(indices));
	}

	emit selectionIndicesChanged(dataset);
}

hdps::DataTypes ImageViewerPlugin::supportedDataTypes() const
{
	hdps::DataTypes supportedTypes;
	
	supportedTypes.append(ImageType);
	supportedTypes.append(PointType);

	return supportedTypes;
}

bool ImageViewerPlugin::eventFilter(QObject* target, QEvent* event)
{
	switch (event->type())
	{
		case QEvent::KeyPress:
		{
			auto keyEvent = static_cast<QKeyEvent*>(event);

			if (!keyEvent->isAutoRepeat() && keyEvent->key() == Qt::Key_Space) {
				_layersModel.dispatchEventToSelectedLayer(event);
			}

			break;
		}

		case QEvent::KeyRelease:
		{
			auto keyEvent = static_cast<QKeyEvent*>(event);

			if (!keyEvent->isAutoRepeat() && keyEvent->key() == Qt::Key_Space) {
				_layersModel.dispatchEventToSelectedLayer(event);
			}

			break;
		}

		default:
			break;
	}

	return QWidget::eventFilter(target, event);
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
	return new ImageViewerPlugin();
}