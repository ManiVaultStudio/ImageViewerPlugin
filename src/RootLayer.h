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
	
	/** TODO */
	enum class Column {
		Start = ult(Layer::Column::End) + 1,
		End = Start
	};

public: // Construction

	/** Default constructor */
	RootLayer();

public: // Visualization

	/** TODO */
	void render(const QMatrix4x4& parentMVP) override;

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	void paint(QPainter* painter) override;

public: // Inherited MVC

	/** TODO */
	int columnCount() const override { return ult(Column::End) + 1; }

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
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