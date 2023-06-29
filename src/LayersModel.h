#pragma once

#include <util/Serializable.h>

#include <event/EventListener.h>

#include "Layer.h"

/**
 * Layers model class
 *
 * Standard item model class for layers
 *
 * @author Thomas Kroes
 */
class LayersModel final : public QStandardItemModel, public hdps::util::Serializable
{
public:

    /**  Columns */
    enum class Column {
        Visible,        /** Visibility of the layer */
        Color,          /** Color of the layer */
        Name,           /** Name of the layer */
        DatasetName,    /** Dataset name */
        DatasetId,      /** Dataset GUI */
        ImageWidth,     /** Width of the image(s) */
        ImageHeight,    /** Height of the image(s) */
        Scale,          /** Layer scale */
        Opacity,        /** Layer opacity */

        Count
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Header standard model item class */
    class HeaderItem : public QStandardItem {
    public:

        /**
         * Construct with \p columHeaderInfo
         * @param columHeaderInfo Column header info
         */
        HeaderItem(const ColumHeaderInfo& columHeaderInfo);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

    private:
        ColumHeaderInfo     _columHeaderInfo;   /** Column header info */
    };

public:

    /** Base standard model item class for layer */
    class Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with \p layer
         * @param action Pointer to layer to display item for
         * @param editable Whether the model item is editable or not
         */
        Item(Layer* layer, bool editable = false);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get layer
         * return Pointer to layer to display item for
         */
        QPointer<Layer> getLayer() const;

    private:
        QPointer<Layer>     _layer;    /** Pointer to layer to display item for */
    };

protected:

    /** Standard model item class for the layer visibility */
    class VisibleItem final : public Item {
    public:

        /**
         * Construct with \p layer
         * @param layer Pointer to layer to display item for
         */
        VisibleItem(Layer* layer);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for the layer color */
    class ColorItem final : public Item {
    public:

        /**
         * Construct with \p layer
         * @param layer Pointer to layer to display item for
         */
        ColorItem(Layer* layer);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;

    private:

        /**
         * Get color icon for the layer
         * @param color Input color
         * @return Icon
         */
        QIcon getColorIcon(const QColor& color) const;
    };

    /** Standard model item class for the layer name */
    class NameItem final : public Item {
    public:

        /**
         * Construct with \p layer
         * @param layer Pointer to layer to display item for
         */
        NameItem(Layer* layer);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for the layer dataset name */
    class DatasetNameItem final : public Item {
    public:

        /**
         * Construct with \p layer
         * @param layer Pointer to layer to display item for
         */
        DatasetNameItem(Layer* layer);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for the layer dataset GUID */
    class DatasetIdItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for the layer width */
    class WidthItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for the layer height */
    class HeightItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for the layer scale */
    class ScaleItem final : public Item {
    public:

        /**
         * Construct with \p layer
         * @param layer Pointer to layer to display item for
         */
        ScaleItem(Layer* layer);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for the layer opacity */
    class OpacityItem final : public Item {
    public:

        /**
         * Construct with \p layer
         * @param layer Pointer to layer to display item for
         */
        OpacityItem(Layer* layer);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Convenience class for combining action items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct row with \p layer
         * @param layer Pointer to row layer
         */
        Row(Layer* layer);
    };

public:

    using Layers = QVector<Layer*>;

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit LayersModel(QObject* parent);
    
    /** Destructor */
    ~LayersModel();

    /**
     * Get item flags
     * @param index Model index
     * @return Item flags
     * Constructor
     * @param parent Pointer to parent object
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public: // Layer operations

    /**
     * Add a layer to the model
     * @param layer Pointer to layer
     */
    void addLayer(Layer* layer);

    /**
     * Remove a layer from the model by row index
     * @param row Row index of the layer
     */
    void removeLayer(const std::uint32_t& row);

    /**
     * Remove a layer from the model by images dataset globally unique identifier
     * @param datasetId Globally unique identifier of the images dataset
     */
    void removeLayer(const QString& datasetId);

    /**
     * Remove a layer from the model by model index of the layer to remove
     * @param layerModelIndex Layer model index
     */
    void removeLayer(const QModelIndex& layerModelIndex);

    /**
     * Move a layer in the model by amount
     * @param layerModelIndex Layer model index of the layer to move
     * @param amount Amount of layers to move up/down
     */
    void moveLayer(const QModelIndex& layerModelIndex, const std::int32_t& amount = 1);

    /**
     * Get layer from \p layerModelIndex
     * @param layerModelIndex Layer model index
     * @return Pointer to layer if found, nullptr otherwise
     */
    Layer* getLayerFromIndex(const QModelIndex& layerModelIndex) const;

    /** Get vector of pointers to layers */
    Layers getLayers() const;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
    
    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

protected:
    hdps::EventListener     _eventListener;     /** Listen to HDPS events */
};
