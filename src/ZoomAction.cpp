#include "ZoomAction.h"
#include "GeneralAction.h"
#include "Layer.h"

#include <QHBoxLayout>

using namespace hdps;

ZoomAction::ZoomAction(GeneralAction& generalAction) :
    WidgetAction(reinterpret_cast<QObject*>(&generalAction)),
    _generalAction(generalAction),
    _zoomExtentsAction(this, "Extents"),
    _zoomSelectionAction(this, "Selected pixels")
{
    setText("Zoom");

    // Zoom to layer extents
    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this]() {
        _generalAction.getLayer().zoomToExtents();
    });

    // Zoom to selection
    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this]() {
        _generalAction.getLayer().zoomToSelection();
    });

    // Enable zoom to selected pixels when the number of pixels is non-zero
    const auto selectionChanged = [this]() {
        _zoomSelectionAction.setEnabled(!_generalAction.getLayer().getSelectedIndices().empty());
    };

    // Re-compute the selection channel when the selection changes
    connect(&_generalAction.getLayer(), &Layer::selectionChanged, this, selectionChanged);

    // Initial update zoom to selected pixels state
    selectionChanged();
}

QWidget* ZoomAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    layout->addWidget(_zoomExtentsAction.createWidget(widget));
    layout->addWidget(_zoomSelectionAction.createWidget(widget));

    widget->setLayout(layout);

    return widget;
}