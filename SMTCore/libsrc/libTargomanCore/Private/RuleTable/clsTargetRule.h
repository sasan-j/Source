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

#ifndef TARGOMAN_CORE_PRIVATE_RULETABLE_CLSTARGETRULE_H
#define TARGOMAN_CORE_PRIVATE_RULETABLE_CLSTARGETRULE_H

#include <QList>
#include "libTargomanCommon/Types.h"
#include "libTargomanCore/clsTranslator.h"
#ifdef TARGOMAN_SHOW_DEBUG
#include <QTextStream>
#endif

namespace Targoman {
namespace Core {
namespace Private {
namespace RuleTable{

TARGOMAN_ADD_EXCEPTION_HANDLER(exRuleTable, exTargomanCore);

class clsTargetRuleData;
extern clsTargetRuleData* InvalidTargetRuleData;

/**
 * @brief The clsTargetRule class is used to store translation and feature values for target language phrase.
 *
 * Each Node of prefix tree is a source phrase, each node has a list of target phrases which are of type of this class.
 *
 * @note: combination of ruleNode and TargetRule can be imagine as a primary key for a table.
 * This table has some fields. Every field is a feature value. These feature values are stored in #Feilds data member of clsTargetRuleData class.
 * Each or a group of feature values are used in Feature Function, for example some of them is used in lexical reodering and some of them is used in phrase table.
 *
 */

class clsTargetRule
{
public:
    clsTargetRule();

    clsTargetRule(const QList<Common::WordIndex_t>& _targetPhrase,
                  const QList<Common::Cost_t>& _fields);

    /**
     * @brief clsTargetRule copy constructor.
     */
    clsTargetRule(const clsTargetRule& _other):
        Data(_other.Data)
    {}
    ~clsTargetRule(){}

    inline bool isInvalid() const;
    inline bool isSame(const clsTargetRule& _other) const;

    inline Common::WordIndex_t at(int _index) const;
    inline size_t size() const;
#ifdef TARGOMAN_SHOW_DEBUG
    inline size_t fieldCount() const;
#endif
    inline Common::Cost_t  field(size_t _index) const;
    inline Common::Cost_t precomputedValue(size_t _index) const;
    inline void setCosts(const QList<Common::Cost_t>& _costs);
    inline void setPrecomputedValue(size_t _index, Common::Cost_t _value);
    /**
     * @brief getColumnIndex    returns index of column by its name.
     * @param[in] _columnName   name of column.
     * @param[in] _moduleName   name of module used for exception string.
     * @return                  returns index of column by its name.
     * @exception               throws exception if input _columnName is not in list of ColumnNames.
     */

    static size_t getColumnIndex(const QString& _columnName, const QString& _moduleName){
        if (clsTargetRule::ColumnNames.contains(_columnName) == false)
            throw exRuleTable("Invalid filed name <"+_columnName+
                              "> not declared in Rule Table. Needed by:" + _moduleName);
        return clsTargetRule::ColumnNames.indexOf(_columnName);
    }

    /**
     * @brief setColumnNames    Sets names of all fields of table.
     * @param _names            input list of names.
     * @exception               throws exception if any field of the table has already a name.
     */

    static void setColumnNames(const QStringList& _names){
        if (clsTargetRule::ColumnNames.size())
            throw exRuleTable("Invalid reinitialization of rule table column names");
        clsTargetRule::ColumnNames = _names;
    }

    /**
     * @return returns #PrecomputedValuesSize and increases its size.
     */

    static size_t allocatePrecomputedValue(){
        size_t AllocatedIndex = clsTargetRule::PrecomputedValuesSize;
        ++clsTargetRule::PrecomputedValuesSize;
        return AllocatedIndex;

    }

    void detachInvalidData(){
        Q_ASSERT(this->isInvalid());
        this->Data.detach();
    }

#ifdef TARGOMAN_SHOW_DEBUG
    QString toStr() const;
#endif

//private:
public:
    QExplicitlySharedDataPointer<clsTargetRuleData>     Data;                   /**< Data member of this class is stored in a seperate class. A shared pointer of this seperate class is stored here */
    static  QStringList                                 ColumnNames;            /** A List of Names of Fields of feature values */
    static  size_t                                      PrecomputedValuesSize;

