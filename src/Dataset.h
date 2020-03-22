#pragma once

#include <QList>
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

//using Datasets = QList<Dataset*>;