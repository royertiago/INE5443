#include <utility>
#include "dendogram_node.h"
#include "pr/data_entry.h"

DendogramIterator::const_reference DendogramIterator::operator*() const {
    return _node->data();
}

DendogramIterator & DendogramIterator::operator++() {
    /* Two phases: first, go up, while we are at the right node;
     * then, after we reach the first node by the left,
     * go to the right and descend by the left.
     */
    const DendogramNode * next = _node->parent();
    while( next != nullptr && &next->right() == _node ) {
        _node = next;
        next = next->parent();
    }
    _node = next;
    if( next == nullptr )
        return *this;
    _node = &next->right();
    while( !_node->leaf() )
        _node = &_node->left();
    return *this;
}

bool operator==(const DendogramIterator & lhs, const DendogramIterator & rhs) {
    return lhs._node == rhs._node;
}

bool operator!=(const DendogramIterator & lhs, const DendogramIterator & rhs) {
    return lhs._node != rhs._node;
}

DendogramNode::DendogramNode(
    std::unique_ptr<DendogramNode> && left_child,
    std::unique_ptr<DendogramNode> && right_child
) :
    _left( std::move(left_child) ),
    _right( std::move(right_child) ),
    _parent( nullptr ),
    _data( nullptr )
{
    _left->_parent = _right->_parent = this;
}

DendogramNode::DendogramNode( const DataEntry * data ) :
    _parent( nullptr ),
    _data( data )
{} // unique_ptr's default constructor is to be a nullptr

bool DendogramNode::structural() const {
    return _data == nullptr;
}

bool DendogramNode::leaf() const {
    return _data != nullptr;
}

const DendogramNode & DendogramNode::left() const {
    return *_left;
}

const DendogramNode & DendogramNode::right() const {
    return *_right;
}

const DendogramNode * DendogramNode::parent() const {
    return _parent;
}

const DataEntry & DendogramNode::data() const {
    return *_data;
}

DendogramIterator DendogramNode::begin() const {
    if( leaf() ) {
        DendogramIterator it;
        it._node = this;
        return it;
    }
    return left().begin();
}

DendogramIterator DendogramNode::end() const {
    DendogramIterator it;
    it._node = nullptr;
    return it;
}
