#pragma once

#include "Layer.h"

#include <QAbstractListModel>

class QItemSelectionModel;

/** TODO */
class LayersModel : public QAbstractListModel
{
public: // Columns
	
	/** TODO */
	enum Columns : int {
		Enabled,
		Type,
		Locked,
		Name,
		Fixed,
		Removable,
		Mask,
		Renamable,
		Order,
		Opacity,
		WindowNormalized,
		LevelNormalized,
		Color,
		Image,
		ImageRange,
		DisplayRange
	};

public: // Construction/destruction

	/** Constructor */
	LayersModel(Layers* layers);

	/** Destructor */
	~LayersModel();

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

public: // Overloaded data access
	
	/** TODO */
	QVariant data(const int& row, const int& column, int role = Qt::DisplayRole) const;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value);

public: // TODO

	/** TODO */
	bool mayMoveUp(const int& row);

	/** TODO */
	bool mayMoveDown(const int& row);

	/** TODO */
	void moveUp(const int& row);

	/** TODO */
	void moveDown(const int& row);

	/** TODO */
	void removeRows(const QModelIndexList& rows);

private:
	Layers*		_layers;		/** TODO */

	friend class MainModel;
};