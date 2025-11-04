#include "graphs/INode.h"

class Node : public INode {
public:
    int getId() { return id_; }

private:
    int id_;
};
