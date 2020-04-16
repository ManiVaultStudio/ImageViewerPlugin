#pragma once

#include "LayerNode.h"

/** TODO */
class RootLayer : public LayerNode
{
public:
	
	/** TODO */
	enum class Column {
		Start = ult(LayerNode::Column::End) + 1,
		End = Start
	};

public:

	/** TODO */
	RootLayer();

	/** TODO */
	void render(const QMatrix4x4& parentMVP);

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