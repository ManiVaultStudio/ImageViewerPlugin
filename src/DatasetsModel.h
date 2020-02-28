#pragma once

#include "Common.h"

#include <QAbstractListModel>

class MainModel;

/** TODO */
class DatasetsModel : public QAbstractListModel
{
	Q_OBJECT

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
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

	/** TODO */
	QModelIndex parent(const QModelIndex& index) const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/** TODO */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole);

	/** TODO */
	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());

	/** TODO */
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

	/** TODO */
	QList<ImageDataset> datasets();

private:
	MainModel*		_mainModel;				/** TODO */
	QString			_currentDatasetName;	/** TODO */
};