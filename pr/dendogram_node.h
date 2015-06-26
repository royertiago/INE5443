#ifndef DENDOGRAM_NODE_H
#define DENDOGRAM_NODE_H

/* Nodes of a dendogram.
 *
 * A dendogram node can be either structural
 * (it will have only left and right child pointers and no data)
 * or leaf
 * (it will have no child pointers and a single data pointer).
 */

#include <memory>
class DataEntry;

class DendogramNode {
    std::unique_ptr<DendogramNode> _left, _right;
    DataEntry * _data;

public:
    /* Constructs a structural dendogram node.
     */
    DendogramNode(
        std::unique_ptr<DendogramNode> && left_child,
        std::unique_ptr<DendogramNode> && right_child
    );

    /* Constructs a leaf dendogram node.
     * The given data entry will not be deleted upon destruction.
     */
    DendogramNode( DataEntry * );

    /* Queries wether this node is structural or leaf.
     * It is guaranteed that structural() == !leaf().
     */
    bool structural() const;
    bool leaf() const;

    /* Returns the left/right child of this node.
     * Invokes undefined behavior if structural() returns false.
     */
    const DendogramNode & left() const;
    const DendogramNode & right() const;

    /* Returns the data of this node.
     * Invokes undefined behavior if leaf() returns false.
     */
    const DataEntry & data() const;
};

#endif // DENDOGRAM_NODE_H
