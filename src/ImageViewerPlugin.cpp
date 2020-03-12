#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "SettingsWidget.h"
#include "ImageDataset.h"
#include "DatasetsModel.h"
#include "LayersModel.h"
#include "Renderer.h"

#include "ImageData/Images.h"
#include "PointData.h"

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
	_datasetsModel(this)
{
	qRegisterMetaType<std::shared_ptr<QImage>>("std::shared_ptr<QImage>");

	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ViewerWidget(this, &_datasetsModel);
	_settingsWidget		= new SettingsWidget(this, &_datasetsModel);

	if (!QFontDatabase::addApplicationFont(":/FontAwesome.otf"))
		qDebug() << "Unable to load Font Awesome";

	QObject::connect(_datasetsModel.selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		const auto datasetName = _datasetsModel.data(current.row(), DatasetsModel::Columns::Name).toString();
		auto imagesDataset = _core->requestData<Images>(datasetName);
		_datasetsModel.layersModel(current.row())->setData(1, LayersModel::Columns::Image, imagesDataset.selectionImage());
	});

	QObject::connect(&_datasetsModel, &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/) {
		const auto imageIdsChanged	= topLeft.column() <= DatasetsModel::Columns::ImageIds && bottomRight.column() >= DatasetsModel::Columns::ImageIds;
		const auto imageIds			= _datasetsModel.data(topLeft.row(), DatasetsModel::Columns::ImageIds, Qt::EditRole).value<Indices>();
		const auto datasetName		= _datasetsModel.data(topLeft.row(), DatasetsModel::Columns::Name).toString();
		const auto type				= _datasetsModel.data(topLeft.row(), DatasetsModel::Columns::Type, Qt::EditRole).toInt();

		auto imagesDataset = _core->requestData<Images>(datasetName);

		auto layersModel = _datasetsModel.layersModel(topLeft.row());

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
	});
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

ViewerWidget* ImageViewerPlugin::imageViewerWidget()
{
	return _imageViewerWidget;
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
	qDebug() << "Data added" << dataset;

	auto imagesDataset = _core->requestData<Images>(dataset);

	auto imageDataset = ImageDataset(nullptr);

	imageDataset.setName(dataset);
	imageDataset.setType(imagesDataset.type());
	imageDataset.setSize(imagesDataset.imageSize());
	imageDataset.setNoPoints(imagesDataset.points()->getNumPoints());
	imageDataset.setNoDimensions(imagesDataset.points()->getNumDimensions());
	imageDataset.setCurrentImage(0);
	imageDataset.setAverage(false);

	auto imageFilePaths = QStringList();

	for (const auto& imageFilePath : imagesDataset.imageFilePaths()) {
		imageFilePaths << imageFilePath;
	}

	switch (imagesDataset.type())
	{
		case ImageData::Type::Sequence:
		{
			auto imageNames = QStringList();

			for (const auto& imageFilePath : imagesDataset.imageFilePaths()) {
				imageNames << QFileInfo(imageFilePath).fileName();
			}

			imageDataset.setImageNames(imageNames);
			imageDataset.addLayer("default_color", "Color", Layer::Type::Image, Layer::Flags::Enabled | Layer::Flags::Enabled);
			break;
		}

		case ImageData::Type::Stack:
		{
			auto dimensionNames = QStringList();

			for (const auto& dimensionName : imagesDataset.dimensionNames()) {
				dimensionNames << dimensionName;
			}

			imageDataset.setImageNames(dimensionNames);

			imageDataset.addLayer("default_color", "Color", Layer::Type::Image, Layer::Flags::Enabled | Layer::Flags::Enabled);
			imageDataset.addLayer("default_selection", "Selection", Layer::Type::Selection, Layer::Flags::Enabled);
			break;
		}

		default:
			break;
	}

	imageDataset.setImageFilePaths(imageFilePaths);
	imageDataset.setPointsName(imagesDataset.points()->getDataName());

	_datasetsModel.add(&imageDataset);
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
	qDebug() << "Data changed" << dataset;
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
	qDebug() << "Data removed" << dataset;
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	const auto hits = _datasetsModel.match(_datasetsModel.index(0, DatasetsModel::Columns::PointsName), Qt::DisplayRole, dataset, -1, Qt::MatchExactly);
	
	if (hits.isEmpty())
		return;

	const auto firstHit		= hits.first();
	const auto datasetType	= _datasetsModel.data(_datasetsModel.index(firstHit.row(), DatasetsModel::Columns::Type), Qt::EditRole).toInt();
	const auto datasetName	=_datasetsModel.data(_datasetsModel.index(firstHit.row(), DatasetsModel::Columns::Name), Qt::EditRole).toString();

	auto imagesDataset = _core->requestData<Images>(datasetName);

	_datasetsModel.setData(_datasetsModel.index(hits.first().row(), DatasetsModel::Columns::Selection), QVariant::fromValue(Indices::fromStdVector(imagesDataset.indices())));

	if (datasetType == ImageData::Type::Stack)
		_datasetsModel.layersModel(firstHit.row())->setDefaultSelectionImage(imagesDataset.selectionImage());
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