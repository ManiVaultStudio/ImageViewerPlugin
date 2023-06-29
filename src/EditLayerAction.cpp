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
        if (projects().isOpeningProject() || projects().isImportingProject())
            return;

        const auto selectedRows     = imageViewerPlugin->getSelectionModel().selectedRows();
        const auto hasSelection     = !selectedRows.isEmpty();
        const auto multiSelection   = selectedRows.count() >= 2;

        GroupsAction::GroupActions groupActions;

        if (hasSelection && !multiSelection) {
            auto layer = imageViewerPlugin->getLayersModel().getLayerFromIndex(selectedRows.first());

            Q_ASSERT(layer != nullptr);

            if (layer == nullptr)
                return;

            groupActions << &layer->getGeneralAction();
            groupActions << &layer->getImageSettingsAction();
            groupActions << &layer->getSelectionAction();
            //groupActions << &layer->getMiscellaneousAction();
            groupActions << &layer->getSubsetAction();
        }
        
        setGroupActions(groupActions);

        imageViewerPlugin->getImageViewerWidget().getPixelSelectionTool().setEnabled(hasSelection);
    };

    connect(&imageViewerPlugin->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, modelSelectionChanged);
}
