#include "graphs/edge.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

IEdge::IEdge(int v, int w, double weight) : v_(v), w_(w), weight_(weight) {
    if (v < 0 || w < 0) {
        throw std::invalid_argument("Vertex indices must be non-negative");
    }
    if (std::isnan(weight)) {
        throw std::invalid_argument("Weight must be a valid number");
    }
}

int IEdge::from() const { return v_; }

int IEdge::to() const { return w_; }

double IEdge::weight() const { return weight_; }

std::string IEdge::toString() const {
    std::ostringstream oss;
    oss << v_ << " -> " << w_ << " "
        << "(" << weight_ << ")";
    return oss.str();
}

