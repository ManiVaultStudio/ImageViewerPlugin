#pragma once

#include <actions/ToggleAction.h>
#include <actions/GroupAction.h>
#include <actions/PixelSelectionAction.h>
#include <actions/TriggerAction.h>

#include <util/PixelSelectionTool.h>

class QWidget;

class Layer;

using namespace hdps::util;
using namespace hdps::gui;

/**
 * Selection action class
 *
 * Action class for selection
 *
 * @author Thomas Kroes
 */
class SelectionAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE SelectionAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p layer, \p targetWidget and \p pixelSelectionTool
     * @param layer Pointer to layer
     * @param targetWidget Target widget to draw the selection
     * @param pixelSelectionTool Tool for selecting the pixels
     */
    void initialize(Layer* layer, QWidget* targetWidget, PixelSelectionTool* pixelSelectionTool);

    /** Get selection rectangle in image coordinates */
    QRect getImageSelectionRectangle() const;

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

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

public: // Action getters

    PixelSelectionAction& getPixelSelectionAction() { return _pixelSelectionAction; }
    ToggleAction& getShowRegionAction() { return _showRegionAction; }

protected:
    Layer*                  _layer;                     /** Pointer to owning layer */
    QWidget*                _targetWidget;              /** Pointer to target widget */
    PixelSelectionAction    _pixelSelectionAction;      /** Pixel selection action */
    PixelSelectionTool*     _pixelSelectionTool;        /** Pointer to pixel selection tool */
    ToggleAction            _showRegionAction;          /** Show region action */
};

Q_DECLARE_METATYPE(SelectionAction)

inline const auto selectionActionMetaTypeId = qRegisterMetaType<SelectionAction*>("SelectionAction");