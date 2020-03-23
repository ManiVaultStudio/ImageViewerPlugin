#pragma once

#include "Layer.h"

/** TODO */
class SelectionLayer : public Layer
{
public:

	/** TODO */
	SelectionLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const int& column) const override;

	/** TODO */
	QVariant data(const int& column, int role) const override;

	/** TODO */
	void setData(const int& column, const QVariant& value, const int& role) override;

private:
};