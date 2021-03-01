#pragma once

#include "ColorMapModel.h"

#include <QComboBox>
#include <QSortFilterProxyModel>

class QListView;

/**
 * Color map combobox class
 *
 * A combobox widget for picking color maps
 * Support for 1D and 2D color maps
 * Connects to the color map model class
 *
 * @author Thomas Kroes
 */
class ColorMapComboBox : public QComboBox
{
protected:

    /**
     * Color map filter model class
     *
     * A class for filtering out 1D or 2D color maps from the color map model
     *
     * @author Thomas Kroes
     */
    class ColorMapFilterModel : public QSortFilterProxyModel {
    public:

        /** Constructor */
        ColorMapFilterModel(QObject *parent, const ColorMap::Type& type = ColorMap::Type::OneDimensional) :
            QSortFilterProxyModel(parent),
            _type(type)
        {
        }

        /**
         * Returns whether a give row with give parent is filtered out (false) or in (true)
         * @param row Row index
         * @param parent Parent index
         */
        bool filterAcceptsRow(int row, const QModelIndex& parent) const override
        {
            const auto index = sourceModel()->index(row, ult(ColorMapModel::Column::NoDimensions));
            return ult(_type) == sourceModel()->data(index, Qt::EditRole).toInt();
        }

        /** Returns the type of color map */
        ColorMap::Type type() const {
            return _type;
        }

        /**
         * Sets the type of color map
         * @param type Type of color map (e.g. 1D, 2D)
         */
        void setType(const ColorMap::Type& type) {
            if (type == _type)
                return;

            _type = type;
            invalidateFilter();
        }

    private:
        ColorMap::Type      _type;      /** Type of color map (e.g. 1D, 2D) */
    };

public: // Construction/destruction

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param type Type of color map
     */
    ColorMapComboBox(QWidget* parent, const ColorMap::Type& type = ColorMap::Type::OneDimensional);

public: 

    /**
     * Sets the associated color map model
     * @param colorMapModel The color map model
     */
    void setModel(ColorMapModel* colorMapModel);

    /** Returns the type of color map */
    ColorMap::Type getType() const;

    /**
    * Sets the type of color map
    * @param type Type of color map (e.g. 1D, 2D)
    */
    void setType(const ColorMap::Type& type);

    /** Returns the currently selected color map image */
    QImage getCurrentImage() const;

signals:

    /** Signals that a color map has been picked
     * @param colorMap Pointer to the picked color map
     */
    void colorMapPicked(ColorMap* colorMap);

private:
    ColorMapFilterModel     _filteredColorMapModel;     /** The filtered color map model (contains either 1D or 2D color maps) */
    QListView*              _view;                      /** List view which displays the color maps when the combobox is opened */
};