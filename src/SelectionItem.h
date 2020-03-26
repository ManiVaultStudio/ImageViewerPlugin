#pragma once

#include "Item.h"

class LayerItem;
class Dataset;

/** TODO */
class SelectionItem : public Item
{
public:

	/** TODO */
	enum class Column {
	};

public:

	/** TODO */
	SelectionItem(LayerItem* layeritem, Dataset* dataset);

public: // Inherited

	/** TODO */
	int columnCount() const override;

	/** TODO */
	QVariant headerData(const int& section, const Qt::Orientation& orientation, const int& role) const override;

	/** TODO */
	Qt::ItemFlags flags(const int& column) const override;

	/** TODO */
	QVariant data(const int& column, const int& role) const override;

	/** TODO */
	void setData(const int& column, const QVariant& value, const int& role) override;

private:
	Dataset*	_dataset;	/** TODO */
};