    friend class clsTargetRuleData;
};

/**
 * This instance of InvalidTargetRule class is static and is used in initilization of clsRuleNode class in constructor of clsSearchGraphNodeData class.
 */
extern clsTargetRule* InvalidTargetRule;
/********************************************************************************/
class clsTargetRuleData : public QSharedData
{
public:
    clsTargetRuleData(const QList<Common::WordIndex_t>& _targetPhrase,
                      const QList<Common::Cost_t>& _fields,
                      size_t _precomputedValueSize):
        TargetPhrase(_targetPhrase),
        Fields(_fields),
        PrecomputedValues(_precomputedValueSize,-INFINITY)
    {}

    /**
     * @brief clsTargetRuleData: A constructor to make invalid target rule data.
     * @exception                Throws exception if size of columnNames list is zero ( which it means clsTargetRule is not initialized).
     */

    clsTargetRuleData(){
        if (clsTargetRule::ColumnNames.size() == 0)
            throw exRuleTable("Invalid TargetRule must be called after initialization");
        for(int i=0; i< clsTargetRule::ColumnNames.size(); ++i)
            this->Fields.append(0);
    }

    /**
     * @brief clsTargetRuleData copy constructor
     */

    clsTargetRuleData(const clsTargetRuleData& _other):
        QSharedData(_other),
        TargetPhrase(_other.TargetPhrase),
        Fields(_other.Fields),
        PrecomputedValues(_other.PrecomputedValues)
    {}
    ~clsTargetRuleData() {}

public:
    QList<Common::WordIndex_t> TargetPhrase;            /**< Translation (target language phrase) */
    QList<Common::Cost_t>      Fields;                  /**< Feature values */
    QVector<Common::Cost_t>    PrecomputedValues;       /**< It is used for caching */
};

/***********************************************************/

/**
 * @param _index            Index of target language phrase letter.
 * @return                  Returns wordIndex of _index-th letter of target phrase
 */

inline Common::WordIndex_t clsTargetRule::at(int _index) const{
    return this->Data->TargetPhrase.at(_index);
}

/**
 * @return  Returns size of target language phrase letters.
 */

inline size_t clsTargetRule::size() const {
    return this->Data->TargetPhrase.size();
}



#ifdef TARGOMAN_SHOW_DEBUG
inline size_t clsTargetRule::fieldCount() const {
    return (size_t)this->Data->Fields.size();
}
#endif

/**
 * @param[in] _index    Index of field.
 * @return              Returns feature value for field number _index.
 */
inline Common::Cost_t  clsTargetRule::field(size_t _index) const{
    Q_ASSERT(_index < (size_t)this->Data->Fields.size());
    return this->Data->Fields.at(_index);
}

/**
 * @param[in] _index    Index of field.
 * @return              Returns _index-th precomputedValue value.
 */

inline Common::Cost_t clsTargetRule::precomputedValue(size_t _index) const{
    Q_ASSERT(_index < (size_t)this->Data->PrecomputedValues.size());
    return this->Data->PrecomputedValues.at(_index);
}

/**
 * @brief clsTargetRule::setCosts   sets value of features (Fields of table).
 * @param[in] _costs                A list of costs
 */

inline void clsTargetRule::setCosts(const QList<Common::Cost_t>& _costs){
    Q_ASSERT(this->Data->Fields.isEmpty());
    Q_ASSERT(clsTargetRule::ColumnNames.size() == _costs.size());

    this->Data->Fields = _costs;
}

/**
 * @brief clsTargetRule::setPrecomputedValue   Sets _value to _index-th index of precomputedValue list.
 * @param _index                               Index of PrecomputedValue.
 * @param _value                               value for PrecomputedValue.
 */

inline void clsTargetRule::setPrecomputedValue(size_t _index, Common::Cost_t _value){
    Q_ASSERT(_index < (size_t)this->Data->PrecomputedValues.size());
    this->Data->PrecomputedValues[_index] = _value;
}

inline bool clsTargetRule::isInvalid() const{
    return this->Data.data() == InvalidTargetRuleData;
}
inline bool clsTargetRule::isSame(const clsTargetRule& _other) const{
    return this->Data == _other.Data;
}

typedef  QList<RuleTable::clsTargetRule> TargetRulesContainer_t;

}
}
}
}

#ifdef TARGOMAN_SHOW_DEBUG
QTextStream& operator << (QTextStream& _outputStream, const Targoman::Core::Private::RuleTable::clsTargetRule& _targetRule);
#endif

#endif // TARGOMAN_CORE_PRIVATE_RULETABLE_CLSTARGETRULE_H
