#include <algorithm>
#include <utility>
#include "dendogram_node.h"
#include "pr/data_entry.h"

DendogramIterator::reference DendogramIterator::operator*() const {
    return _node->data();
}

DendogramIterator::pointer DendogramIterator::operator->() const {
    return &**this;
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
    std::unique_ptr<DendogramNode> && right_child,
    double linkage_distance
) :
    _left( std::move(left_child) ),
    _right( std::move(right_child) ),
    _parent( nullptr ),
    _data( nullptr ),
    _linkage_distance( linkage_distance ),
    _size( _left->size() + _right->size() ),
    _depth( std::max( _left->depth(), _right->depth() ) + 1 )
{
    _left->_parent = _right->_parent = this;
}

DendogramNode::DendogramNode( const DataEntry * data ) :
    _parent( nullptr ),
    _data( data ),
    _linkage_distance( 0.0 ),
    _size( 1 ),
    _depth( 0 )
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
    if( _parent == nullptr ) {
        it._node = nullptr;
        return it;
    }
    /* If we have a parent,
     * end() is not a nullptr, but begin() of the next node.
     *
     * We will simply descend recursively to the rightmost leaf,
     * increment the iterator, and return.
     */
    if( !leaf() )
        return right().end();

    it._node = this;
    ++it;
    return it;
}

double DendogramNode::linkage_distance() const {
    return _linkage_distance;
}

unsigned DendogramNode::size() const {
    return _size;
}

unsigned DendogramNode::depth() const {
    return _depth;
}
