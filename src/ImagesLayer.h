#pragma once

#include "Layer.h"

class Dataset;

/** TODO */
class ImagesLayer : public Layer
{
public:

	/** TODO */
	ImagesLayer(Dataset* dataset, const QString& id = "", const QString& name = "", const std::uint32_t& flags = Layer::Flags::Enabled);

public: // Getters/setters

private:
};