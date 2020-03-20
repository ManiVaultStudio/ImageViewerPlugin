#pragma once

#include <QString>
#include <QImage>
#include <QModelIndex>

using Indices = QVector<std::uint32_t>;

Q_DECLARE_METATYPE(Indices);

class ImageViewerPlugin;

class Dataset : public QObject
{
public:

	/** TODO */
	enum class Columns {
		NoPoints,			/** TODO */
		NoDimensions,		/** TODO */
	};

	static QString columnName(const Columns& column) {
		switch (column)
		{
			case Columns::NoPoints:
				return "No. Points";

			case Columns::NoDimensions:
				return "No. Dimensions";

			default:
				break;
		}

		return "";
	}

	/** TODO */
	enum class Type {
		Images,		/** TODO */
		Points,		/** TODO */
		Clusters	/** TODO */
	};

	static QString typeName(const Type& type) {
		switch (type)
		{
			case Type::Images:
				return "Images";

			case Type::Points:
				return "Points";

			case Type::Clusters:
				return "Clusters";

			default:
				break;
		}

		return "";
	}

public:

	/** TODO */
	Dataset(ImageViewerPlugin* imageViewerPlugin, const QString& name, const Dataset::Type& type);

	/** TODO */
	virtual void init() = 0;

public: // MVC

	/** TODO */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const = 0;

	/** TODO */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const = 0;

	/** TODO */
	virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;

	/** TODO */
	virtual QVariant data(const int& row, const int& column, int role) const = 0;

	/** TODO */
	virtual void setData(const int& row, const int& column, const QVariant& value) = 0;

public: // Getters/setters

	/** TODO */
	QVariant name(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setName(const QString& name);

	/** TODO */
	QVariant type(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setType(const Dataset::Type& type);

	/** TODO */
	QVariant selectionImage(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSelectionImage(const QImage& selectionImage);

signals:
	
	/** TODO */
	void selectionImageChanged(const QImage& selectionImage);

protected:
	ImageViewerPlugin*		_imageViewerPlugin;		/** TODO */
	QString					_name;					/** TODO */
	Dataset::Type			_type;					/** TODO */
	QImage					_selectionImage;		/** TODO */
};

using Datasets = QList<Dataset*>;