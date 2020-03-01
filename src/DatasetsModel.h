#pragma once

#include "MainModel.h"

#include <QAbstractListModel>

/** TODO */
class DatasetsModel : public QAbstractListModel
{
	Q_OBJECT

public: // Columns

	/** TODO */
	enum class Columns : int {
		Name,
		Type,
		NoImages,
		Size,
		NoPoints,
		NoDimensions,
		CurrentImage,
		CurrentDimension,
		ImageNames,
		DimensionNames,
		AverageImages,
		ImageFilePaths,
		CurrentImageFilepath,
		CurrentDimensionFilepath
	};

public: // Construction/destruction

	/** Constructor */
	DatasetsModel(MainModel* mainModel);

	/** Destructor */
	~DatasetsModel();

public: // Inherited members

	/** TODO */
	int rowCount(const QModelIndex& parent) const;

	/** TODO */
	int columnCount(const QModelIndex& parent) const;

	/** TODO */
	QVariant data(const QModelIndex& index, int role) const;

	/** TODO */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/** TODO */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole);

	/** TODO */
	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());

	/** TODO */
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

	/** TODO */
	const Datasets* datasets() const;

	/** TODO */
	Datasets* datasets();

public: // Getters

	/** TODO */
	QVariant type(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentDimension(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImage(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageNames(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant dimensionNames(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant averageImages(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImageFilepath(const std::uint32_t& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentDimensionFilepath(const std::uint32_t& row, int role = Qt::DisplayRole) const;

public: // Setters

	/** TODO */
	void setCurrentImage(const std::uint32_t& row, const std::uint32_t& currentImageID);

	/** TODO */
	void setCurrentDimension(const std::uint32_t& row, const std::uint32_t& currentDimensionID);

	/** TODO */
	void setAverageImages(const std::uint32_t& row, const bool& averageImages);

protected: // TODO
	
	/** TODO */
	void add(const MainModel::Dataset& dataset);

private:
	MainModel*		_mainModel;				/** TODO */
	QString			_currentDatasetName;	/** TODO */

	friend class MainModel;
};