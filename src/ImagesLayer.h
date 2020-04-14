#pragma once

#include "LayerNode.h"
#include "WindowLevelImage.h"

#include "ImageData/Images.h"

class Images;

/** TODO */
class ImagesLayer : public LayerNode
{
public:

	/** TODO */
	enum class Column {
		NoImages = ult(LayerNode::Column::End) + 1,
		Width,
		Height,
		WindowNormalized,
		LevelNormalized,
		ImageSize,
		NoPoints,
		NoDimensions,
		ImageNames,
		FilteredImageNames,
		ImageIDs,
		ImageFilePaths,
		CurrentImageId,
		CurrentImageName,
		CurrentImageFilePath,
		Average,

		Start = NoImages,
		End = Average
	};

	/** TODO */
	ImagesLayer(const QString& dataset, const QString& id, const QString& name, const int& flags);

	/** Initializes the layer */
	void init();

public: // Inherited MVC

	/** TODO */
	int noColumns() const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

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
	QVariant filteredImageNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImageId(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setCurrentImageId(const std::uint32_t& currentImage);

	/** TODO */
	QVariant currentImageName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImageFilePath(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant average(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setAverage(const bool& average);

private:
	Images*				_images;			/** TODO */
	ImageData::Type		_imageDataType;		/** TODO */
	QSize				_size;				/** TODO */
	std::uint32_t		_noPoints;			/** TODO */
	std::uint32_t		_noDimensions;		/** TODO */
	QStringList			_imageNames;		/** TODO */
	QStringList			_imageFilePaths;	/** TODO */
	QString				_pointsName;		/** TODO */
	std::int32_t		_currentImage;		/** TODO */
	bool				_average;			/** TODO */
	WindowLevelImage	_image;				/** Image */
};