#pragma once

#include "MainModel.h"

#include <QAbstractListModel>

/** TODO */
class LayersModel : public QAbstractListModel
{
public: // Columns
	
	/** TODO */
	enum class Columns : int {
		Name,
		Order,
		Opacity,
		Window,
		Level
	};

public: // Construction/destruction

	/** Constructor */
	LayersModel(MainModel* mainModel);

	/** Destructor */
	~LayersModel();

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

public: // TODO

	/** TODO */
	const Layers layers() const;

	/** TODO */
	Layers layers();

private:
	MainModel*		_mainModel;				/** TODO */
	QString			_currentDatasetName;	/** TODO */
};