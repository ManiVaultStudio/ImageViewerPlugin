#pragma once

#include "LayerNode.h"
#include "WindowLevelImage.h"

class ImagesDataset;

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
		Selection,
		SelectionSize,

		Start = NoImages,
		End = SelectionSize
	};

	/** TODO */
	ImagesLayer(ImagesDataset* imagesDataset, const QString& id, const QString& name, const int& flags);

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
	WindowLevelImage	_image;				/** Image */
	ImagesDataset*		_images;			/** TODO */
};