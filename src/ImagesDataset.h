#pragma once

#include "Dataset.h"

#include "ImageData/Images.h"

#include <QStringList>

/** TODO */
class ImagesDataset : public Dataset
{
public:
	/** TODO */
	ImagesDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name);

	/** TODO */
	void init();

public: // Getters/setters

	/** TODO */
	QVariant imageDataType(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageDataType(const ImageData::Type& imageDataType);

	/** TODO */
	QVariant noImages(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant width(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant height(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageSize(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageSize(const QSize& size);

	/** TODO */
	QVariant noPoints(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoPoints(const std::uint32_t& noPoints);

	/** TODO */
	QVariant noDimensions(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoDimensions(const std::uint32_t& noDimensions);

	/** TODO */
	QVariant imageNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageIds(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageNames(const QStringList& imageNames);

	/** TODO */
	QVariant imageFilePaths(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageFilePaths(const QStringList& imageFilePaths);

	/** TODO */
	QVariant pointsName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setPointsName(const QString& pointsName);

	/** TODO */
	QVariant selection(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSelection(const Indices& selection);

	/** TODO */
	QVariant selectionSize(const int& role = Qt::DisplayRole) const;

private:
	ImageData::Type		_imageDataType;			/** TODO */
	QSize				_size;					/** TODO */
	std::uint32_t		_noPoints;				/** TODO */
	std::uint32_t		_noDimensions;			/** TODO */
	QStringList			_imageNames;			/** TODO */
	QStringList			_imageFilePaths;		/** TODO */
	QString				_pointsName;			/** TODO */
	Indices				_selection;				/** TODO */
};