#pragma once

#include "RenderNode.h"
#include "ImageRange.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>
#include <QMatrix4x4>

class ImageViewerPlugin;
class Dataset;
class Prop;

/**
 * Layer node class
 *
 * Contains props for rendering in OpenGL
 *
 * @author Thomas Kroes
 */
class LayerNode : public RenderNode
{
public:

	/** TODO */
	enum class Column {
		Name,
		Type,
		ID,
		DatasetName,
		RawDataName,
		Opacity,
		ColorMap,
		Flags,
		Selection,
		SelectionSize,

		Start = Name,
		End = SelectionSize
	};

	/** TODO */
	static QString columnName(const Column& column) {
		switch (column) {
			case Column::Name:
				return "Name";

			case Column::Type:
				return "";

			case Column::ID:
				return "ID";

			case Column::DatasetName:
				return "Dataset Name";

			case Column::RawDataName:
				return "Raw Data Name";

			case Column::Opacity:
				return "Opacity";
			
			case Column::ColorMap:
				return "Color";

			case Column::Flags:
				return "Flags";

			case Column::Selection:
				return "Selection";

			case Column::SelectionSize:
				return "Selection Size";

			default:
				return QString();
		}

		return QString();
	}

	/** TODO */
	enum class Type {
		Points,			/** TODO */
		Images,			/** TODO */
		Clusters,		/** TODO */
		Selection,		/** TODO */
		Group			/** TODO */
	};

	/** TODO */
	static QString typeName(const Type& type) {
		switch (type)
		{
			case Type::Images:
				return "Image";

			case Type::Selection:
				return "Selection";

			case Type::Clusters:
				return "Clusters";

			case Type::Points:
				return "Points";

			default:
				break;
		}

		return "";
	}

	/** Constructor */
	LayerNode(const QString& dataset, const Type& type, const QString& id, const QString& name, const int& flags);

	/** Destructor */
	virtual ~LayerNode();

public: // MVC

	/** TODO */
	virtual int noColumns() const;

	/** TODO */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/** TODO */
	virtual QVariant data(const QModelIndex& index, const int& role) const;

	/** TODO */
	virtual QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role);

public: // Getters/setters

	/** TODO */
	QVariant datasetName(const int& role) const;

	/** TODO */
	QVariant rawDataName(const int& role) const;

	/** TODO */
	QVariant type(const int& role) const;

	/** TODO */
	void setType(const LayerNode::Type& type);

	/** TODO */
	QVariant colorMap(const int& role) const;

	/** TODO */
	void setColorMap(const QImage& colorMap);

	/** TODO */
	QVariant selection(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSelection(const Indices& selection);

	/** TODO */
	QVariant selectionSize(const int& role = Qt::DisplayRole) const;

signals:

	/** TODO */
	void dataChanged();

public:
	static ImageViewerPlugin* imageViewerPlugin;

protected:
	QString				_datasetName;	/** TODO */
	QString				_rawDataName;	/** TODO */
	LayerNode::Type		_type;			/** TODO */
	QImage				_colorMap;		/** TODO */
	Indices				_selection;		/** TODO */
};

using Layers = QList<LayerNode*>;