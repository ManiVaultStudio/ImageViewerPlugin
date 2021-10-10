#include "ZoomAction.h"
#include "GeneralAction.h"
#include "Layer.h"

#include <QHBoxLayout>

using namespace hdps;

ZoomAction::ZoomAction(GeneralAction& generalAction) :
    WidgetAction(reinterpret_cast<QObject*>(&generalAction)),
    EventListener(),
    _generalAction(generalAction),
    _zoomExtentsAction(this, "Extents"),
    _zoomSelectionAction(this, "Selected pixels")
{
    setText("Zoom");
    setMayReset(false);

    // Enable registering for core events
    setEventCore(Application::core());

    // Zoom to layer extents
    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this]() {
        _generalAction.getLayer().zoomToExtents();
    });

    // Zoom to selection
    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this]() {
        _generalAction.getLayer().zoomToSelection();
    });

    // Enable zoom to selected pixels when the number of pixels is non-zero
    const auto onSelectionChanged = [this]() {
        _zoomSelectionAction.setEnabled(!_generalAction.getLayer().getSelectedIndices().empty());
    };

    // Re-compute the selection channel when the selection changes
    registerDataEventByType(PointType, [this, onSelectionChanged](hdps::DataEvent* dataEvent) {
        switch (dataEvent->getType())
        {
            case EventType::SelectionChanged:
            {
                if (dataEvent->dataSetName == _generalAction.getLayer().getPoints()->getName())
                    onSelectionChanged();

                break;
            }

            default:
                break;
        }
    });

    // Initial update zoom to selected pixels state
    onSelectionChanged();
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