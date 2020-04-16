#pragma once

#include "LayerNode.h"

class ClustersDataset;

/** TODO */
class GroupLayer : public LayerNode
{
public:
	
	/** TODO */
	enum class Column {
		ParamA = ult(LayerNode::Column::End) + 1,
		ParamB,

		Start = ParamA,
		End = ParamB
	};

public:

	/** TODO */
	GroupLayer(const QString& id, const QString& name, const int& flags);

public: // Inherited MVC

	/** TODO */
	int columnCount() const override { return ult(Column::End) + 1; }

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;
};