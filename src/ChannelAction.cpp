#include "ChannelAction.h"
#include "LayerAction.h"
#include "LayerImageAction.h"
#include "Layer.h"

#include <QHBoxLayout>
#include <QCheckBox>

using namespace hdps;

const QMap<ChannelAction::ChannelIndex, QString> ChannelAction::channelIndexes = {
    { ChannelAction::Channel1, "Channel 1" },
    { ChannelAction::Channel2, "Channel 2" },
    { ChannelAction::Channel3, "Channel 3" },
    { ChannelAction::Mask, "Mask channel" },
    { ChannelAction::Selection, "Selection channel" }
};

ChannelAction::ChannelAction(LayerImageAction& layerImageAction, const ChannelIndex& index, const QString& name) :
    WidgetAction(reinterpret_cast<QObject*>(&layerImageAction)),
    EventListener(),
    _layerImageAction(layerImageAction),
    _index(index),
    _enabledAction(this, "Enabled"),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(*this),
    _scalarData()
{
    setText(name);
    setEventCore(Application::core());

    switch (_index)
    {
        case Channel1:
        case Mask:
        case Selection:
            _enabledAction.setChecked(true);
            break;

        default:
            break;
    }

    connect(&_dimensionAction, &OptionAction::resettableChanged, this, [this]() {
        setResettable(isResettable());
    });

    connect(&_dimensionAction, &OptionAction::currentIndexChanged, this, [this]() -> void {
        computeScalarData();
    });

    // Compute scalar data when the action is enabled
    connect(this, &QAction::changed, this, [this]() -> void {
        if (isEnabled())
            computeScalarData();
    });

    // Allocate space for the scalar data
    _scalarData.resize(getImages()->getNumberOfPixels());

    // Re-compute the selection scalar data when the points selection changes
    registerDataEventByType(PointType, [this](hdps::DataEvent* dataEvent) {
        if (dataEvent->getType() == hdps::EventType::SelectionChanged) {
            auto selectionChangedEvent = static_cast<hdps::SelectionChangedEvent*>(dataEvent);

            if (selectionChangedEvent->dataSetName != getPoints()->getName() || _index != Selection)
                return;

            computeScalarData();
        }
    });
}

bool ChannelAction::isResettable() const
{
    return _dimensionAction.isResettable();
}

void ChannelAction::reset()
{
    _dimensionAction.reset();
}

hdps::util::DatasetRef<Images>& ChannelAction::getImages()
{
    return _layerImageAction.getLayerAction().getLayer().getImages();
}

hdps::util::DatasetRef<Points>& ChannelAction::getPoints()
{
    return _layerImageAction.getLayerAction().getLayer().getPoints();
}

std::vector<std::uint32_t> ChannelAction::getSelectionIndices()
{
    auto& selection = dynamic_cast<Points&>(getPoints()->getSelection());
    return selection.indices;
}

void ChannelAction::computeScalarData()
{
    try
    {
        if (!_enabledAction.isChecked())
            return;

        if (!getPoints().isValid())
            throw std::runtime_error("Points dataset is not valid");

        if (!getImages().isValid())
            throw std::runtime_error("Images dataset is not valid");

        switch (_index)
        {
            case Channel1:
            case Channel2:
            case Channel3:
            {
                if (_dimensionAction.getCurrentIndex() <= 0)
                    break;

                switch (getImages()->getType())
                {
                    case ImageData::Sequence:
                        computeScalarDataForImageSequence();
                        break;

                    case ImageData::Stack:
                        computeScalarDataForImageStack();
                        break;

                    default:
                        break;
                }

                break;
            }

            case Mask:
            {
                computeMaskChannel();
                break;
            }

            case Selection:
            {
                computeSelectionChannel();
                break;
            }

            default:
                break;
        }
        
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to compute scalar data", e.what());
    }
}

