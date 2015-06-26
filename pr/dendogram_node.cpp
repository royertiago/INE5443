#include <utility>
#include "dendogram_node.h"
#include "pr/data_entry.h"

DendogramNode::DendogramNode(
    std::unique_ptr<DendogramNode> && left_child,
    std::unique_ptr<DendogramNode> && right_child
) :
    _left( std::move(left_child) ),
    _right( std::move(right_child) ),
    _data( nullptr )
{}

DendogramNode::DendogramNode( DataEntry * data ) :
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

const DataEntry & DendogramNode::data() const {
    return *_data;
}
