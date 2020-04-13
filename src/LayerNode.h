#pragma once

#include "RenderNode.h"
#include "ImageRange.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>
#include <QMatrix4x4>

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
		Dataset,
		Opacity,
		ColorMap,
		Image,
		Flags,

		Start = Name,
		End = Flags
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

			case Column::Dataset:
				return "Dataset";

			case Column::Opacity:
				return "Opacity";
			
				/*
			case Column::WindowNormalized:
				return "Window";

			case Column::LevelNormalized:
				return "Level";
				*/
			case Column::ColorMap:
				return "Color";

				/*
			case Column::Image:
				return "Image";

			case Column::ImageRange:
				return "Image range";

			case Column::DisplayRange:
				return "Display range";
				*/

			case Column::Flags:
				return "Flags";

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
	LayerNode(Dataset* dataset, const Type& type, const QString& id, const QString& name, const int& flags);

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
	QVariant dataset(const int& role) const;

	/** TODO */
	QVariant type(const int& role) const;

	/** TODO */
	void setType(const LayerNode::Type& type);

	/** TODO */
	QVariant colorMap(const int& role) const;

	/** TODO */
	void setColorMap(const QImage& colorMap);

signals:

	/** TODO */
	void dataChanged();

protected:
	Dataset*				_dataset;		/** TODO */
	LayerNode::Type			_type;			/** TODO */
	QImage					_colorMap;		/** TODO */
};

using Layers = QList<LayerNode*>;