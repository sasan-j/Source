/*************************************************************************
 * Copyright © 2012-2015, Targoman.com
 *
 * Published under the terms of TCRL(Targoman Community Research License)
 * You can find a copy of the license file with distributed source or
 * download it from http://targoman.com/License.txt
 *
 *************************************************************************/
/**
 @author S. Mohammad M. Ziabary <smm@ziabary.com>
 @author Behrooz Vedadian <vedadian@gmail.com>
 */

#include "clsSearchGraphNode.h"
#include "Private/FeatureFunctions/intfFeatureFunction.hpp"

// Vedadian
//#include <QDebug>

namespace Targoman{
namespace SMT {
namespace Private{
/**
 * @brief Namespace surrounding all classes related to generate, store and manage SearchGraph
 */
namespace SearchGraph {

using namespace RuleTable;
using namespace Common;

clsSearchGraphNodeData* InvalidSearchGraphNodeData = NULL;
clsSearchGraphNode* pInvalidSearchGraphNode = NULL;
size_t  clsSearchGraphNodeData::RegisteredFeatureFunctionCount;

/**
 * @brief constructor of this class set invalid data to #Data in order to know whether it is used before or not.
 * This funciton also calls initRootNode of every feature function to always have a valid previous node data.
 */
clsSearchGraphNode::clsSearchGraphNode():
    Data(InvalidSearchGraphNodeData)
{
    foreach (FeatureFunction::intfFeatureFunction* FF, gConfigs.ActiveFeatureFunctions)
        FF->initRootNode(*this);
}

// Vedadian
/*
QString getPartialTranslation(const clsSearchGraphNode& _node)
{
    if(_node.isInvalid())
        return QString();
    return getPartialTranslation(_node.prevNode()) + " " + _node.targetRule().toStr();
}
//*/
/**
 * @brief This constructor of this class set values of #Data and calculates cost of this translation up to now using all feature functions.
 * @param _prevNode         Previous node of this search graph node.
 * @param _startPos         This node has a translation for a phrase of input sentence, start position of this phrase in input sentence is this variable.
 * @param _endPos           This node has a translation for a phrase of input sentence, end position of this phrase in input sentence is this variable.
 * @param _newCoverage      Updated covered for that is translated is stored in this variable.
 * @param _targetRule       Target language phrase translation.
 * @param _isFinal          Has this node covered translation for all word of input sentence.
 * @param _restCost         Approximated cost of rest of translation.
 */
clsSearchGraphNode::clsSearchGraphNode(const clsSearchGraphNode &_prevNode,
                                       quint16 _startPos,
                                       quint16 _endPos,
                                       const Coverage_t &_newCoverage,
                                       const clsTargetRule &_targetRule,
                                       bool _isFinal,
                                       Cost_t _restCost):
    Data(new clsSearchGraphNodeData(
             _prevNode,
             _startPos,
             _endPos,
             _newCoverage,
             _targetRule,
             _isFinal,
             _restCost))
{
    // Vedadian
    /*
    qDebug() << "PrevNode: " << getPartialTranslation(this->prevNode());
    qDebug() << "NewNode: " << getPartialTranslation(*this);
    int a = 2;
    ++a;
    //*/
    foreach (FeatureFunction::intfFeatureFunction* FF, gConfigs.ActiveFeatureFunctions) {
        Cost_t Cost = FF->scoreSearchGraphNode(*this);
        //qDebug() << "Cost: " << Cost;
        this->Data->Cost += Cost;
    }
}

template<class Class_t, class Container_t, typename Functor_t>
/**
 * @brief findInsertionPos finds correct place to insert _element in the _container using _comperator to have a sorted list.
 * @return Returns correct place of insertion.
 */
size_t findInsertionPos(const Container_t& _conatiner, const Class_t& _element, Functor_t _comparator){
    for(size_t i = 0; i< _conatiner.size(); ++i){
        if (_comparator(_element, _conatiner.at(i)) > 0)
            return i;
    }
    return _conatiner.size();
}

/**
 * @brief Recombines input node with this node and .
 * @param _node better node for translation that should be replaced with this node.
 */
void clsSearchGraphNode::recombine(clsSearchGraphNode &_node)
{
    Q_ASSERT(this->Data->IsRecombined == false && _node.Data->IsRecombined == false);
    //swaps _node with this instance of node
    if (_node.getTotalCost() < this->getTotalCost()){
        this->Data.swap(_node.Data);
    }
    //now, this node is better node and _node is the worse node.
    auto NodeComparator = [](const clsSearchGraphNode& _firstNode,const clsSearchGraphNode& _secondNode){
        return _secondNode.getTotalCost() - _firstNode.getTotalCost();
    };

    //inserts worse node to CombinedNodes container of this node.
    this->Data->CombinedNodes.insert(
                findInsertionPos(this->Data->CombinedNodes, _node, NodeComparator),
                _node);
    // Now we want to add recombined nodes of worse node to better node.
    // We do not need to find correct places of insertion for recombined nodes because better node is a new node
    // and dosn't have any combined node so far. we just add recombined node of worse node to this new node.

    ///@todo CombinedNodes QList can be swapped instead if appended and cleared because one of the QLists is surely empty
    this->Data->CombinedNodes.append(_node.Data->CombinedNodes);
    _node.Data->CombinedNodes.clear();
    _node.Data->IsRecombined = true;
}

/**
 * @brief determines whether this node and input _node expands same future nodes or not.
 * @param _node input node.
 */
bool clsSearchGraphNode::haveSameFuture(const clsSearchGraphNode &_node) const
{
    if (this->sourceRangeEnd() != _node.sourceRangeEnd())
        return false;

    foreach(FeatureFunction::intfFeatureFunction* FF, gConfigs.ActiveFeatureFunctions.values())
        if (FF->nodesHaveSameState(*this, _node) == false)
            return false;

    if (Q_UNLIKELY(_node.coverage() != this->coverage()))
        return false;

    return true;
}

/**
 * @brief allocates an index for feature functions.
 * @return Returns allocated index.
 */
size_t clsSearchGraphNode::allocateFeatureFunctionData()
{
    size_t AllocatedIndex = clsSearchGraphNodeData::RegisteredFeatureFunctionCount;
    ++clsSearchGraphNodeData::RegisteredFeatureFunctionCount;
    return AllocatedIndex;

}


}
}
}
}
