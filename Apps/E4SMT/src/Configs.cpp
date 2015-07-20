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

#include "Configs.h"

namespace Targoman {
namespace Apps {

using namespace Common;
using namespace Common::Configuration;

tmplConfigurable<enuAppMode::Type> gConfigs::Mode(
        gConfigs::appConfig("Mode"),
        "Working Mode of the application. Can be [" + enuAppMode::options().join("|")+"]",
        enuAppMode::Text2IXML,
        ReturnTrueCrossValidator,
        "m",
        "ApplicationMode",
        "mode",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<QString> gConfigs::Language(
        gConfigs::appConfig("Language"),
        "Language of source text in ISO639 format",
        "",
        ReturnTrueCrossValidator,
        "l",
        "LANGUAGE",
        "lang",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<QString> gConfigs::Input(
        gConfigs::appConfig("Input"),
        "One line input which can be plain text or IXML. All other Input/Output configs will be ignored",
        "",
        ReturnTrueCrossValidator,
        "i",
        "TEXT/IXML",
        "input",
        enuConfigSource::Arg);

tmplConfigurable<FilePath_t>     gConfigs::InputFile(
        gConfigs::appConfig("InputFile"),
        "Input file path to convert",
        "",
        Validators::tmplPathAccessValidator<
        (enuPathAccess::Type)(enuPathAccess::File | enuPathAccess::Readable),
        false>,
        "f",
        "FILEPATH",
        "input-file",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<FilePath_t>     gConfigs::InputDir(
        gConfigs::appConfig("InputDir"),
        "Input directory to convert",
        "",
        Validators::tmplPathAccessValidator<
        (enuPathAccess::Type)(enuPathAccess::Dir | enuPathAccess::Readable),
        false>,
        "d",
        "PATH",
        "input-dir",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<FilePath_t>     gConfigs::OutputDir(
        gConfigs::appConfig("Output"),
        "output path to write converted file/files. Defaults to standard output",
        "",
        Validators::tmplPathAccessValidator<
        (enuPathAccess::Type)(enuPathAccess::Dir | enuPathAccess::Writeatble),
        false>,
        "o",
        "PATH",
        "output",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<QRegExp,true>     gConfigs::IncludePattern(
        gConfigs::appConfig("IncludePattern"),
        "FilePatterns to include when converting (WildCard format)",
        "",
        ReturnTrueCrossValidator,
        "p",
        "PTRN",
        "pattern",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<bool>     gConfigs::Overwrite(
        gConfigs::appConfig("Overwrite"),
        "Overwrite existing files",
        false,
        ReturnTrueCrossValidator,
        "w",
        "",
        "overwrite",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<bool>     gConfigs::Interactive(
        gConfigs::appConfig("Interactive"),
        "Interactive normalization and spell correction",
        false,
        ReturnTrueCrossValidator,
        "",
        "",
        "interactive",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<bool>     gConfigs::NoSpellcorrector(
        gConfigs::appConfig("UseSpellcorrector"),
        "Do not use spell corrector",
        false,
        ReturnTrueCrossValidator,
        "",
        "",
        "no-spellcorrect",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<bool>     gConfigs::Recursive(
        gConfigs::appConfig("Recursive"),
        "Recursive directory processing",
        false,
        ReturnTrueCrossValidator,
        "r",
        "",
        "recursive",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<bool>     gConfigs::PlainText(
        gConfigs::appConfig("PlainText"),
        "Assume all files as plain text and ignore file suffix",
        false,
        ReturnTrueCrossValidator,
        "t",
        "",
        "plaintext",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<bool>     gConfigs::KeepTitles(
        gConfigs::appConfig("KeepTitles"),
        "On XML files keep titles as text",
        false,
        ReturnTrueCrossValidator,
        "k",
        "",
        "keeptitles",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

tmplConfigurable<FilePath_t>     gConfigs::DTDFilePath(
        gConfigs::appConfig("DTDFilePath"),
        "DTD file used to validate XML Files",
        "",
        Validators::tmplPathAccessValidator<
        (enuPathAccess::Type)(enuPathAccess::Dir | enuPathAccess::Writeatble),
        false>,
        "",
        "DTD_Path",
        "dtd",
        (enuConfigSource::Type)(
            enuConfigSource::Arg  |
            enuConfigSource::File));

}}
/**************************************************************************************************************/
ENUM_CONFIGURABLE_IMPL(Targoman::Apps::enuAppMode)