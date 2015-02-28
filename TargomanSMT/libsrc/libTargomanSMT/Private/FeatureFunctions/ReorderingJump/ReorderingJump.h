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

#ifndef TARGOMAN_CORE_PRIVATE_FEATUREFUNCTIONS_REORDERINGJUMP_H
#define TARGOMAN_CORE_PRIVATE_FEATUREFUNCTIONS_REORDERINGJUMP_H

#include "Private/FeatureFunctions/intfFeatureFunction.hpp"

namespace Targoman {
namespace SMT {
namespace Private {
namespace FeatureFunction{

TARGOMAN_ADD_EXCEPTION_HANDLER(exReorderingJump, exFeatureFunction);


class ReorderingJump : public intfFeatureFunction
{
public:
    ~ReorderingJump(){}

    void initialize(const QString &){}

    Common::Cost_t scoreSearchGraphNode(SearchGraph::clsSearchGraphNode& _newHypothesisNode) const;

    Common::Cost_t getRestCostForPosition(const Coverage_t& _coverage, size_t _beginPos, size_t endPos) const;

    inline Common::Cost_t getApproximateCost(unsigned _sourceStart,
                                             unsigned _sourceEnd,
                                             const RuleTable::clsTargetRule& _targetRule) const {
        Q_UNUSED(_sourceStart)
        Q_UNUSED(_sourceEnd)
        Q_UNUSED(_targetRule)
        return 0;
    }

    inline QStringList columnNames() const{return QStringList();}

    void initRootNode(SearchGraph::clsSearchGraphNode &_rootNode);

private:
    ReorderingJump():
        intfFeatureFunction(this->moduleName(), true)
    {}

    TARGOMAN_DEFINE_SINGLETONMODULE("FeatureFunctions/ReorderingJump", ReorderingJump)

private:
    static Common::Cost_t getJumpCost(size_t _jumpWidth) {
        Common::Cost_t Cost = _jumpWidth;

        if (_jumpWidth > ReorderingJump::MaximumJumpWidth.value())
            Cost = _jumpWidth + (_jumpWidth * _jumpWidth);

        return Cost;
    }

private:
    static Common::Configuration::tmplConfigurable<double> ScalingFactor;
    static Common::Configuration::tmplConfigurable<quint8> MaximumJumpWidth;

};

}
}
}
}


#endif // TARGOMAN_CORE_PRIVATE_FEATUREFUNCTIONS_REORDERINGJUMP_H