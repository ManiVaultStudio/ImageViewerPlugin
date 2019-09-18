#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QImageReader>
#include <QInputDialog>

#include <vector>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr),
	_datasetNames(),
	_currentDataset(),
	_imageNames(),
	_currentImage(-1),
	_dimensionNames(),
	_currentDimension(),
	_averageImages(false),
	_displayImageIds()
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	//connect(this, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerPlugin::update);
	//connect(this, &ImageViewerPlugin::currentImageChanged, this, &ImageViewerPlugin::update);
	//connect(this, &ImageViewerPlugin::currentDimensionChanged, this, &ImageViewerPlugin::update);
	//connect(this, &ImageViewerPlugin::averageImagesChanged, this, &ImageViewerPlugin::update);
}

ImageViewerPlugin::~ImageViewerPlugin()
{
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);

	setLayout(layout);
}

PointsPlugin& ImageViewerPlugin::pointsData() const
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(_currentDataset));

	return set.getData();
}

Indices ImageViewerPlugin::selection() const
{
	const IndexSet& selection = dynamic_cast<const IndexSet&>(pointsData().getSelection());

	return selection.indices;
}

void ImageViewerPlugin::setSelection(Indices& indices)
{
	IndexSet& selection = dynamic_cast<IndexSet&>(pointsData().getSelection());

	selection.indices = indices;

	_core->notifySelectionChanged(selection.getDataName());
}

bool ImageViewerPlugin::hasSelection() const
{
	return selection().size() > 0;
}

ImageCollectionType ImageViewerPlugin::imageCollectionType() const
{
	if (_currentDataset.isEmpty())
		return ImageCollectionType::Undefined;

	PointsPlugin& points = pointsData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type").toString();

		if (type == "STACK")
			return ImageCollectionType::Stack;

		if (type == "SEQUENCE")
			return ImageCollectionType::Sequence;

		if (type == "MULTIPART")
			return ImageCollectionType::MultiPartSequence;
	}

	return ImageCollectionType::Undefined;
}

QStringList ImageViewerPlugin::dimensionNames() const
{
	PointsPlugin& points = pointsData();

	const auto noDimensions = points.dimNames.size();

	auto dimensionNames = QStringList();

	dimensionNames.reserve(noDimensions);

	for (size_t i = 0; i < noDimensions; ++i)
		dimensionNames << points.dimNames[i];

	return dimensionNames;
}

auto ImageViewerPlugin::imageFileNames() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageFileNames")) {
		return points.getProperty("imageFileNames").toStringList();
	}

	return QStringList();
}

int ImageViewerPlugin::noImages() const
{
	return imageFileNames().size();
}

QSize ImageViewerPlugin::imageSize() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageSizes")) {
		if (imageCollectionType() == ImageCollectionType::Stack || imageCollectionType() == ImageCollectionType::Sequence) {
			return points.getProperty("imageSizes").toMap()["0"].toSize();
		}

		/*
		if (imageCollectionType() == "MULTIPART") {
			return points.getProperty("imageSizes").toMap()["0"].toSize();
		}
		*/
	}

	return QSize();
}

Indices ImageViewerPlugin::displayImageIds() const
{
	return _displayImageIds;
}

long ImageViewerPlugin::noPixels() const
{
	return imageSize().width() * imageSize().height();
}

void ImageViewerPlugin::update()
{
	qDebug() << "update";

	if (imageCollectionType() == ImageCollectionType::Sequence) {
		auto imageNames = QStringList();

		if (hasSelection()) {
			auto images = QStringList();

			for (unsigned int index : selection())
			{
				images << QString("%1").arg(imageFileNames()[index]);
			}

			const auto imagesString = images.join(", ");

			imageNames << imagesString;
		}
		else {
			if (_averageImages) {
				auto images = QStringList();

				for (int i = 0; i < noImages(); i++) {
					images << QString("%1").arg(imageFileNames()[i]);
				}

				const auto imagesString = images.join(", ");

				imageNames << imagesString;
			}
			else {
				for (int i = 0; i < noImages(); i++) {
					imageNames << QString("%1").arg(imageFileNames()[i]);
				}
			}
		}

		setImageNames(NameSet::fromList(imageNames));
		setDimensionNames(NameSet());
		setAverageImages(hasSelection());
	}

	if (imageCollectionType() == ImageCollectionType::Stack) {
		setImageNames(NameSet());
		setDimensionNames(NameSet::fromList(dimensionNames()));
		//setAverageImages(_dimensionNames.size() > 0);
	}

	if (imageCollectionType() == ImageCollectionType::MultiPartSequence) {
		setDimensionNames(NameSet::fromList(dimensionNames()));
	}

	/*
	const auto type = imageCollectionType();

	if (_averageImages) {
		if (type == "SEQUENCE") {
			if (hasSelection()) {
				_displayImageIds = selection();
			} else {
				_displayImageIds.resize(noImages());
				std::iota(std::begin(_displayImageIds), std::end(_displayImageIds), 0);
			}
		}
		
		if (type == "STACK") {
			_displayImageIds.resize(noImages());
			std::iota(std::begin(_displayImageIds), std::end(_displayImageIds), 0);
		}
	} else {
		if (_currentImage >= 0) {
			_displayImageIds = Indices({ static_cast<unsigned int>(_currentImage) });
		}
		else {
			_displayImageIds = Indices();
		}
		
	}
	*/

	emit displayImagesChanged();
}

