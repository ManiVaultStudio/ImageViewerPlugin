#pragma once

#include "Layer.h"

#include <QAbstractListModel>

class LayersModel : public QAbstractListModel 
{
public:

    /**  Columns */
    enum Column {
        Name,           /** Name of the layer */
        ImageWidth,     /** Width of the image(s) */
        ImageHeight,    /** Height of the image(s) */
        Scale,          /** Layer scale */
        Opacity,        /** Layer opacity */

        Count = Opacity + 1
    };

public:
    explicit LayersModel(QObject* parent);
    ~LayersModel();

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
    QVariant data(const QModelIndex &index, int role) const override;

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

    void addLayer(const SharedLayer& layer);
    void removeLayer(const QModelIndex& index);
    void moveLayer(const QModelIndex& layerModelIndex, const std::int32_t& amount = 1);

protected:
    QVector<SharedLayer>    _layers;        /** Layers data */
};
