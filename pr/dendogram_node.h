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
class DendogramNode;

class DendogramIterator {
    // Past-the-end iterators are represented with _node == nullptr.
    const DendogramNode * _node;
    friend class DendogramNode;
public:
    typedef const DataEntry value_type;
    typedef const DataEntry & reference;
    typedef const DataEntry * pointer;
    typedef std::forward_iterator_tag iterator_category;

    reference operator*() const;
    pointer operator->() const;
    DendogramIterator & operator++();
    DendogramIterator operator++(int) {
        auto tmp = *this;
        ++*this;
        return tmp;
    }
    friend bool operator==(const DendogramIterator &, const DendogramIterator &);
    friend bool operator!=(const DendogramIterator &, const DendogramIterator &);
};

class DendogramNode {
    std::unique_ptr<DendogramNode> _left, _right;
    DendogramNode * _parent;
    const DataEntry * _data;

    // Additional information
    double _linkage_distance;
    unsigned _size;
    unsigned _depth;

public:
    /* Constructs a structural dendogram node.
     * Each child will have its _parent attribute set to 'this'.
     */
    DendogramNode(
        std::unique_ptr<DendogramNode> && left_child,
        std::unique_ptr<DendogramNode> && right_child,
        double linkage_distance
    );

    /* Constructs a leaf dendogram node.
     * The given data entry will not be deleted upon destruction.
     */
    DendogramNode( const DataEntry * );

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

    /* Returns the parent of this node.
     * If this node has no parent, returns nullptr.
     */
    const DendogramNode * parent() const;

    /* Returns the data of this node.
     * Invokes undefined behavior if leaf() returns false.
     */
    const DataEntry & data() const;

    // The iterator meets the requirements of an ForwardIterator.
    DendogramIterator begin() const;
    DendogramIterator end() const;

    double linkage_distance() const;
    unsigned size() const;
    unsigned depth() const;
};

#endif // DENDOGRAM_NODE_H
