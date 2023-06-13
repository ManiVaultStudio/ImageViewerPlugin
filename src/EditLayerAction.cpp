#include "EditLayerAction.h"
#include "ImageViewerPlugin.h"

using namespace hdps;
using namespace hdps::gui;

EditLayerAction::EditLayerAction(QObject* parent, const QString& title) :
    GroupsAction(parent, title)
{
    setConnectionPermissionsToForceNone(true);

    auto imageViewerPlugin = static_cast<ImageViewerPlugin*>(parent->parent());

    const auto modelSelectionChanged = [this, imageViewerPlugin]() -> void {
        const auto selectedRows = imageViewerPlugin->getSelectionModel().selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        GroupsAction::GroupActions groupActions;

        if (hasSelection) {
            auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

            groupActions << &layer->getGeneralAction();
            groupActions << &layer->getImageSettingsAction();
            groupActions << &layer->getSelectionAction();
            groupActions << &layer->getMiscellaneousAction();
            groupActions << &layer->getSubsetAction();
        }

        setGroupActions(groupActions);

        imageViewerPlugin->getImageViewerWidget().getPixelSelectionTool().setEnabled(hasSelection);
    };

    connect(&imageViewerPlugin->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, modelSelectionChanged);
}
