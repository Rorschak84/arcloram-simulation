#include <memory>
#include <vector>
#include "Layer.h"


class LayerStack {
public:
    void add_layer(std::unique_ptr<Layer> layer) {
        if (!layers.empty()) {
            // Connect the new layer with the current top layer
            layer->set_below(layers.back().get());
            layers.back()->set_above(layer.get());
        }
        layers.push_back(std::move(layer));
    }

    void send_down(const std::vector<uint8_t>& data) {
        if (!layers.empty()) {
            layers.front()->send_down(data);
        }
    }

    void send_up(const std::vector<uint8_t>& data) {
        if (!layers.empty()) {
            layers.back()->send_up(data);
        }
    }

private:
    std::vector<std::unique_ptr<Layer>> layers;
};