/******************************************************************************
 * Targoman: A robust Statistical Machine Translation framework               *
 *                                                                            *
 * Copyright 2014-2015 by ITRC <http://itrc.ac.ir>                            *
 *                                                                            *
 * This file is part of Targoman.                                             *
 *                                                                            *
 * Targoman is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * Targoman is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with Targoman. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                            *
 ******************************************************************************/
/**
 * @author S. Mohammad M. Ziabary <ziabary@targoman.com>
 * @author Behrooz Vedadian <vedadian@targoman.com>
 * @author Saeed Torabzadeh <saeed.torabzadeh@targoman.com>
 */

#ifndef TARGOMAN_CORE_CLSTRANSLATOR_H
#define TARGOMAN_CORE_CLSTRANSLATOR_H

#include <QMap>
#include <QStringList>
#include <QSettings>
#include "libTargomanSMT/Types.h"

namespace Targoman{
namespace SMT {

class Translator
{
public:
    static void init(QSharedPointer<QSettings> _configSettings);
    static void saveBinaryRuleTable(const QString& _filePath);
    static stuTranslationOutput translate(const QString& _inputStr,
                                          bool _justTranslationString = false,
                                          bool _getNBest = false,
                                          bool _isIXML = false);
    static void printNBestPath(const QString& _inputStr, int _sentenceNum, bool _isIXML = false);
    static void showWeights();
};

}
}



#endif // TARGOMAN_CORE_CLSTRANSLATOR_H
