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
	_currentDataSetName(""),
	_averageImages(false),
	_currentImageId(-1),
	_displayImageIds()
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);
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
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(_currentDataSetName));

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
	if (_currentDataSetName.isEmpty())
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

	QStringList dimensionNames;
	
	dimensionNames.reserve(points.getDimensionNames().size());
	std::copy(points.getDimensionNames().begin(), points.getDimensionNames().end(), std::back_inserter(dimensionNames));

	return dimensionNames;
}

int ImageViewerPlugin::noImages() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("noImages")) {
		return points.getProperty("noImages").toInt();
	}

	return 0;
}

QSize ImageViewerPlugin::imageSize() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageSize")) {
		return points.getProperty("imageSize").toSize();
	}

	return QSize();
}

QString ImageViewerPlugin::currentDataSetName() const
{
	return _currentDataSetName;
}

void ImageViewerPlugin::setCurrentDataSetName(const QString& currentDataSetName)
{
	qDebug() << "Set current data set name: " << currentDataSetName;

	_currentDataSetName = currentDataSetName;

	emit currentDataSetNameChanged(_currentDataSetName);
}

bool ImageViewerPlugin::averageImages() const
{
	return _averageImages;
}

void ImageViewerPlugin::setAverageImages(const bool& averageImages)
{
	qDebug() << "Set average images: " << averageImages;

	_averageImages = averageImages;

	updateDisplayImageIds();

	emit averageImagesChanged(_averageImages);
}

Index ImageViewerPlugin::currentImageId() const
{
	return _currentImageId;
}

void ImageViewerPlugin::setCurrentImageId(const int& currentImageId)
{
	if (currentImageId < 0)
		return;

	qDebug() << "Set current image index to:" << currentImageId;

	_currentImageId = currentImageId;

	updateDisplayImageIds();
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

	/*
	auto imageIds = QStringList();

	for (auto displayImageId : _displayImageIds) {
		imageIds << QString::number(displayImageId);
	}
	
	qDebug() << imageIds.size();
	*/
}

void ImageViewerPlugin::dataAdded(const QString name)
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type");
		
		if (type == "SEQUENCE" || type == "STACK") {
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