QString ImageViewerPlugin::currentDataset() const
{
	return _currentDataset;
}

void ImageViewerPlugin::setCurrentDataset(const QString& currentDataset)
{
	_currentDataset = currentDataset;

	emit currentDatasetChanged(_currentDataset);

	update();
}

auto ImageViewerPlugin::currentImage() const
{
	return _currentImage;
}

void ImageViewerPlugin::setCurrentImage(const int& currentImage)
{
	if (currentImage < 0)
		return;

	_currentImage = currentImage;

	emit currentImageChanged(_currentImage);
}

auto ImageViewerPlugin::currentDimension() const
{
	return _currentDimension;
}

void ImageViewerPlugin::setCurrentDimension(const int& currentDimension)
{
	if (currentDimension < 0)
		return;

	_currentDimension = currentDimension;

	emit currentDimensionChanged(_currentDimension);
}

auto ImageViewerPlugin::averageImages() const
{
	return _averageImages;
}

void ImageViewerPlugin::setAverageImages(const bool& averageImages)
{
	_averageImages = averageImages;

	emit averageImagesChanged(_averageImages);
}

void ImageViewerPlugin::setDatasetNames(const NameSet& datasetNames)
{
	_datasetNames = datasetNames;

	emit datasetNamesChanged(_datasetNames);
}

void ImageViewerPlugin::setImageNames(const NameSet& imageNames)
{
	_imageNames = imageNames;

	emit imageNamesChanged(_imageNames);
}

void ImageViewerPlugin::setDimensionNames(const NameSet& dimensionNames)
{
	_dimensionNames = dimensionNames;

	emit dimensionNamesChanged(_dimensionNames);
}

void ImageViewerPlugin::dataAdded(const QString name)
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type");
		
		if (type == "SEQUENCE" || type == "STACK" || type == "MULTIPART") {
			setDatasetNames(_datasetNames << name);
			setCurrentDataset(name);
		}
	}
}

void ImageViewerPlugin::dataChanged(const QString name)
{
}

void ImageViewerPlugin::dataRemoved(const QString name)
{
	_datasetNames.remove(name);

	emit datasetNamesChanged(_datasetNames);
}

void ImageViewerPlugin::selectionChanged(const QString dataName)
{
	update();
}

void ImageViewerPlugin::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent->key();

	switch (keyEvent->key())
	{
		case Qt::Key::Key_R:
		{
			_imageViewerWidget->setSelectionType(ImageViewerWidget::SelectionType::Rectangle);
			break;
		}

		case Qt::Key::Key_B:
		{
			_imageViewerWidget->setSelectionType(ImageViewerWidget::SelectionType::Brush);
			break;
		}

		case Qt::Key::Key_F:
		{
			_imageViewerWidget->setSelectionType(ImageViewerWidget::SelectionType::Freehand);
			break;
		}

		case Qt::Key::Key_Shift:
		{
			//if (_imageViewerWidget->selectionModifier() != ImageViewerWidget::SelectionModifier::Remove)
			_imageViewerWidget->setSelectionModifier(ImageViewerWidget::SelectionModifier::Add);
			break;
		}

		case Qt::Key::Key_Control:
		{
			//if (_imageViewerWidget->selectionModifier() != ImageViewerWidget::SelectionModifier::Add)
			_imageViewerWidget->setSelectionModifier(ImageViewerWidget::SelectionModifier::Remove);
			break;
		}

		case Qt::Key::Key_Alt:
		{
			_imageViewerWidget->setInteractionMode(ImageViewerWidget::InteractionMode::Navigation);
			break;
		}

		default:
			break;
	}

	QWidget::keyPressEvent(keyEvent);
}

void ImageViewerPlugin::keyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event" << keyEvent->key();

	switch (keyEvent->key())
	{
		case Qt::Key::Key_Shift:
		case Qt::Key::Key_Control:
		{
			if (_imageViewerWidget->selectionType() != ImageViewerWidget::SelectionType::Brush) {
				_imageViewerWidget->setSelectionModifier(ImageViewerWidget::SelectionModifier::Replace);
			}
			
			break;
		}

		case Qt::Key::Key_Alt:
		{
			_imageViewerWidget->setInteractionMode(ImageViewerWidget::InteractionMode::Selection);
			break;

		}
		default:
			break;
	}

	QWidget::keyReleaseEvent(keyEvent);
}

QStringList ImageViewerPlugin::supportedDataKinds()
{
	QStringList supportedKinds;

	supportedKinds << "Points";
	
	return supportedKinds;
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin();
}