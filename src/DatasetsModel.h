#pragma once

#include "Dataset.h"

#include <QAbstractListModel>

class ImageViewerPlugin;

/** TODO */
class DatasetsModel : public QAbstractListModel
{
public: // Columns

	/** TODO */
	enum Columns : int {
		Name,
		Type,
		SelectionImage
	};

public: // Construction/destruction

	/** Constructor */
	DatasetsModel(ImageViewerPlugin* imageViewerPlugin);

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
	QVariant data(const int& row, const int& column, int role) const;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value);

public: // 
	
	/** TODO */
	Dataset* findDataset(const QString& name);

	/** TODO */
	Dataset* addDataset(const QString& name, const Dataset::Type& type);

private:
	ImageViewerPlugin*		_imageViewerPlugin;		/** TODO */
	Datasets				_datasets;				/** TODO */

	friend class MainModel;
};