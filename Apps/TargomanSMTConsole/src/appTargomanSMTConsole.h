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
 */

#ifndef TARGOMAN_APPS_APPTARGOMANSMTCONSOLE_H
#define TARGOMAN_APPS_APPTARGOMANSMTCONSOLE_H

#include "libTargomanCommon/clsSafeCoreApplication.h"

namespace Targoman {
namespace Apps{

class appTargomanSMTConsole : public QObject
{
    Q_OBJECT
public:
    appTargomanSMTConsole() {}

public slots:
    void slotExecute();

private:
};

}
}

#endif // TARGOMAN_APPS_APPTARGOMANSMTCONSOLE_H
