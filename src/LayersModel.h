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

        Count = Scale + 1
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
     * Returns the data for the given model index and data role
     * @param index Model index
     * @param role Data role
     * @return Data in variant form
     */
    QVariant data(const QModelIndex &index, int role) const override;

public: /** Add/remove layer */
    void addLayer(const SharedLayer& layer);
    void removeLayer(const SharedLayer& layer);

protected:
    QVector<SharedLayer>    _layers;        /** Layers data */
};
