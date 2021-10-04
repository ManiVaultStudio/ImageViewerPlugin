#pragma once

#include "event/EventListener.h"

#include "Layer.h"

#include <QAbstractListModel>

class LayersModel : public QAbstractListModel , public hdps::EventListener
{
public:

    /**  Columns */
    enum Column {
        Visible,        /** Visibility of the layer */
        Color,          /** Color of the layer */
        Name,           /** Name of the layer */
        ImageWidth,     /** Width of the image(s) */
        ImageHeight,    /** Height of the image(s) */
        Scale,          /** Layer scale */
        Opacity,        /** Layer opacity */

        Last    = Opacity,
        Count   = Last + 1
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit LayersModel(QObject* parent);
    
    /** Destructor */
    ~LayersModel() = default;

    /**
     * Returns the number of rows in the model given the parent model index
     * @param parent Parent model index
     * @return Number of rows in the model given the parent model index
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the number of columns in the model given the parent model index
     * @param parent Parent model index
     * @return Number of columns in the model given the parent model index
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the model index for the given row, column and parent model index
     * @param row Row
     * @param column Column
     * @param parent Parent model index
     * @return Model index
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the data for the given model index and data role
     * @param index Model index
     * @param role Data role
     * @return Data in variant form
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * Sets the data value for the given model index and data role
     * @param index Model index
     * @param value Data value in variant form
     * @param role Data role
     * @return Whether the data was properly set or not
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    /**
     * Get header data
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Get item flags
     * @param index Model index
     * @return Item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public: // Layer operations

    /**
     * Add a layer to the model
     * @param layer Shared pointer to the layer
     */
    void addLayer(const SharedLayer& layer);

    /**
     * Remove a layer from the model by images dataset name
     * @param datasetName Name of the images dataset
     */
    void removeLayer(const QString& datasetName);

    /**
     * Remove a layer from the model by model index of the layer to remove
     * @param layerModelIndex Layer model index
     */
    void removeLayer(const QModelIndex& layerModelIndex);

    /**
     * Duplicates a layer  by model index
     * @param layerModelIndex Layer model index of the layer to duplicate
     */
    void duplicateLayer(const QModelIndex& layerModelIndex);

    /**
     * Move a layer in the model by amount
     * @param layerModelIndex Layer model index of the layer to move
     * @param amount Amount of layers to move up/down
     */
    void moveLayer(const QModelIndex& layerModelIndex, const std::int32_t& amount = 1);

    /** Get the layers data */
    QVector<SharedLayer>& getLayers();

protected:
    
    /**
     * Get color icon for the layer
     * @param color Input color
     * @return Icon
     */
    QIcon getColorIcon(const QColor& color) const;

protected:
    QVector<SharedLayer>    _layers;        /** Layers data */
};
