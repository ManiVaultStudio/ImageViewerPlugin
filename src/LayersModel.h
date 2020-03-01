#pragma once

#include "Layer.h"

#include <QAbstractListModel>

class QItemSelectionModel;

/** TODO */
class LayersModel : public QAbstractListModel
{
public: // Columns
	
	/** TODO */
	enum class Columns : int {
		Name,
		Type,
		Enabled,
		Order,
		Opacity,
		Window,
		Level
	};

public: // Construction/destruction

	/** Constructor */
	LayersModel(Layers* layers);

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

public: // Getters
	
	/** TODO */
	QString name(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	int type(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	bool enabled(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	int order(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	float opacity(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	float window(const int& row, int role = Qt::DisplayRole) const;

	/** TODO */
	float level(const int& row, int role = Qt::DisplayRole) const;

public: // Getters

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

public: // TODO

	/** TODO */
	QItemSelectionModel* selectionModel() { return _selectionModel; }

private:
	Layers*					_layers;			/** TODO */
	QItemSelectionModel*	_selectionModel;	/** TODO */

	friend class MainModel;
};