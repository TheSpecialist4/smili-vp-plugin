#ifndef NODETYPE_H
#define NODETYPE_H


enum class NodeType
{
    FOR_NODE,
    IF_ELSE_NODE,
    VALUE_NODE,
    IMAGE_OP_NODE,
    MODEL_OP_NODE,
    VAR_NODE,
    START_NODE,
    END_NODE,
    PYTHON_NODE
};

#endif // NODETYPE_H
