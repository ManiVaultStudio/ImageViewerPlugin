#pragma once

#include "Layer.h"
#include "Channel.h"

class QPainter;

/**
 * Group layer class
 *
 * Layer that contains child layers
 *
 * @author Thomas Kroes
 */
class GroupLayer : public Layer, public Channels<std::uint8_t>
{
public:
	
	/**  Columns */
	enum class Column {
		Start = ult(Layer::Column::End) + 1,
		End = Start
	};

public: // Construction

	/**
	 * Constructor
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	GroupLayer(const QString& id, const QString& name, const int& flags);

public: // Miscellaneous

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	void paint(QPainter* painter) override;

public: // Inherited MVC

	/** Returns the number of columns */
	[[nodiscard]] int columnCount() const override { return ult(Column::End) + 1; }

	/**
	 * Returns the item flags for the given model index
	 * @param index Model index
	 * @return Item flags for the index
	 */
	[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

	/**
	 * Returns the data for the given model index and data role
	 * @param index Model index
	 * @param role Data role
	 * @return Data in variant form
	 */
	[[nodiscard]] QVariant data(const QModelIndex& index, const int& role) const override;

	/**
	 * Sets the data value for the given model index and data role
	 * @param index Model index
	 * @param value Data value in variant form
	 * @param role Data role
	 * @return Model indices that are affected by the operation
	 */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

protected: // Miscellaneous

	/** Returns hints that pertain to the layer */
	[[nodiscard]] Hints hints() const override;
};