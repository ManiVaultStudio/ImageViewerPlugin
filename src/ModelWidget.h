#pragma once

#include <QWidget>
#include <QPersistentModelIndex>

class LayersModel;

/** TODO */
class ModelWidget : public QWidget
{
public:
	/** TODO */
	ModelWidget(QWidget* parent, const int& noColumns);

	/** TODO */
	virtual void initialize(LayersModel* layersModel);

protected:

	/** TODO */
	void setIndex(const QModelIndex& index);

	/** TODO */
	virtual void updateData(const QModelIndex& index) = 0;

	/** TODO */
	virtual bool shouldUpdate(const QModelIndex& index) const = 0;

private:

	/** TODO */
	void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

protected:
	LayersModel*	_layersModel;		/** TODO */
	QModelIndex		_index;				/** TODO */
	int				_noColumns;			/** TODO */
};