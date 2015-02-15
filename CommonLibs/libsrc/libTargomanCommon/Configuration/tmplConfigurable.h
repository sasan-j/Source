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
 */

#ifndef TARGOMAN_COMMON_CONFIGURATION_TMPLCONFIGURABLE_HPP
#define TARGOMAN_COMMON_CONFIGURATION_TMPLCONFIGURABLE_HPP

#include <functional>
#include "libTargomanCommon/Configuration/intfConfigurable.hpp"
#include "libTargomanCommon/Configuration/ConfigManager.h"
#include "libTargomanCommon/Configuration/intfCrossValidate.hpp"
#include "libTargomanCommon/Macros.h"

namespace Targoman {
namespace Common {
namespace Configuration {

static std::function<bool(const intfConfigurable& _item,
                          QString& _errorMessage)> ReturnTrueCrossValidator = [] (const intfConfigurable&, QString& ) {
    return true;};

/**
 * @brief The clsConfigurable template is used to store and validate different configurable items
 */
template <class Type_t, bool ... _rest> class tmplConfigurable : public intfConfigurable
{
public:
    tmplConfigurable(const QString&  _configPath,
                    const QString&  _description,
                    const QVariant& _default = QVariant(),
                    const std::function< bool(const intfConfigurable& _item,
                                              QString& _errorMessage) >& _crossValidator = ReturnTrueCrossValidator,
                    const QString&  _shortSwitch = "",
                    const QString&  _shortHelp = "",
                    const QString&  _LongSwitch = ""):
        intfConfigurable(_configPath,
                                _description,
                                _shortSwitch,
                                _shortHelp,
                                _LongSwitch){
        try{
            QString ErrorMessage;
            if (!this->validate(_default, ErrorMessage)){
                throw exTargomanInitialization("Invalid default value for: " + _configPath + ": " + ErrorMessage);
            }
            this->setFromVariant(_default);
            this->CrossValidator = _crossValidator;
        }catch(exTargomanBase &e){
            TargomanError(e.what());
            throw;
        }
    }

    tmplConfigurable(tmplConfigurable<Type_t>&& _other) :
        intfConfigurable(_other.ConfigPath,
                         _other.Description,
                         _other.ShortSwitch,
                         _other.ShortHelp,
                         _other.LongSwitch){
        this->Value = _other.Value;
        this->CrossValidator = _other.CrossValidator;
    }

    /**
     * @brief This function will be overloaded for every type (such as int, float ,...).
     * This function converts input value from QVariant to a specific type based on overloaded implementation.
     */
    virtual inline void setFromVariant(const QVariant& _value){
        throw exTargomanMustBeImplemented("setFromVariant for "+this->ConfigPath+" Not Implemented");
    }
    /**
     * @brief convert #value variable to QVariant type.
     */
    virtual inline QVariant    toVariant() const{
        return QVariant(this->Value);
    }
    /**
     * @brief This function will be overloaded for every type (such as int, float ,...)
     * This function validate value of input value.
     */
    virtual inline bool        validate(const QVariant& _value, QString& _errorMessage) const{
        return true;
    }
    /**
     * @brief if cross validator function is prepared that function will be called, else returns true.
     */
    virtual inline bool        crossValidate(QString& _errorMessage) const{
        return this->CrossValidator(*this, _errorMessage);
    }

    inline Type_t  value(){return this->Value;}
    inline Type_t  operator ()(){return this->Value;}

private:
    Type_t  Value;
    //QScopedPointer<intfCrossValidate> CrossValidator;
    std::function<bool(const intfConfigurable& _item, QString& _errorMessage)> CrossValidator;
};


/***************************************************************************************/
/**
 * @def _SPECIAL_CONFIGURABLE type specific validate and setFromVariant functions signiture.
 */
#define _SPECIAL_CONFIGURABLE(_type) \
    template <> bool Targoman::Common::Configuration::tmplConfigurable<_type>::validate(const QVariant& _value, QString& _errorMessage) const ;\
    template <> void Targoman::Common::Configuration::tmplConfigurable<_type>::setFromVariant(const QVariant& _value);

/***************************************************************************************/
_SPECIAL_CONFIGURABLE(qint8)
_SPECIAL_CONFIGURABLE(qint16)
_SPECIAL_CONFIGURABLE(qint32)
_SPECIAL_CONFIGURABLE(qint64)
_SPECIAL_CONFIGURABLE(quint8)
_SPECIAL_CONFIGURABLE(quint16)
_SPECIAL_CONFIGURABLE(quint32)
_SPECIAL_CONFIGURABLE(quint64)
_SPECIAL_CONFIGURABLE(double)
_SPECIAL_CONFIGURABLE(float)
_SPECIAL_CONFIGURABLE(QString)
_SPECIAL_CONFIGURABLE(bool)
_SPECIAL_CONFIGURABLE(QRegExp MACRO_SAFE_COMMA false) /* Used on normal regex matching */
_SPECIAL_CONFIGURABLE(QRegExp MACRO_SAFE_COMMA true) /* Used on wildcard matching */

}
}
}
#endif // TARGOMAN_COMMON_CONFIGURATION_TMPLCONFIGURABLE_HPP