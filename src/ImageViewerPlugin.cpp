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
	_currentDatasetName(),
	_currentImageId(-1),
	_currentDimensionId(),
	_averageImages(false),
	_displayImageIds()
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	connect(this, &ImageViewerPlugin::currentDatasetNameChanged, this, &ImageViewerPlugin::updateDisplayImageIds);
	connect(this, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerPlugin::updateDisplayImageIds);
	connect(this, &ImageViewerPlugin::currentDimensionIdChanged, this, &ImageViewerPlugin::updateDisplayImageIds);
	connect(this, &ImageViewerPlugin::averageImagesChanged, this, &ImageViewerPlugin::updateDisplayImageIds);
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
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(_currentDatasetName));

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

QString ImageViewerPlugin::imageCollectionType() const
{
	if (_currentDatasetName.isEmpty())
		return "";

	PointsPlugin& points = pointsData();

	if (points.hasProperty("type")) {
		return points.getProperty("type").toString();
	}

	return "";
}

bool ImageViewerPlugin::isSequence() const
{
	return imageCollectionType() == "SEQUENCE";
}

bool ImageViewerPlugin::isStack() const
{
	return imageCollectionType() == "STACK";
}

QStringList ImageViewerPlugin::dimensionNames() const
{
	PointsPlugin& points = pointsData();

	const auto noDimensions = points.dimNames.size();

	auto dimensionNames = QStringList();

	dimensionNames.reserve(noDimensions);

	for (size_t i = 0, l = noDimensions; i < l; ++i)
		dimensionNames << points.dimNames[i];

	return dimensionNames;
}

QStringList ImageViewerPlugin::imageFileNames() const
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

	if (isSequence() || isStack()) {
		if (points.hasProperty("imageSizes")) {
			return points.getProperty("imageSizes").toMap()["0"].toSize();
		}
	}

	return QSize();
}

QString ImageViewerPlugin::currentDatasetName() const
{
	return _currentDatasetName;
}

void ImageViewerPlugin::setCurrentDatasetName(const QString& currentDatasetName)
{
	qDebug() << "Set current data set name: " << currentDatasetName;

	_currentDatasetName = currentDatasetName;

	emit currentDatasetNameChanged(_currentDatasetName);
}

int ImageViewerPlugin::currentImageId() const
{
	return _currentImageId;
}

void ImageViewerPlugin::setCurrentImageId(const int& currentImageId)
{
	if (currentImageId < 0)
		return;

	qDebug() << "Set current image index to:" << currentImageId;

	_currentImageId = currentImageId;

	emit currentImageIdChanged(_currentImageId);
}

int ImageViewerPlugin::currentDimensionId() const
{
	return _currentDimensionId;
}

void ImageViewerPlugin::setCurrentDimensionId(const int& currentDimensionId)
{
	if (currentDimensionId < 0)
		return;

	qDebug() << "Set current dimension to:" << currentDimensionId;

	_currentDimensionId = currentDimensionId;

	emit currentDimensionIdChanged(_currentDimensionId);
}

bool ImageViewerPlugin::averageImages() const
{
	return _averageImages;
}

void ImageViewerPlugin::setAverageImages(const bool& averageImages)
{
	qDebug() << "Set average images: " << averageImages;

	_averageImages = averageImages;

	emit averageImagesChanged(_averageImages);
}

Indices ImageViewerPlugin::displayImageIds() const
{
	return _displayImageIds;
}

long ImageViewerPlugin::noPixels() const
{
	return imageSize().width() * imageSize().height();
}

void ImageViewerPlugin::updateDisplayImageIds()
{
	// qDebug() << "Update display image IDs";

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
		if (_currentImageId >= 0) {
			_displayImageIds = Indices({ static_cast<unsigned int>(_currentImageId) });
		}
		else {
			_displayImageIds = Indices();
		}
		
	}

	emit displayImageIdsChanged();
}

void ImageViewerPlugin::dataAdded(const QString name)
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type");
		
		if (type == "SEQUENCE" || type == "STACK" || type == "MULTIPART") {
			_settingsWidget->addDataSet(name);
		}
	}
}

void ImageViewerPlugin::dataChanged(const QString name)
{
}

void ImageViewerPlugin::dataRemoved(const QString name)
{
	_settingsWidget->removeDataSet(name);
}

void ImageViewerPlugin::selectionChanged(const QString dataName)
{
	if (imageCollectionType() == "SEQUENCE") {
		setAverageImages(hasSelection());
	}

	emit selectedPointsChanged();
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