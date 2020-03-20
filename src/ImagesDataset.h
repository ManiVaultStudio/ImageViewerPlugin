#pragma once

#include "Dataset.h"

#include <QStringList>

/** TODO */
class ImagesDataset : public Dataset
{
public:
	/** TODO */
	enum class Column {
		NoImages,			/** TODO */
		Width,				/** TODO */
		Height,				/** TODO */
		Size				/** TODO */
	};

	static QString columnName(const Column& column) {
		switch (column)
		{
			case Column::NoImages:
				return "No. Images";

			case Column::Width:
				return "Width";

			case Column::Height:
				return "Height";

			case Column::Size:
				return "Size";

			default:
				break;
		}

		return "";
	}

public:
	/** TODO */
	ImagesDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name);

	/** TODO */
	void init();

public: // Inherited model members

	/** TODO */
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/** TODO */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	/** TODO */
	QVariant data(const int& row, const int& column, int role) const override;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value) override;

public: // Getters/setters

	/** TODO */
	QVariant noImages(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant width(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant height(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant size(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSize(const QSize& size);

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
	QVariant filteredImageNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageIds(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageNames(const QStringList& imageNames);

	/** TODO */
	QVariant imageFilePaths(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageFilePaths(const QStringList& imageFilePaths);

	/** TODO */
	QVariant currentImage(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setCurrentImage(const std::uint32_t& currentImage);

	/** TODO */
	QVariant currentImageName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImageFilePath(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant average(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setAverage(const bool& average);

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

	static QString displayStringList(const QStringList& stringList);

private:
	QSize				_size;					/** TODO */
	std::uint32_t		_noPoints;				/** TODO */
	std::uint32_t		_noDimensions;			/** TODO */
	QStringList			_imageNames;			/** TODO */
	QStringList			_imageFilePaths;		/** TODO */
	std::int32_t		_currentImage;			/** TODO */
	bool				_average;				/** TODO */
	QString				_pointsName;			/** TODO */
	Indices				_selection;				/** TODO */
};