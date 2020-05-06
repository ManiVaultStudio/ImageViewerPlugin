#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "SettingsWidget.h"
#include "LayersModel.h"
#include "LayerNode.h"
#include "Renderer.h"

#include "PointData.h"
#include "IndexSet.h"
#include "ImageData/Images.h"

#include <QFontDatabase>
#include <QItemSelectionModel>
#include <QFileInfo>
#include <QDebug>

using namespace hdps;

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_viewerWidget(),
	_settingsWidget(),
	_layersModel(this),
	_colorMapModel(this, ColorMap::Type::OneDimensional),
	_imagesDatasets()
{
	qRegisterMetaType<QVector<int> >("QVector<int>");

	LayerNode::imageViewerPlugin = this;

	// TODO
	_viewerWidget	= new ViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);
}

void ImageViewerPlugin::init()
{
	auto layout = new QHBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);
	
	setMainLayout(layout);

	addWidget(_viewerWidget);
	addWidget(_settingsWidget);

	layout->setStretchFactor(_viewerWidget, 1);

	_layersModel.initialize();
}

Images* ImageViewerPlugin::sourceImagesSetFromPointsSet(const QString& pointSetName)
{
	const auto pointsSet			= _core->requestData<Points>(pointSetName);
	const auto sourcePointsSet		= hdps::DataSet::getSourceData(pointsSet);
	const auto sourcePointsDataName	= sourcePointsSet.getDataName();

	auto originatesFromImages = false;

	for (auto imageDataset : _imagesDatasets) {
		Images& imagesSet = _core->requestData<Images>(imageDataset);

		if (imagesSet.points()->getDataName() == sourcePointsDataName)
			return &imagesSet;
	}

	return nullptr;
}

void ImageViewerPlugin::dataAdded(const QString dataset)
{
	auto images = dynamic_cast<Images*>(&_core->requestData(dataset));

	if (images != nullptr)
		_imagesDatasets << dataset;
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	const auto hits = _layersModel.match(_layersModel.index(0, ult(LayerNode::Column::DataName)), Qt::DisplayRole, dataset, -1, Qt::MatchExactly);

	for (auto hit : hits) {
		auto selection = dynamic_cast<Points&>(_core->requestSelection(dataset));
		_layersModel.setData(hit.siblingAtColumn(ult(LayerNode::Column::Selection)), QVariant::fromValue(Indices::fromStdVector(selection.indices)));
	}
}

hdps::DataTypes ImageViewerPlugin::supportedDataTypes() const
{
	hdps::DataTypes supportedTypes;
	supportedTypes.append(ImageType);
	return supportedTypes;
}

void ImageViewerPlugin::keyPressEvent(QKeyEvent* keyEvent)
{
	layersModel().keyPressEvent(keyEvent);

	ViewPlugin::keyPressEvent(keyEvent);
}

void ImageViewerPlugin::keyReleaseEvent(QKeyEvent* keyEvent)
{
	layersModel().keyReleaseEvent(keyEvent);

	ViewPlugin::keyReleaseEvent(keyEvent);
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
	return new ImageViewerPlugin();
}