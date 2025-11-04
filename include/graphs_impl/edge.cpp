#include "graphs/edge.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

class Edge : public IEdge{
public:
    Edge(int v, int w, double weight): IEdge(v, w, weight) {
    if (v < 0 || w < 0) {
        throw std::invalid_argument("Vertex indices must be non-negative");
    }

    if (std::isnan(weight)) {
        throw std::invalid_argument("Weight must be non-negative");
    }
    }

    int from() const { return v_; }
    int to() const { return w_; }
    double weight() const { return weight_; }

    std::string toString() const {
        std::ostringstream oss;
        oss << v_ << " -> " << w_ << " "
            << "(" << weight_ << ")";

        return oss.str();
    }

private:
    int v_;
    int w_;
    double weight_;

};
