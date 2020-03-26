#pragma once

#include "Item.h"

class LayerItem;
class ImagesDataset;

/** TODO */
class ImagesItem : public Item
{
public:

	/** TODO */
	enum class Column {
		NoImages,
		Width,
		Height,
		Size,
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
		Selection,

		Start = NoImages,
		End = Selection
	};

	/** TODO */
	ImagesItem(LayerItem* layeritem, ImagesDataset* imagesDataset);

public: // Inherited

	/** TODO */
	int columnCount() const override;

	/** TODO */
	QVariant headerData(const int& section, const Qt::Orientation& orientation, const int& role) const override;

	/** TODO */
	Qt::ItemFlags flags(const int& column) const override;

	/** TODO */
	QVariant data(const int& column, const int& role) const override;

	/** TODO */
	void setData(const int& column, const QVariant& value, const int& role) override;

public: // Getters/setters

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
	std::int32_t		_currentImage;		/** TODO */
	bool				_average;			/** TODO */
	ImagesDataset*		_dataset;			/** TODO */
};