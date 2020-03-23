#pragma once

#include "Settings.h"
#include "ImagesDataset.h"

/** TODO */
class ImagesSettings : public Settings<ImagesDataset>
{
public:

	/** TODO */
	ImagesSettings(QObject* parent, Dataset* dataset);

public: // MVC

	/** TODO */
	Qt::ItemFlags itemFlags(const Layer::Column& column) const override;

	/** TODO */
	QVariant data(const Layer::Column& column, int role) const override;

	/** TODO */
	void setData(const Layer::Column& column, const QVariant& value, const int& role) override;

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