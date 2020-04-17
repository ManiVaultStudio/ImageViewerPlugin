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
	_imageViewerWidget(),
	_settingsWidget(),
	_layersModel(this),
	_colorMapModel(this, ColorMap::Type::OneDimensional),
	_imagesDatasets()
{
	qRegisterMetaType<QVector<int> >("QVector<int>");

	//_root->st
	LayerNode::imageViewerPlugin = this;

	// TODO
	_imageViewerWidget	= new ViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	
	
	/*
	if (!QFontDatabase::addApplicationFont(":/FontAwesome.otf"))
		qDebug() << "Unable to load Font Awesome";
	*/

	/*
	QObject::connect(_layersModel.selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this](const QModelIndex &current, const QModelIndex &previous) {
		emit _layersModel.dataChanged(_layersModel.index(current.row(), DatasetsModel::Columns::ImageIds), _layersModel.index(current.row(), DatasetsModel::Columns::ImageIds));
	});

	QObject::connect(&_layersModel, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()
	) {
		const auto imageIdsChanged	= topLeft.column() <= DatasetsModel::Columns::ImageIds && bottomRight.column() >= DatasetsModel::Columns::ImageIds;

		if (imageIdsChanged) {
			const auto imageIds = _layersModel.data(topLeft.row(), DatasetsModel::Columns::ImageIds, Qt::EditRole).value<Indices>();
			const auto datasetName = _layersModel.data(topLeft.row(), DatasetsModel::Columns::Name, Qt::EditRole).toString();
			const auto type = _layersModel.data(topLeft.row(), DatasetsModel::Columns::Type, Qt::EditRole).toInt();

			auto imagesDataset = _core->requestData<Images>(datasetName);

			auto layersModel = _layersModel.layersModel(topLeft.row());

			if (!imageIds.isEmpty()) {
				switch (type)
				{
					case ImageData::Type::Sequence:
						layersModel->setDefaultColorImage(imagesDataset.sequenceImage(imageIds.toStdVector()));
						break;

					case ImageData::Type::Stack:
						layersModel->setDefaultColorImage(imagesDataset.stackImage(imageIds.first()));
						break;

					default:
						break;
				}
			}
		}
	});
	*/
}

void ImageViewerPlugin::init()
{
	auto layout = new QHBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);
	
	setMainLayout(layout);

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);

	layout->setStretchFactor(_imageViewerWidget, 1);

	_layersModel.initialize();
}

void ImageViewerPlugin::updateWindowTitle()
{
	QStringList properties;

	//properties << QString("dataset=%1").arg(_currentDatasetName);
	//properties << QString("image=%1").arg(_imageNames.size() > 0 ? _imageNames[_currentImageId] : "");
	//properties << QString("dimension=%1").arg(_dimensionNames.size() > 0 ? _dimensionNames[_currentDimensionId] : "");
	
	//auto imageQuad = _imageViewerWidget->renderer()->imageQuad();

	//const auto size = imageQuad->size();

	/*
	properties << QString("width=%1").arg(QString::number(size.width()));
	properties << QString("height=%1").arg(QString::number(size.height()));
	properties << QString("window=%1").arg(QString::number(imageQuad->windowNormalized(), 'f', 2));
	properties << QString("level=%1").arg(QString::number(imageQuad->levelNormalized(), 'f', 2));
	properties << QString("imageMin=%1").arg(QString::number(imageQuad->imageMin()));
	properties << QString("imageMax=%1").arg(QString::number(imageQuad->imageMax()));
	properties << QString("noSelectedPixels=%1").arg(QString::number(selection().size()));
	properties << QString("noPixels=%1").arg(QString::number(size.width() * size.height()));
	*/

	setWindowTitle(QString("%1").arg(properties.join(", ")));
}

Images* ImageViewerPlugin::sourceImagesSetFromPointsSet(const QString& pointSetName)
{
	const auto pointsSet			= _core->requestData<Points>(pointSetName);
	const auto sourcePointsSet		= hdps::DataSet::getSourceData(pointsSet);
	const auto sourcePointsDataName	= sourcePointsSet.getDataName();

	auto originatesFromImages = false;

	for (auto imageDataset : _imagesDatasets) {
		auto imagesSet = _core->requestData<Images>(imageDataset);

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
	const auto hits = _layersModel.match(_layersModel.index(0, ult(LayerNode::Column::RawDataName)), Qt::DisplayRole, dataset, -1, Qt::MatchExactly);

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

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
	return new ImageViewerPlugin();
}