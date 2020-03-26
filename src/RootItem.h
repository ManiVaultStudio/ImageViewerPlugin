#pragma once

#include "Item.h"
#include "LayerItem.h"

class LayerItem;

/** TODO */
class RootItem : public Item
{
public:

	/** TODO */
	enum class Column {
	};

public:

	/** TODO */
	RootItem();

	/** TODO */
	void addLayer(Dataset* dataset, const LayerItem::Type& type, const QString& id, const QString& name, const std::uint32_t& flags);

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
};