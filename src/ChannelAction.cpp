#include "ChannelAction.h"
#include "ImageAction.h"
#include "Layer.h"

#include "util//Exception.h"

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

ChannelAction::ChannelAction(ImageAction& imageAction, const ChannelIndex& index, const QString& name) :
    WidgetAction(reinterpret_cast<QObject*>(&imageAction)),
    _imageAction(imageAction),
    _index(index),
    _enabledAction(this, "Enabled"),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(this),
    _scalarData(),
    _scalarDataRange({0.0f, 0.0f}),
    _selectionData(),
    _selectionBoundaries()
{
    setText(name);
    setMayReset(true);
    setDefaultWidgetFlags(ChannelAction::ComboBox | ChannelAction::WindowLevelWidget);

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

    connect(&_imageAction.getSubsampleFactorAction(), &IntegralAction::valueChanged, this, [this]() {
        computeScalarData();
    });

    // Compute scalar data when the action is enabled
    connect(this, &QAction::changed, this, [this]() -> void {
        computeScalarData();
    });

    const auto resizeScalars = [this]() {

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
                _selectionData.resize(getImages()->getNumberOfPixels());
                break;
            }

            default:
                break;
        }
    };

    const auto updateEnabled = [this]() -> void {
        setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateEnabled);

    // Flag as changed when the window level settings change
    connect(&_windowLevelAction, &WindowLevelAction::changed, this, [this]() {
        emit changed(*this);
    });

    // Flag as changed when the window level settings change
    connect(&_imageAction.getSubsampleFactorAction(), &IntegralAction::valueChanged, this, resizeScalars);

    updateEnabled();
    resizeScalars();

    computeScalarData();
}

const ChannelAction::ChannelIndex ChannelAction::getIndex() const
{
    return _index;
}

QSize ChannelAction::getImageSize()
{
    const auto imageSize        = getImages()->getImageSize();
    const auto subsampleFactor  = _imageAction.getSubsampleFactorAction().getValue();
    
    return QSize(static_cast<int>(floorf(imageSize.width() / subsampleFactor)), static_cast<int>(floorf(imageSize.width() / subsampleFactor)));
}

const QVector<float>& ChannelAction::getScalarData() const
{
    return _scalarData;
}

const QPair<float, float>& ChannelAction::getScalarDataRange() const
{
    return _scalarDataRange;
}

QPair<float, float> ChannelAction::getDisplayRange()
{
    QPair<float, float> displayRange;

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

QRect ChannelAction::getSelectionBoundaries() const
{
    return _selectionBoundaries;
}

bool ChannelAction::isResettable() const
{
    return _dimensionAction.isResettable();
}

void ChannelAction::reset()
{
    _dimensionAction.reset();
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

                getImages()->getScalarData(_dimensionAction.getCurrentIndex(), _scalarData, _scalarDataRange, _imageAction.getSubsampleFactorAction().getValue());

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

        // Publish scalar data change
        emit changed(*this);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to compute scalar data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to compute scalar data");
    }
}

void ChannelAction::computeMaskChannel()
{
    qDebug() << "Compute mask for channel" << _index << QString("(%1)").arg(_imageAction.getLayer().getGeneralAction().getNameAction().getString());

    // Future implementations can use external masks, for now just leave opaque
    std::fill(_scalarData.begin(), _scalarData.end(), 1.0f);
}

void ChannelAction::computeSelectionChannel()
{
    qDebug() << "Compute selection for channel" << _index << QString("(%1)").arg(_imageAction.getLayer().getGeneralAction().getNameAction().getString());

    // Fill with non-selected
    std::fill(_selectionData.begin(), _selectionData.end(), 0);

    // Initialize selection boundaries with numeric extremes
    _selectionBoundaries.setTop(std::numeric_limits<int>::max());
    _selectionBoundaries.setBottom(std::numeric_limits<int>::lowest());
    _selectionBoundaries.setLeft(std::numeric_limits<int>::max());
    _selectionBoundaries.setRight(std::numeric_limits<int>::lowest());

    // Convert image width to floating point for division later
    const auto width = static_cast<float>(getImageSize().width());

    // Establish selected pixel boundaries
    for (auto selectionIndex : _imageAction.getLayer().getSelectedIndices()) {

        // Assign selected pixel
        _selectionData[selectionIndex] = 255;

        // Deduce pixel coordinate
        auto pixelCoordinate = QPoint(selectionIndex % getImageSize().width(), static_cast<int>(floorf(selectionIndex / width)));

        // Add pixel pixel coordinate and possibly inflate the selection boundaries
        _selectionBoundaries.setLeft(std::min(_selectionBoundaries.left(), pixelCoordinate.x()));
        _selectionBoundaries.setRight(std::max(_selectionBoundaries.right(), pixelCoordinate.x()));
        _selectionBoundaries.setTop(std::min(_selectionBoundaries.top(), pixelCoordinate.y()));
        _selectionBoundaries.setBottom(std::max(_selectionBoundaries.bottom(), pixelCoordinate.y()));
    }
}

hdps::util::DatasetRef<Images>& ChannelAction::getImages()
{
    return _imageAction.getLayer().getImages();
}

hdps::util::DatasetRef<Points>& ChannelAction::getPoints()
{
    return _imageAction.getLayer().getPoints();
}

QWidget* ChannelAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    auto widget = new WidgetActionWidget(parent, this, state);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & ChannelAction::ComboBox)
        layout->addWidget(_dimensionAction.createWidget(widget));

    if (widgetFlags & WidgetFlag::WindowLevelWidget)
        layout->addWidget(_windowLevelAction.createCollapsedWidget(widget));

    if (widgetFlags & ChannelAction::ResetPushButton)
        layout->addWidget(createResetButton(widget));

    widget->setLayout(layout);

    return widget;
}