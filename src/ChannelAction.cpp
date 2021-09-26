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
    _layerImageAction(layerImageAction),
    _index(index),
    _enabledAction(this, "Enabled"),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(*this),
    _scalarData(),
    _scalarDataRange({0.0f, 0.0f}),
    _selectionData()
{
    setText(name);

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

    connect(&_dimensionAction, &OptionAction::currentIndexChanged, this, [this]() {
        computeScalarData();
    });

    // Compute scalar data when the action is enabled
    connect(this, &QAction::changed, this, [this]() -> void {
        computeScalarData();
    });

    // Get number of pixels
    const auto numberOfPixels = getImages()->getNumberOfPixels();

    // Allocate space for the scalar data
    switch (_index)
    {
        case Channel1:
        case Channel2:
        case Channel3:
        case Mask:
        {
            _scalarData.resize(numberOfPixels);
            break;
        }

        case Selection:
        {
            _selectionData.resize(numberOfPixels);
            break;
        }

        default:
            break;
    }

    const auto updateEnabled = [this]() -> void {
        setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateEnabled);

    // Flag as changed when the window level settings change
    connect(&_windowLevelAction, &WindowLevelAction::changed, this, [this]() {
        emit changed(*this);
    });

    updateEnabled();

    computeScalarData();
}

const ChannelAction::ChannelIndex ChannelAction::getIndex() const
{
    return _index;
}

const std::vector<float>& ChannelAction::getScalarData() const
{
    return _scalarData;
}

const std::pair<float, float>& ChannelAction::getScalarDataRange() const
{
    return _scalarDataRange;
}

std::pair<float, float> ChannelAction::getDisplayRange()
{
    std::pair<float, float> displayRange;

    const auto maxWindow        = _scalarDataRange.second - _scalarDataRange.first;
    const auto windowNormalized = _windowLevelAction.getWindowAction().getValue();
    const auto levelNormalized  = _windowLevelAction.getLevelAction().getValue();
    const auto level            = std::clamp(_scalarDataRange.first + (levelNormalized * maxWindow), _scalarDataRange.first, _scalarDataRange.second);
    const auto window           = std::clamp(windowNormalized * maxWindow, _scalarDataRange.first, _scalarDataRange.second);

    displayRange.first  = std::clamp(level - (window / 2.0f), _scalarDataRange.first, _scalarDataRange.second);
    displayRange.second = std::clamp(level + (window / 2.0f), _scalarDataRange.first, _scalarDataRange.second);

    return displayRange;
}

const std::vector<std::uint8_t>& ChannelAction::getSelectionData() const
{
    return _selectionData;
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
                if (_dimensionAction.getCurrentIndex() < 0)
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

        // Compute scalar data range for the first three channels
        switch (_index)
        {
            case Channel1:
            case Channel2:
            case Channel3:
            case Mask:
            {
                computeScalarDataRange();
                break;
            }

            case Selection:
                break;

            default:
                break;
        }

        // Publish scalar data change
        emit changed(*this);
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to compute scalar data", e.what());
    }
    catch (...) {
        QMessageBox::critical(nullptr, "Unable to compute scalar data", "An unhandled exception occurred");
    }
}

void ChannelAction::computeScalarDataForImageSequence()
{
    qDebug() << "Compute image sequence scalar for channel" << _index;

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
    qDebug() << "Compute image stack scalars for channel" << _index;

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
    qDebug() << "Compute mask for channel" << _index;

    if (getImages()->getType() != ImageData::Type::Stack)
        return;

    std::fill(_scalarData.begin(), _scalarData.end(), 1.0f);

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
    qDebug() << "Compute selection for channel" << _index;

    // Fill with non-selected
    std::fill(_selectionData.begin(), _selectionData.end(), 0.0f);
    
    // Assign selected pixels
    for (auto selectionIndex : getSelectionIndices())
        _selectionData[selectionIndex] = 255;
}

void ChannelAction::computeScalarDataRange()
{
    qDebug() << "Compute scalar range for channel" << _index;

    // Initialize scalar data range
    _scalarDataRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

    // Compute the actual scalar data range
    for (auto& scalar : _scalarData) {
        _scalarDataRange.first  = std::min(scalar, _scalarDataRange.first);
        _scalarDataRange.second = std::max(scalar, _scalarDataRange.second);
    }
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

