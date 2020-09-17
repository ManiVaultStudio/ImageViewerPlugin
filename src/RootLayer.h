#pragma once

#include "Layer.h"
#include "Channel.h"

class QPainter;

/**
 * Root layer class
 *
 * All layer hierarchies consist of a root layer
 *
 * @author Thomas Kroes
 */
class RootLayer : public Layer, public Channels<std::uint8_t>
{
public:
	
	/** Columns */
	enum class Column {
		Start = ult(Layer::Column::End) + 1,
		End = Start
	};

public: // Construction

	/** Default constructor */
	RootLayer();

public: // Visualization

	/** Renders the layer */
	void render(const QMatrix4x4& parentMVP) override;

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	void paint(QPainter* painter) override;

public: // Inherited MVC

	/** Returns the number of columns */
	int getColumnCount() const override { return ult(Column::End) + 1; }

	/**
	 * Returns the item flags for the given model index
	 * @param index Model index
	 * @return Item flags for the index
	 */
	Qt::ItemFlags getFlags(const QModelIndex& index) const override;

	/**
	 * Returns the data for the given model index and data role
	 * @param index Model index
	 * @param role Data role
	 * @return Data in variant form
	 */
	QVariant getData(const QModelIndex& index, const int& role) const override;

	/**
	 * Sets the data value for the given model index and data role
	 * @param index Model index
	 * @param value Data value in variant form
	 * @param role Data role
	 * @return Model indices that are affected by the operation
	 */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

protected: // Miscellaneous

	/**
	* Returns the image size
	* @return Image size in variant form
	*/
	QSize getImageSize() const override;

	/** Returns hints that pertain to the layer */
	Hints getHints() const override;
};