#pragma once

#include "Layer.h"

/** TODO */
class SelectionLayer : public LayerItem
{
public:

	/** TODO */
	SelectionLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, int role) const override;

	/** TODO */
	void setData(const QModelIndex& index, const QVariant& value, const int& role) override;

private:
};