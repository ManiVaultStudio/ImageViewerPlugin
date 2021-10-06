#include "ZoomAction.h"
#include "GeneralAction.h"
#include "Layer.h"

#include <QHBoxLayout>

using namespace hdps;

ZoomAction::ZoomAction(GeneralAction& generalAction) :
    WidgetAction(reinterpret_cast<QObject*>(&generalAction)),
    _generalAction(generalAction),
    _zoomExtentsAction(this, "Extents"),
    _zoomSelectionAction(this, "Selected")
{
    setText("Zoom");
    setMayReset(false);

    // Zoom to layer extents
    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this]() {
        _generalAction.getLayerAction().getLayer().zoomToExtents();
    });

    // Zoom to selection
    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this]() {
        _generalAction.getLayerAction().getLayer().zoomToSelection();
    });
}

QWidget* ZoomAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    auto widget = new WidgetActionWidget(parent, this, state);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    layout->addWidget(_zoomExtentsAction.createWidget(widget));
    layout->addWidget(_zoomSelectionAction.createWidget(widget));

    widget->setLayout(layout);

    return widget;
}