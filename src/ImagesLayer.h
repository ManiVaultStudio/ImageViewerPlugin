#pragma once

#include "Layer.h"
#include "ImagesDataset.h"

/** TODO */
class ImagesLayer : public Layer
{
public:

	/** TODO */
	enum class Column {
		NoImages = static_cast<int>(Layer::Column::Count),
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

		Count
	};

	/** TODO */
	ImagesLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags);

	const ImagesDataset* imagesDataset() const { return reinterpret_cast<ImagesDataset*>(_dataset); }

public: // MVC

	/** TODO */
	Qt::ItemFlags itemFlags(const int& column) const override;

	/** TODO */
	QVariant data(const int& column, int role) const override;

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
	std::int32_t		_currentImage;			/** TODO */
	bool				_average;				/** TODO */
};