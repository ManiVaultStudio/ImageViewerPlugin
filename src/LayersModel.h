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
		Name,
		Type,
		Enabled,
		Fixed,
		Order,
		Opacity,
		Window,
		Level,
		Color
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

	/** TODO */
	//void sort(int column, Qt::SortOrder order /* = Qt::AscendingOrder */);

public: // Getters
	
	/** TODO */
	QVariant name(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant type(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant enabled(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant fixed(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant order(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant opacity(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant window(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant level(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	QVariant color(const int& row, int role = Qt::DisplayRole) const;

public: // Setters

	/** TODO */
	void setName(const int& row, const QString& name);

	/** TODO */
	void setType(const int& row, const int& type);

	/** TODO */
	void setEnabled(const int& row, const bool& enabled);

	/** TODO */
	void setOrder(const int& row, const int& order);

	/** TODO */
	void setOpacity(const int& row, const float& opacity);

	/** TODO */
	void setWindow(const int& row, const float& window);

	/** TODO */
	void setLevel(const int& row, const float& level);

	/** TODO */
	void setColor(const int& row, const QColor& color);

public: // TODO

	/** TODO */
	bool mayMoveUp(const int& row);

	/** TODO */
	bool mayMoveDown(const int& row);

	/** TODO */
	void moveUp(const int& row);

	/** TODO */
	void moveDown(const int& row);

private:
	Layers*		_layers;		/** TODO */

	friend class MainModel;
};