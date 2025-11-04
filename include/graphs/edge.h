#ifndef EDGE_H
#define EDGE_H
#include <string>

class IEdge {
public:
    //directed
    IEdge(int v, int w, double weight);
    double weight() const;
    int from() const;
    int to() const;
    std::string toString() const;

private:
    int v_;
    int w_;
    double weight_;
};
#endif //EDGE_H
