#pragma once

#include "Node.h"
#include "Range.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>
#include <QVector>

class QPaintEvent;

class ImageViewerPlugin;
class Dataset;
class Prop;

/**
 * Layer node class
 *
 * TODO
 *
 * @author Thomas Kroes
 */
class Layer : public Node
{
	Q_OBJECT

public:

	/**  Columns */
	enum class Column {
		Name,					// Name of the layer
		Type,					// Type of layer
		DatasetName,			// Name of the dataset (if any)
		DataName,				// Name of the data (if any)
		ID,						// Layer identifier (for internal use)
		ImageSize,				// Size of the image(s)
		ImageWidth,				// Width of the image(s)
		ImageHeight,			// Height of the image(s)
		Opacity,				// Layer opacity
		Scale,					// Layer scale
		Flags,					// Configuration bit flags
		Selection,				// Selection
		SelectionSize,			// Size of the selection

		Start = Name,
		End = SelectionSize
	};

	/** Get string representation of layer column enumeration */
	static QString columnName(const Column& column) {
		switch (column) {
			case Column::Name:
				return "Name";

			case Column::Type:
				return "";

			case Column::DatasetName:
				return "Dataset Name";

			case Column::DataName:
				return "Data Name";

			case Column::ID:
				return "ID";

			case Column::ImageSize:
				return "Image Size";

			case Column::ImageWidth:
				return "Width";

			case Column::ImageHeight:
				return "Height";

			case Column::Opacity:
				return "Opacity";

			case Column::Scale:
				return "Scale";

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

	/** Layer types */
	enum class Type {
		Points,			/** Points dataset */
		Images,			/** Images dataset */
		Clusters,		/** Clusters dataset */
		Selection,		/** Selection layer */
		Group			/** Group layer */
	};

	/** Get string representation of layer type enumeration */
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

	/**
	 * Constructor
	 * @param datasetName Name of the dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	Layer(const QString& datasetName, const Type& type, const QString& id, const QString& name, const int& flags);

	/** Destructor */
	virtual ~Layer();

	/**
	 * Adjust the layer scaling to fit into the supplied image size
	 * @param imageSize Size of the image to scale into
	 */
	void matchScaling(const QSize& targetImageSize);

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	virtual void paint(QPainter* painter) = 0;

public: // MVC
	
	/** Returns the number of columns */
	virtual int columnCount() const { return ult(Column::End) + 1; }

	/**
	 * Returns the item flags for the given model index
	 * @param index Model index
	 * @return Item flags for the index
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/**
	 * Returns the data for the given model index and data role
	 * @param index Model index
	 * @param role Data role
	 * @return Data in variant form
	 */
	virtual QVariant data(const QModelIndex& index, const int& role) const;

	/**
	 * Sets the data value for the given model index and data role
	 * @param index Model index
	 * @param value Data value in variant form
	 * @param role Data role
	 * @return Model indices that are affected by the operation
	 */
	virtual QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role);

public: // Mouse (wheel) and keyboard events

	/**
	 * Invoked when a mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	virtual void mousePressEvent(QMouseEvent* mouseEvent, const QModelIndex& index) {};

	/**
		* Invoked when a mouse button is released
		* @param mouseEvent Mouse event
		*/
	virtual void mouseReleaseEvent(QMouseEvent* mouseEvent, const QModelIndex& index) {};

	/**
		* Invoked when the mouse pointer is moved
		* @param mouseEvent Mouse event
		*/
	virtual void mouseMoveEvent(QMouseEvent* mouseEvent, const QModelIndex& index) {};

	/**
		* Invoked when the mouse wheel is rotated
		* @param wheelEvent Mouse wheel event
		* @param index Model index of the layer
		*/
	virtual void mouseWheelEvent(QWheelEvent* wheelEvent, const QModelIndex& index) {};

	/**
		* Invoked when a key is pressed
		* @param keyEvent Key event
		* @param index Model index of the layer
		*/
	virtual void keyPressEvent(QKeyEvent* keyEvent, const QModelIndex& index) {};

	/**
		* Invoked when a key is released
		* @param keyEvent Key event
		* @param index Model index of the layer
		*/
	virtual void keyReleaseEvent(QKeyEvent* keyEvent, const QModelIndex& index) {};

public: // Getters/setters

	/**
	 * Returns the dataset name
	 * @param role The data role
	 * @return Dataset name in variant form
	 */
	QVariant datasetName(const int& role) const;

	/**
	 * Returns the data name
	 * @param role The data role
	 * @return Data name in variant form
	 */
	QVariant dataName(const int& role) const;

	/**
	 * Returns the layer type
	 * @param role The data role
	 * @return Layer type in variant form
	 */
	QVariant type(const int& role) const;

	/**
	 * Sets the layer type
	 * @param type Layer type
	 */
	void setType(const Layer::Type& type);

	/**
	 * Returns the image size
	 * @param role Data role
	 * @return Image size in variant form
	 */
	QVariant imageSize(const int& role) const;

	/**
	 * Returns the width of the images in the dataset
	 * @param role The data role
	 * @return Image width in variant form
	 */
	QVariant imageWidth(const int& role) const;

	/**
	 * Returns the height of the images in the dataset
	 * @param role The data role
	 * @return Image height in variant form
	 */
	QVariant imageHeight(const int& role) const;

	/**
	 * Returns the data point selection
	 * @param role The data role
	 * @return Data point selection in variant form
	 */
	QVariant selection(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the data point selection
	 * @param selection Data point selection
	 */
	void setSelection(const Indices& selection);

	/**
	 * Returns the size of the data point selection
	 * @param role The data role
	 * @return Data point selection size in variant form
	 */
	QVariant selectionSize(const int& role = Qt::DisplayRole) const;

	/**
	 * Returns the pressed keys
	 * @param role The data role
	 * @return Keys in variant form
	 */
	QVariant keys(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the keys
	 * @param keys Keys
	 */
	void setKeys(const int& keys);

protected:
	
	/**
	 * Returns the image size
	 * @return Image size
	 */
	virtual QSize imageSize() const = 0;

	/**
	 * Returns the number of pixels in the image
	 * @return Number of pixels
	 */
	int noPixels() const;

public:

	/** Pointer to the image viewer plugin for interfacing with datasets */
	static ImageViewerPlugin* imageViewerPlugin;

signals:
	
	/**
	 * Signals that a channel has changed
	 * @param channelId Identifier of the channel
	 */
	void channelChanged(const std::uint32_t& channelId);

protected:
	QString				_datasetName;		/** Name of the dataset to which the layer refers */
	QString				_dataName;			/** Name of the raw data to which the layer refers */
	Layer::Type			_type;				/** Type of layer */
	Indices				_selection;			/** Data point selection */
	QVector<QPoint>		_mousePositions;	/** Recorded mouse positions */
	int					_mouseButtons;		/** State of the left, middle and right mouse buttons */
	int					_keys;				/** Pressed keyboard buttons */
};