void ChannelAction::computeScalarDataForImageSequence()
{
    qDebug() << "Compute scalar data for image sequence";

    getPoints()->visitData([this](auto pointData) {
        const auto dimensionId      = _dimensionAction.getCurrentIndex();
        const auto imageSize        = _layerImageAction.getLayerAction().getLayer().getImageSize();
        const auto noPixels         = getImages()->getNumberOfPixels();
        const auto selectionIndices = getSelectionIndices();
        const auto selectionSize    = selectionIndices.size();

        if (!selectionIndices.empty()) {
            for (std::uint32_t p = 0; p < noPixels; p++) {
                auto sum = 0.0f;

                for (auto selectionIndex : selectionIndices)
                    sum += pointData[selectionIndex][p];

                _scalarData[p] = static_cast<float>(sum / selectionSize);
            }
        }
        else {
            for (std::uint32_t p = 0; p < noPixels; p++)
                _scalarData[p] = pointData[dimensionId][p];
        }
    });
}

void ChannelAction::computeScalarDataForImageStack()
{
    qDebug() << "Compute scalar data for image stack";

    const auto dimensionId = _dimensionAction.getCurrentIndex();

    if (getPoints()->isDerivedData()) {
        getPoints()->visitData([this, dimensionId](auto pointData) {
            auto& sourceData = getPoints()->getSourceData<Points>(*getPoints());
            
            if (sourceData.isFull()) {
                for (std::uint32_t i = 0; i < getPoints()->getNumPoints(); i++)
                    _scalarData[i] = pointData[i][dimensionId];
            }
            else {
                for (int i = 0; i < sourceData.indices.size(); i++)
                    _scalarData[sourceData.indices[i]] = pointData[i][dimensionId];
            }
        });
    }
    else {
        getPoints()->visitSourceData([this, dimensionId](auto pointData) {
            for (auto pointView : pointData)
                _scalarData[pointView.index()] = pointView[dimensionId];
        });
    }
}

void ChannelAction::computeMaskChannel()
{
    qDebug() << "Compute mask channel";

    if (getImages()->getType() != ImageData::Type::Stack)
        return;

    if (getPoints()->isDerivedData()) {
        getPoints()->visitData([this](auto pointData) {
            auto& sourceData = getPoints()->getSourceData<Points>(*getPoints());

            if (sourceData.isFull()) {
                for (std::uint32_t i = 0; i < getPoints()->getNumPoints(); i++)
                    _scalarData[i] = 1.0f;
            }
            else {
                for (std::uint32_t i = 0; i < sourceData.indices.size(); i++)
                    _scalarData[sourceData.indices[i]] = 1.0f;
            }
        });
    }
    else {
        getPoints()->visitData([this](auto pointData) {
            for (auto pointView : pointData)
                _scalarData[pointView.index()] = 1.0f;
        });
    }
}

void ChannelAction::computeSelectionChannel()
{
    qDebug() << "Compute selection channel";

    /*
    auto& selectionChannel = (*getChannel(ult(ChannelIndex::Selection)));
    
    selectionChannel.setImageSize(getImageSize());
    selectionChannel.fill(0);
    
    for (auto selectionIndex : getSelectionIndices())
        selectionChannel[selectionIndex] = 255;
    
    selectionChannel.setChanged();
    
    emit channelChanged(ult(ChannelIndex::Selection));
    */
}

ChannelAction::Widget::Widget(QWidget* parent, ChannelAction* channelAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, channelAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(0);

    auto checkBox = new QCheckBox();

    if (channelAction->hasWidgetFlag(ChannelAction::ComboBox))
        layout->addWidget(channelAction->getDimensionAction().createWidget(this));

    if (channelAction->hasWidgetFlag(WidgetFlag::WindowLevelWidget))
        layout->addWidget(channelAction->getWindowLevelAction().createCollapsedWidget(this));

    if (channelAction->hasWidgetFlag(ChannelAction::ResetPushButton))
        layout->addWidget(channelAction->createResetButton(this));

    setLayout(layout);
}

