#pragma once

#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"
#include "actions/StringAction.h"

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Subset action class
 *
 * Action class for creating images/points subsets
 *
 * @author Thomas Kroes
 */
class SubsetAction : public TriggerAction
{
protected: // Widget

    /** Widget class for subset action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param subsetAction Pointer to subset action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, SubsetAction* subsetAction, const std::int32_t& widgetFlags);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    SubsetAction(ImageViewerPlugin& imageViewerPlugin);

    /** Get reference to image viewer plugin */
    ImageViewerPlugin& getImageViewerPlugin() { return _imageViewerPlugin; }

public: // Action getters

    ToggleAction& getFromRegionAction() { return _fromRegionAction; }
    StringAction& getNameAction() { return _nameAction; }
    TriggerAction& getCreateAction() { return _createAction; }

protected:
    ImageViewerPlugin&  _imageViewerPlugin;     /** Reference to image viewer plugin */
    StringAction        _nameAction;            /** Subset name action */
    ToggleAction        _fromRegionAction;      /** Whether to create an image set from the selected region of pixels */
    TriggerAction       _createAction;          /** Create subset action */
};
