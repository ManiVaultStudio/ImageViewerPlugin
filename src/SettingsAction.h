#pragma once

#include <actions/GroupAction.h>

#include "EditLayersAction.h"
#include "EditLayerAction.h"

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Settings action class
 *
 * Action class for image viewer plugin settings (panel on the right of the view)
 *
 * @author Thomas Kroes
 */
class SettingsAction : public GroupAction
{
public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE SettingsAction(QObject* parent, const QString& title);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /** Get reference to the image viewer plugin */
    ImageViewerPlugin& getImageViewerPlugin() { return _imageViewerPlugin; };

public: // Action getters

    EditLayersAction& getEditLayersAction() { return _editLayersAction; }
    EditLayerAction& getEditLayerAction() { return _editLayerAction; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;     /** Reference to image viewer plugin */
    EditLayersAction        _editLayersAction;      /** Action for managing layers */
    EditLayerAction         _editLayerAction;       /** Action for editing the selected layer in the edit layers action */
};
