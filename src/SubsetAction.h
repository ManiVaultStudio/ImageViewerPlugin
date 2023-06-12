#pragma once

#include <actions/GroupAction.h>
#include <actions/TriggerAction.h>
#include <actions/StringAction.h>

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Subset action class
 *
 * Action class for creating images/points subsets
 *
 * @author Thomas Kroes
 */
class SubsetAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE SubsetAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p imageViewerPlugin
     * @param imageViewerPlugin Pointer to image viewer plugin
     */
    void initialize(ImageViewerPlugin* imageViewerPlugin);

public: // Action getters

    StringAction& getNameAction() { return _nameAction; }
    TriggerAction& getCreateAction() { return _createAction; }

protected:
    ImageViewerPlugin*  _imageViewerPlugin;     /** Pointer to image viewer plugin */
    StringAction        _nameAction;            /** Subset name action */
    TriggerAction       _createAction;          /** Create subset action */
};

Q_DECLARE_METATYPE(SubsetAction)

inline const auto subsetActionMetaTypeId = qRegisterMetaType<SubsetAction*>("SubsetAction");