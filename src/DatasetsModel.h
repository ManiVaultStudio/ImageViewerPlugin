#pragma once

#include "ImageDataset.h"

#include <QAbstractListModel>

class QItemSelectionModel;

/** TODO */
class DatasetsModel : public QAbstractListModel
{
public: // Columns

	/** TODO */
	enum Columns : int {
		Name,
		Type,
		NoImages,
		Size,
		NoPoints,
		NoDimensions,
		ImageNames,
		ImageFilePaths,
		CurrentImage,
		CurrentImageName,
		CurrentImageFilepath,
		Average,
		PointsName,
		Selection,
		SelectionSize
	};

public: // Construction/destruction

	/** Constructor */
	DatasetsModel(QObject* parent);

	/** Destructor */
	~DatasetsModel();

public: // Inherited members

	/** TODO */
	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/** TODO */
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

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
	const Datasets& datasets() const;

	/** TODO */
	Datasets& datasets();

public: // Overloaded data access

	/** TODO */
	QVariant data(const int& row, const int& column, int role = Qt::DisplayRole) const;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value);

public: // 
	
	/** TODO */
	void add(ImageDataset* dataset);

	/** TODO */
	LayersModel* layersModel(const int& row);

	/** TODO */
	QItemSelectionModel* selectionModel() { return _selectionModel; }

private:
	Datasets				_datasets;				/** TODO */
	QItemSelectionModel*	_selectionModel;		/** TODO */

	friend class MainModel;
};