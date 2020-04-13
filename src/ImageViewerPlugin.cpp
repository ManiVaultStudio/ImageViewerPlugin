#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "SettingsWidget.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QFontDatabase>
#include <QItemSelectionModel>
#include <QFileInfo>
#include <QDebug>

#include "IndexSet.h"

using namespace hdps;

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(),
	_settingsWidget(),
	_datasetsModel(this),
	_datasetsSelectionModel(&_datasetsModel),
	_layersModel(this),
	_colorMapModel(this, ColorMap::Type::OneDimensional)
{
	qRegisterMetaType<QVector<int> >("QVector<int>");

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

void ImageViewerPlugin::dataAdded(const QString dataset)
{
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	/*
	const auto hits = _layersModel.match(_layersModel.index(0, DatasetsModel::Columns::PointsName), Qt::DisplayRole, dataset, -1, Qt::MatchExactly);
	
	if (hits.isEmpty())
		return;

	const auto firstHit		= hits.first();
	const auto datasetType	= _layersModel.data(_layersModel.index(firstHit.row(), DatasetsModel::Columns::Type), Qt::EditRole).toInt();
	const auto datasetName	=_layersModel.data(_layersModel.index(firstHit.row(), DatasetsModel::Columns::Name), Qt::EditRole).toString();

	auto imagesDataset = _core->requestData<Images>(datasetName);

	_layersModel.setData(_layersModel.index(hits.first().row(), DatasetsModel::Columns::Selection), QVariant::fromValue(Indices::fromStdVector(imagesDataset.indices())));

	if (datasetType == ImageData::Type::Stack)
		_layersModel.layersModel(firstHit.row())->setDefaultSelectionImage(imagesDataset.selectionImage());
	*/
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