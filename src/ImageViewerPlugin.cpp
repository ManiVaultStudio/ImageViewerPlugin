#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"
#include "ColorImageActor.h"
#include "Renderer.h"

#include <QDebug>
#include <QFileInfo>

#include "IndexSet.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr)
{
	qRegisterMetaType<std::shared_ptr<QImage>>("std::shared_ptr<QImage>");

	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	/*
	connect(this, &ImageViewerPlugin::currentDatasetChanged, this, [&]() {
		computeDisplayImage();
		updateWindowTitle();
	}, Qt::AutoConnection);

	connect(this, &ImageViewerPlugin::currentImageIdChanged, this, [&]() {
		computeDisplayImage();
		updateWindowTitle();
	}, Qt::AutoConnection);

	connect(this, &ImageViewerPlugin::currentDimensionIdChanged, this, [&]() {
		computeDisplayImage();
		updateWindowTitle();
	}, Qt::AutoConnection);

	connect(this, &ImageViewerPlugin::averageImagesChanged, this, [&]() {
		computeDisplayImage();
		updateWindowTitle();
	}, Qt::AutoConnection);

	auto* colorImageActor = this->_imageViewerWidget->renderer()->actorByName<ColorImageActor>("ColorImageActor");

	connect(colorImageActor, &ColorImageActor::windowLevelChanged, this, [&]() {
		updateWindowTitle();
	}, Qt::AutoConnection);
	
	connect(colorImageActor, &ColorImageActor::imageMinMaxChanged, [&]() {
		updateWindowTitle();
	});

	connect(colorImageActor, &ColorImageActor::imageSizeChanged, [&]() {
		updateWindowTitle();
	});
	*/
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);

	setMainLayout(layout);

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);
}

void ImageViewerPlugin::update()
{
	/*
	if (_currentImages == nullptr)
		return;

	auto imageFileNames = QStringList();

	foreach(const QString& imageFilePath, _currentImages->imageFilePaths())
	{
		imageFileNames << QFileInfo(imageFilePath).fileName();
	}

	if (imageCollectionType() == ImageCollectionType::Sequence) {
		auto imageNames = QStringList();

		if (hasSelection()) {
			auto images = QStringList();

			for (auto& index : selection())
			{
				images << QString("%1").arg(imageFileNames[index]);
			}

			const auto imagesString = images.join(", ");

			imageNames << imagesString;
		}
		else {
			if (_averageImages) {
				auto images = QStringList();

				for (std::uint32_t i = 0; i < _currentImages->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageNames << imagesString;
			}
			else {
				for (std::uint32_t i = 0; i < _currentImages->noImages(); i++) {
					imageNames << QString("%1").arg(imageFileNames[i]);
				}
			}
		}

		setImageNames(imageNames);
		setDimensionNames(QStringList());
	}
	
	if (imageCollectionType() == ImageCollectionType::Stack) {
		setImageNames(QStringList());

		auto dimensionNames = QStringList();

		if (_averageImages) {
			dimensionNames << imageFileNames.join(", ");
		}
		else {
			dimensionNames = imageFileNames;
		}

		setDimensionNames(dimensionNames);
	}
	*/
}

void ImageViewerPlugin::computeDisplayImage()
{
	/*
	if (_currentImages == nullptr)
		return;

	switch (imageCollectionType()) {
		case ImageCollectionType::Sequence:
		{
			auto ids = std::vector<std::uint32_t>();
			
			if (hasSelection()) {
				const auto pointSelection = selection();
				ids = _averageImages ? pointSelection : std::vector<std::uint32_t>({ pointSelection.front() });
			}
			else
			{
				if (_averageImages) {
					ids.resize(_currentImages->noImages());
					std::iota(ids.begin(), ids.end(), 0);
				}
				else
				{
					ids = std::vector<std::uint32_t>({ static_cast<std::uint32_t>(_currentImageId) });
				}
			}
			
			auto image = _currentImages->sequenceImage(ids);

			emit displayImageChanged(image);
			break;
		}

		case ImageCollectionType::Stack:
		{
			auto image = _currentImages->stackImage(_currentDimensionId);

			emit displayImageChanged(image);
			break;
		}

		default:
			break;
	}
	*/
}

void ImageViewerPlugin::computeSelectionImage()
{
	/*
	if (_currentImages == nullptr)
		return;

	qDebug() << "Compute selection image";

	if (imageCollectionType() == ImageCollectionType::Stack) {
		emit selectionImageChanged(_currentImages->selectionImage(), _currentImages->selectionBounds(true));
	}
	else {
		emit selectionImageChanged(std::make_shared<QImage>(), QRect());
	}
	*/
}

ImageViewerWidget* ImageViewerPlugin::imageViewerWidget()
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

	_datasets.add(dataset, QSharedPointer<Dataset>::create(dataset, &_core->requestData<Images>(dataset)));
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
	qDebug() << "Data changed" << dataset;
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
	qDebug() << "Data removed" << dataset;
	
	// TODO: _datasetNames.removeAt(_datasetNames.indexOf(dataset));
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	/*
	qDebug() << "Selection changed" << dataset;

	update();

	if (imageCollectionType() == ImageCollectionType::Sequence)
		computeDisplayImage();
	else
		computeSelectionImage();

	updateWindowTitle();
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