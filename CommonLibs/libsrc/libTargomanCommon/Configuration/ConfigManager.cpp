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

#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <iostream>
#include "ConfigManager.h"
#include "Private/clsConfigManager_p.h"

namespace Targoman {
namespace Common {
namespace Configuration {

ConfigManager* ConfigManager::Instance = NULL;

ConfigManager::ConfigManager() :
    pPrivate(new Private::clsConfigManagerPrivate)
{
    this->pPrivate->Initialized = false;
}

ConfigManager::~ConfigManager()
{
    //Defined just to suppress compiler error on QScoppedPointer
}

/**
 * @brief Initializes configuration managment.
 *
 * First, checks arguments for configFile path or set default configFile path. Second, checks configFile and loads all module names and sets configurable values (calls setFromvariant function of that configurable) after validating them.
 * Then, checks arguments and overrides settings. In the next step, it cross validates all config items, then calls finalizeItem for each config.
 * Finally, initializes singleton modules.
 * @note: content of Configs Map of #pPrivate is inserted in constructor methods of intfConfigurable.
 * @note: Each module has a static member of clsModuleRegistrar instance which will be constructed before main function. using that constructor that module insert itself in ModuleInstantiators Map of #pPrivate.
 *
 * @param _arguments list of arguments in QStringList format.
 * @exception throws exception if multiple configuration file defined
 * @exception throws exception if config option has no file name
 * @exception throws exception if config path not found or can not be read
 * @exception throws exception if a configurable in config file is not registered
 * @exception throws exception if a configurable could not be validated
 * @exception throws exception if not enough parameters is provided for a switch
 * @exception throws exception if invalid or unrecognized arguments are provided for switch
 * @exception throws exception if every configurable could not be cross validated
 */

void ConfigManager::init(const QString& _license, const QStringList &_arguments)
{
    QString ErrorMessage;

    if (_arguments.indexOf(QRegExp("-h|--help")) >= 0){
        this->pPrivate->printHelp(_license);
        QCoreApplication::exit(1);
        exit(1);
        return;
    }
    // ////////////////////////////////////////////////
    // /check arguments to see wheter we must save file or not
    // ////////////////////////////////////////////////
    bool SaveFile = false;
    bool FirstTimeConfigFile = false;
    if (_arguments.count("--save"))
        SaveFile = true;
    
    // ////////////////////////////////////////////////
    // /check arguments for configFile path or set default cogFileplFile path
    // ////////////////////////////////////////////////
    if (_arguments.count("-c") + _arguments.count("--config") > 1)
        throw exConfiguration("Invalid multiple ConfigManager file definition");

    int Index = _arguments.indexOf(QRegExp("-c|--config"));
    if (Index>=0){
        if (_arguments.size() <= Index + 1)
            throw exConfiguration("Invalid config option with no file name");
        this->pPrivate->ConfigFilePath = _arguments.at(Index + 1);
        if (QFileInfo(this->pPrivate->ConfigFilePath).isReadable() == false){
            if (SaveFile){
                TargomanWarn(5, "File: <"+this->pPrivate->ConfigFilePath+"> not found or can not be read. Creating new file.");
                this->save2File(this->pPrivate->ConfigFilePath, false);
                FirstTimeConfigFile = true;
            }else
               throw exConfiguration("File: <"+this->pPrivate->ConfigFilePath+"> not found or can not be read.");
        }
    }

    if (this->pPrivate->ConfigFilePath.isEmpty()){
        this->pPrivate->ConfigFilePath = QCoreApplication::applicationDirPath() + QCoreApplication::applicationName() + ".ini";
        if (QFileInfo(this->pPrivate->ConfigFilePath).isReadable() == false && SaveFile == false){
            TargomanWarn(1, "No ConfigFile can be found. It is absolutely recomended to write one. Use --save to create one");
            this->pPrivate->ConfigFilePath.clear();
        }
    }

    // ////////////////////////////////////////////////
    // /check configFile and load everything
    // ////////////////////////////////////////////////
    QStringList Modules;
    if (FirstTimeConfigFile == false &&
        this->pPrivate->ConfigFilePath.size()){
        QSettings ConfigFile(this->pPrivate->ConfigFilePath, QSettings::IniFormat);
        Modules = ConfigFile.childGroups();
        foreach (const QString& Key, ConfigFile.allKeys()){
            if (this->pPrivate->Configs.contains(Key) == false){
                QString BasePath = Key;
                bool Found = false;
                while(BasePath.contains("/")){
                    BasePath.truncate(BasePath.lastIndexOf("/"));
                    if (this->pPrivate->Configs.value(BasePath) &&
                            this->pPrivate->Configs.value(BasePath)->canBemanaged() == false){
                        Found = true;
                        break;
                    }
                }
                if (Found)
                    continue; // Continue to next key
                else
                    throw exConfiguration("Configuration path <"+Key+"> is not registered");
            }
            intfConfigurable* ConfigItem  = this->pPrivate->Configs[Key];
            QVariant Value = ConfigFile.value(Key);
            if (ConfigItem->validate(Value, ErrorMessage) == false)
                throw exConfiguration(ErrorMessage);
            else
                ConfigItem->setFromVariant(Value);
        }
    }

    // ////////////////////////////////////////////////
    // /check arguments and override settings
    // ////////////////////////////////////////////////
    for (auto KeyIter = _arguments.begin();
         KeyIter != _arguments.end();
         KeyIter++){
        if (*KeyIter == "-c" || *KeyIter == "--config"){
            KeyIter++;
            if (KeyIter != _arguments.end())
                continue;
        }else if(*KeyIter == "--save"){
            continue;
        }

        if (KeyIter->startsWith("-")){
            for (auto ConfigItemIter = this->pPrivate->Configs.begin();
                 ConfigItemIter != this->pPrivate->Configs.end();
                 ConfigItemIter++){
                if ((KeyIter->startsWith("--") && *KeyIter == "--" + ConfigItemIter.value()->longSwitch()) ||
                       *KeyIter  == "-" + ConfigItemIter.value()->shortSwitch()){
                    QString Value;
                    for (qint8 i=0; i<ConfigItemIter.value()->argCount(); i++){
                        KeyIter++;
                        if (KeyIter == _arguments.end())
                            throw exConfiguration("Switch: <" +*KeyIter+ "> needs at least: " +
                                                  QString::number(ConfigItemIter.value()->argCount())+ " arguments.");
                        Value += *KeyIter + " ";
                    }
                    if (ConfigItemIter.value()->validate(Value.trimmed(), ErrorMessage) == false)
                        throw exConfiguration(ErrorMessage);
                    else
                        ConfigItemIter.value()->setFromVariant(Value.trimmed());
                }else if (KeyIter->startsWith("--") && this->pPrivate->ModuleInstantiators.value(KeyIter->mid(2)).IsSingleton){
                    Modules.append(KeyIter->mid(2));
                }else{
                    throw exConfiguration("Unrecognized argument: " + *KeyIter);
                }
            }
        }else
            throw exConfiguration("invalid argument <"+*KeyIter+">");
    }

    //Do not move following to the end of this method as finalization needs an initialized ConfigManager
    this->pPrivate->Initialized = true;

    try{
        // ////////////////////////////////////////////////
        // /validate all config items
        // ////////////////////////////////////////////////
        foreach (intfConfigurable* ConfigItem, this->pPrivate->Configs.values()){
            if (ConfigItem->crossValidate(ErrorMessage) == false){
                throw exConfiguration(ErrorMessage);
            }
        }

        // ////////////////////////////////////////////////
        // /finalize all config items (for module configurables, this puts instantiator function of module to Instatiator member of that.)
        // ////////////////////////////////////////////////
        foreach (intfConfigurable* ConfigItem, this->pPrivate->Configs.values()){
            ConfigItem->finalizeConfig();
        }

        // ////////////////////////////////////////////////
        // /marshal all singletons
        // ////////////////////////////////////////////////
        foreach (const QString& Module, Modules){
            stuInstantiator Instantiator = this->pPrivate->ModuleInstantiators.value(Module);
            if (Instantiator.IsSingleton && Instantiator.fpMethod)
                Instantiator.fpMethod();
        }

        if (SaveFile)
            this->save2File(this->pPrivate->ConfigFilePath, FirstTimeConfigFile ? false : true);
    }catch(...){
        this->pPrivate->Initialized = false;
        throw;
    }
}


/**
 * @brief save current configuration to ini file (not implimented yet).
 * @param _fileName name of file to save.
 * @param _backup save backup option.
 */

void ConfigManager::save2File(const QString &_fileName, bool _backup)
{
    if (_backup){
        if (QFile::exists(_fileName))
            QFile::copy(_fileName, _fileName + ".back-" + QDateTime::currentDateTime().toString("dd-MM-yyyy_hh:mm:ss"));
    }
    QSettings ConfigFile(_fileName, QSettings::IniFormat);

    foreach (Configuration::intfConfigurable* Config, this->pPrivate->Configs.values())
        if (Config->canBemanaged())
            ConfigFile.setValue(Config->configPath(),Config->toVariant());
    ConfigFile.sync();
}

/**
 * @brief Registers new configurations
 * @param _path configuration path
 * @param _item configurable item
 * @exception throws exception if a path for a configurable already exists.
 */

void ConfigManager::addConfig(const QString _path, intfConfigurable *_item)
{
    QString Path = _path;
    if(Path.startsWith("/"))
        Path = Path.remove(0,1);

    if (this->pPrivate->Configs.contains(_path))
        throw exConfiguration("Duplicate path key: " + Path);
    this->pPrivate->Configs.insert(Path, _item);
}

/**
 * @brief Registers new modules.
 * @param _name         Name of module
 * @param _instantiator A structure to encapsulate module instantiator and whether it is singleton or not.
 * @exception throws exception if a name for a module already exists.
 */

void ConfigManager::addModuleInstantiaor(const QString _name, const stuInstantiator &_instantiator)
{
    if (this->pPrivate->Configs.contains(_name))
        throw exConfiguration("Duplicate Module Name: " + _name);
    this->pPrivate->ModuleInstantiators.insert(_name, _instantiator);
}

/**
 * @brief gives value of a configurable in QVariant format.
 * @param _path     path of configurable (key in config registery Map).
 * @param _default  default value if value could not be convert to variant.
 * @exception throws exception configuration class is not initialized yet.
 */

QVariant ConfigManager::getConfig(const QString &_path, const QVariant& _default) const
{
    if (this->pPrivate->Initialized == false)
        throw exConfiguration("Configuration is not initialized yet.");

    intfConfigurable* Item= this->pPrivate->Configs.value(_path);
    if (Item && Item->toVariant().isValid() == false)
        return Item->toVariant();
    else
        return _default;
}

/**
 * @brief           Sets _value of a configurable in Config data member of #pPrivate using its _path.
 * @param _path     Path of configurable (key of hash map in the Config data member of #pPrivate).
 * @param _value    Value of configurable. (value of hash map in the Config data member of #pPrivate).
 * @exception       Throws exception if #pPrivate is not initialized yet.
 */

void ConfigManager::setValue(const QString &_path, const QVariant &_value) const
{
    if (this->pPrivate->Initialized == false)
        throw exConfiguration("Configuration is not initialized yet.");

    intfConfigurable* Item= this->pPrivate->Configs.value(_path);
    if (Item)
        return Item->setFromVariant(_value);
}

/**
 * @brief gives instantiator function of a module.
 * @param _name     Name of module.
 * @exception throws exception if ConfigManager is not initialized yet.
 * @exception throws exception if input module is singleton (because singleton module can not be reinitialized).
 */

fpModuleInstantiator_t ConfigManager::getInstantiator(const QString &_name) const
{
    if (this->pPrivate->Initialized == false)
        throw exConfiguration("Configuration is not initialized yet.");

    stuInstantiator Instantiator = this->pPrivate->ModuleInstantiators.value(_name);
    if (Instantiator.IsSingleton)
        throw exConfiguration(_name + " Is a singleton module and can not be reinstantiated");

    return Instantiator.fpMethod;
}

QString ConfigManager::configFilePath()
{
    return this->pPrivate->ConfigFilePath;
}

/***********************************************************************************************/
void Private::clsConfigManagerPrivate::printHelp(const QString& _license)
{
    std::cout<<_license.toUtf8().constData()<<std::endl;
    std::cout<<"Usage:"<<std::endl;
    std::cout<<"\t-h|--help:\t Print this help"<<std::endl;
    QStringList Keys = this->Configs.keys();
    Keys.sort();
    foreach(const QString& Key, Keys){
        intfConfigurable* Item = this->Configs.value(Key);
        if (Item && (Item->shortSwitch().size() || Item->longSwitch().size())){
            std::cout<<"\t";
            if(Item->shortSwitch().size())
                std::cout<<("-" + Item->shortSwitch()).toUtf8().constData();
            if (Item->longSwitch().size()){
                if (Item->shortSwitch().size())
                    std::cout<<"|";
                std::cout<<Item->longSwitch().toUtf8().constData();
            }
            if (Item->shortHelp().size())
                std::cout<<Item->shortHelp().toUtf8().constData()<<std::endl;
            std::cout<<"\t\t\t"<<Item->description().toUtf8().constData()<<std::endl;
        }
    }
}

/***********************************************************************************************/

namespace Private {
class intfConfigurablePrivate{
public:
    /**
     * @brief  updateConfig Finds location which _old configurable pointer is located in Configs hash map and changes its value with _new configurable pointer.
     * @param _old          old configurable pointer.
     * @param _new          new configurable pointer.
     */
    void updateConfig(const intfConfigurable* _old, intfConfigurable* _new){
        ConfigManager::instance().pPrivate->Configs[ConfigManager::instance().pPrivate->Configs.key((intfConfigurable*)_old)] = _new;
    }
};
}

/***********************************************************************************************/
/**
 * @brief constructor of intfConfigurable. this is where each configurable inserts itself to Configs hash map of pPrivate member of ConfigManager class.
 */
intfConfigurable::intfConfigurable(const QString &_configPath,
                                   const QString &_description,
                                   const QString &_shortSwitch,
                                   const QString &_shortHelp,
                                   const QString &_longSwitch) :
    pPrivate(new Private::intfConfigurablePrivate)
{
    try{
        this->Description = _description;
        this->ShortSwitch = _shortSwitch;
        this->LongSwitch = _longSwitch;
        this->ShortHelp = _shortHelp;
        this->ConfigPath = _configPath;
        this->ArgCount = this->ShortHelp.split(" ").size();

        ConfigManager::instance().addConfig(_configPath, this);
    }catch(exTargomanBase &e){
        TargomanError(e.what());
        throw;
    }
}

/**
 * @brief intfConfigurable::intfConfigurable Copy constructor of intfConfigurable class.
 *
 * This function is defined to call updateConfig function of #pPrivate.
 * Usage of this function is when we have QList of Configurables and we want to insert a new configurable to it.
 * When a temporary configurable is instantiated to be inserted into QList, its pointer will be added into Configs hash map, but that pointer will be deleted soon because it is temporary.
 * This copy constructor will be called when the temporary configurable is inserted into QList.
 * Calling updateConfig in this copy constructor, helps us to change the deleted pointer with the valid pointer of configurable which is in the QList.
 *
 */

intfConfigurable::intfConfigurable(const intfConfigurable &_other):
    pPrivate(new Private::intfConfigurablePrivate)
{
    this->Description = _other.Description;
    this->ShortSwitch = _other.ShortSwitch;
    this->LongSwitch = _other.LongSwitch;
    this->ShortHelp = _other.ShortHelp;
    this->ConfigPath = _other.ConfigPath;

    //To replace pointer of old registered config with the new copied config
    this->pPrivate->updateConfig(&_other, this);
}

intfConfigurable::~intfConfigurable()
{
    //Just to suppress compiler error on QSopedPointer
}

/***********************************************************************************************/
/**
 * @brief constructor of clsModuleRegistrar. This is where each module inserts its instantiator to ModuleInstantiators Map of pPrivate member of ConfigManager.
 */
clsModuleRegistrar::clsModuleRegistrar(const QString &_name, stuInstantiator _instantiatior){
    ConfigManager::instance().addModuleInstantiaor(_name, _instantiatior);
}

}
}
}
