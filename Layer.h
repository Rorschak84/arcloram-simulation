#include <vector>
#include <cstdint>

class Layer {
public:
    virtual ~Layer() = default;

    virtual void send_down(const std::vector<uint8_t>& data) = 0;
    virtual void send_up(const std::vector<uint8_t>& data) = 0;

    void set_above(Layer* layer) { above = layer; }
    void set_below(Layer* layer) { below = layer; }

protected:
    Layer* above = nullptr; // Layer above this one
    Layer* below = nullptr; // Layer below this one
};
