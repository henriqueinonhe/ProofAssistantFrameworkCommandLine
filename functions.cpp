#include "functions.h"
#include <iostream>
#include <QDir>
#include "storagemanager.h"
#include "commandlineparser.h"
#include "programmanager.h"
#include "logicalsystemrecord.h"
#include "theoryrecord.h"
#include "theorybuilder.h"
#include "pluginmanager.h"
#include "theorypluginsrecord.h"
#include "proofassistant.h"

ProgramManager manager;
CommandLineParser parser;

//Routines
void createRootDir()
{
    QDir dir;
    dir.cd("C:/Users/Henrique/Desktop/Logos CLI Data/");
    if(!dir.exists("filesystem"))
    {
        dir.mkdir("filesystem");

        dir.cd("filesystem");
        dir.mkdir("data");
        dir.mkdir("plugins");

        dir.cd("data");
        dir.mkdir("Logical Systems");

        dir.cd("Logical Systems");
        QFile(dir.absolutePath() + "/logicalsystemsrecords.dat").open(QIODevice::WriteOnly);

        dir.cdUp();
        dir.cdUp();
        dir.cd("plugins");
        dir.mkdir("Signatures");
        dir.mkdir("Inference Rules");
        dir.mkdir("Inference Tactics");
        dir.mkdir("Pre Processors");
        dir.mkdir("Post Processors");
        dir.cdUp();
    }

    const auto currentWorkingDir = dir.absolutePath();
    StorageManager::setRootPath(currentWorkingDir + "/filesystem");
}

void entryMenu()
{
    while(true)
    {
        try
        {
            cout << endl;
            cout << "Proof Assistant Framework" << endl
                 << "Commands:" << endl
                 << "   list           List logical systems" << endl
                 << "   new            Create logical system" << endl
                 << "   load           Load logical system" << endl
                 << "   delete         Delete logical system" << endl
                 << "   signatures     List signature plugins" << endl
                 << "   rules          List inference rules plugins" << endl
                 << "   proofs         List proof plugins" << endl
                 << "   quit           Quits" << endl << endl;

            string command;
            getline(cin, command);

            parser.parse(command);
            const auto mainCommand = parser.getMainCommand();
            const auto options = parser.getOptions();
            const auto positionalArgs = parser.getPositionalArgs();

            if(mainCommand == "list")
            {
                listLogicalSystems(options, positionalArgs);
            }
            else if(mainCommand == "new")
            {
                createLogicalSystem(options);
            }
            else if(mainCommand == "load")
            {
                loadLogicalSystem(options, positionalArgs);
                logicalSystemMenu();
            }
            else if(mainCommand == "delete")
            {
                deleteLogicalSystem(options, positionalArgs);
            }
            else if(mainCommand == "signatures")
            {
                listSignaturePlugins();
            }
            else if(mainCommand == "rules")
            {
                listInferenceRulePlugins();
            }
            else if(mainCommand == "proofs")
            {
                listProofPlugins();
            }
            else if(mainCommand == "quit")
            {
                return;
            }
            else
            {
                invalidCommand(mainCommand);
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }

    }
}

void invalidCommand(const QString &string)
{
    QString errorMsg;
    errorMsg += "\"";
    errorMsg += string;
    errorMsg += "\" is not a valid command.";
    throw invalid_argument(errorMsg.toStdString());
}

ostream &operator <<(ostream &stream, const QString &string)
{
    stream << string.toStdString();
    return stream;
}

ostream &operator <<(ostream &stream, const QStringList &list)
{
    for(const QString &string : list)
    {
        stream << string << endl;
    }
    return stream;
}

void listInferenceRulePlugins()
{
    // Standardize
    cout << endl << "Inferece Rule Plugins:" << endl;
    const auto list = StorageManager::inferenceRulesPluginsList();
    auto ruleCount = 1;
    for(const auto &rule : list)
    {
        cout << ruleCount << ". " << rule.mid(0, rule.size() - 4) << endl; // Refactor magic number
        ruleCount++;
    }
}

void listLogicalSystems(const QStringList &options, const QStringList &positionalArgs)
{
    const QStringList admissibleOptions({"h", "help", "d", "description"});
    checkOptionsAdmissibility(options, admissibleOptions);
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: list [<options>] [system-name]" << endl;
        cout << "   -d, --description           Shows list with descriptions" << endl;
        return;
    }

    auto records = StorageManager::retrieveLogicalSystemsRecords();
    if(records.isEmpty())
    {
        cout << endl;
        cout << "There are currently no logical systems." << endl;
        return;
    }

    if(!positionalArgs.isEmpty())
    {
        const auto &systemName = positionalArgs.first();
        records = filterRecords(systemName, records);
    }

    cout << endl;
    cout << "Logical Systems:" << endl;
    if(options.contains("d") || options.contains("description"))
    {
        auto logicalSystemNumber = 1;
        for(const auto &record : records)
        {
            cout << endl;
            cout << logicalSystemNumber << ":" << endl;
            cout << "Name: " << record.getName() << endl;
            cout << "Description: " << record.getDescription() << endl;
            logicalSystemNumber++;
        }
    }
    else
    {
        auto logicalSystemNumber = 1;
        for(const auto &record : records)
        {
            cout << endl;
            cout << logicalSystemNumber << ":" << endl;
            cout << "Name: " << record.getName() << endl;
            logicalSystemNumber++;
        }
    }
}

void checkOptionsAdmissibility(const QStringList &options, const QStringList &admissibleOptions)
{
    for(const auto &option : options)
    {
        auto isAdmissible = false;
        for(const auto &admissibleOption : admissibleOptions)
        {
            if(option == admissibleOption)
            {
                isAdmissible = true;
                break;
            }
        }
        if(!isAdmissible)
        {
            QString errorMsg;
            errorMsg += "\"";
            errorMsg += option;
            errorMsg += "\" is not an admissible option.";
            throw invalid_argument(errorMsg.toStdString());
        }
    }
}

void checkPositionalArgumentsExpectedNumber(const QStringList &positionalArgs, const QVector<int> &expectedNumbers)
{
    const auto providedArgumentsNumber = positionalArgs.size();
    if(!expectedNumbers.contains(providedArgumentsNumber))
    {
        QString errorMsg;
        for(auto expectedNumber = expectedNumbers.begin(); expectedNumber != expectedNumbers.end() - 1; expectedNumber++)
        {
            errorMsg += QString::number(*expectedNumber);
            errorMsg += ", or ";
        }
        errorMsg += QString::number(expectedNumbers.last());
        errorMsg += " arguments were expected, but ";
        errorMsg += QString::number(positionalArgs.size());
        errorMsg += " were provided.";
        throw invalid_argument(errorMsg.toStdString());
    }
}

template <class T> QVector<T> filterRecords(const QString &name, const QVector<T> &records)
{
    QVector<T> filteredRecords;
    for(const auto &record : records)
    {
        if(record.getName().contains(name))
        {
            filteredRecords << record;
        }
    }
    return filteredRecords;
}

void createLogicalSystem(const QStringList &options)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help"}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: new" << endl;
        cout << "Enters logical system creation setup" << endl;
        cout << "You can quit the creation process by typing \"QUIT\"" << endl;
        return;
    }

    cout << endl;
    cout << "Enter logical system name:" << endl;
    string name;
    getline(cin, name);
    if(name == "QUIT")
    {
        return;
    }

    cout << endl;
    cout << "Enter logical system description:" << endl;
    string description;
    getline(cin, description);
    if(description == "QUIT")
    {
        return;
    }

    const auto signaturePluginName = setupSignaturePlugin();
    const auto inferenceRulesPluginNames = setupInferenceRulesPlugins();
    const auto proofPluginName = setupProofPlugin();

    cout << endl;
    cout << "Write a type for the well formed formulas." << endl;
    string wffType;
    getline(cin, wffType);
    if(wffType == "QUIT")
    {
        return;
    }

    manager.createLogicalSystem(name.data(),
                                description.data(),
                                inferenceRulesPluginNames,
                                signaturePluginName,
                                proofPluginName,
                                Type(wffType.data()));
    cout << "Logical system created!" << endl;
}

QStringList setupInferenceRulesPlugins()
{
    listInferenceRulePlugins();
    cout << "Choose inference rule plugins:" << endl;
    cout << "   add <name>          Adds inference rule" << endl;
    cout << "   add -i, --id <number> Addds inference rule by list number" << endl;
    cout << "   remove <name>       Removes inference rule" << endl;
    cout << "   remove -i, --id <number> Removes inference rule by list number" << endl;
    cout << "   done                Finishes list." << endl;

    QStringList inferenceRulesNames;
    while(true)
    {
        try
        {
            listInferenceRulePlugins();
            cout << endl;
            cout << "Chosen inference rules:" << endl;
            cout << inferenceRulesNames;
            string command;
            getline(cin, command);
            parser.parse(command);

            const auto mainCommand = parser.getMainCommand();
            const auto options = parser.getOptions();
            const auto positionalArgs = parser.getPositionalArgs();
            checkOptionsAdmissibility(options, QStringList({"i", "id"}));
            checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

            QString arg;
            if(!positionalArgs.isEmpty())
            {
                arg = positionalArgs.first();
            }
            if(mainCommand == "add")
            {
                QString ruleName;
                if(options.contains("i") || options.contains("id"))
                {
                    const auto zeroIndexCompensation = 1;
                    const auto ruleIndex = arg.toUInt() - zeroIndexCompensation;
                    const auto pluginsList = StorageManager::inferenceRulesPluginsList();
                    if(ruleIndex >= static_cast<uint>(pluginsList.size()))
                    {
                        throw invalid_argument("There is no rule corresponding to this number in the list!");
                    }

                    ruleName = pluginsList[ruleIndex].mid(0, pluginsList[ruleIndex].size() - 4); //NOTE refactor
                }
                else
                {
                    ruleName = arg;
                    if(!StorageManager::inferenceRulePluginExists(ruleName))
                    {
                        QString errorMsg;
                        errorMsg += "There is no inference rule plugin named \"";
                        errorMsg += ruleName;
                        errorMsg += "\".";
                        throw invalid_argument(errorMsg.toStdString());
                    }
                }
                if(inferenceRulesNames.contains(ruleName))
                {
                    QString errorMsg;
                    errorMsg += "There is already an inference rule named \"";
                    errorMsg += ruleName;
                    errorMsg += "\" in the list.";
                    throw invalid_argument(errorMsg.toStdString());
                }
                inferenceRulesNames << ruleName;
            }
            else if(mainCommand == "remove")
            {
                if(options.contains("i") || options.contains("id"))
                {
                    const auto zeroIndexCompensation = 1;
                    const auto ruleIndex = arg.toInt() - zeroIndexCompensation;
                    if(ruleIndex >= inferenceRulesNames.size())
                    {
                        throw invalid_argument("There is no inference rule associated with this number in the list!");
                    }
                    inferenceRulesNames.removeAt(ruleIndex);
                }
                else
                {
                    if(!inferenceRulesNames.contains(arg))
                    {
                        QString errorMsg;
                        errorMsg += "There is no inference rule named \"";
                        errorMsg += arg;
                        errorMsg += "\"in the list.";
                        throw invalid_argument(errorMsg.toStdString());
                    }
                    inferenceRulesNames.removeAll(positionalArgs.first());
                }
            }
            else if(mainCommand == "done")
            {
                break;
            }
            else
            {
                invalidCommand(mainCommand);
            }
        }
        catch(invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }

    return inferenceRulesNames;
}

void deleteLogicalSystem(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"s", "silent", "h", "help"})); //TODO
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: list [<options>] [system-name]" << endl;
        cout << "   -s, --silent            Deletes system name silently" << endl;
        return;
    }

    const auto systemName = positionalArgs.first();
    checkLogicalSystemExists(systemName);

    if(options.contains("s") || options.contains("silent"))
    {
        manager.removeLogicalSystem(systemName);
        cout << endl;
        cout << "Logical system \"" + systemName + "\" removed!" << endl;
    }
    else
    {
        cout << endl;
        cout << "Are you sure you want to delete \"" << systemName << "\"? (Y/N)" << endl;

        string command;
        getline(cin, command);
        if(command == "Y")
        {
            manager.removeLogicalSystem(systemName);
            cout << endl;
            cout << "\"" + systemName + "\" removed!" << endl;
        }
    }
}

void loadLogicalSystem(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList());
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: load <exact-system-name>" << endl;
    }

    const auto systemName = positionalArgs.first();
    checkLogicalSystemExists(systemName); //NOTE Probably this is unecessary

    manager.loadLogicalSystem(systemName);
    cout << endl;
    cout << "Logical system \"" << manager.getActiveLogicalSystem()->getName() << "\" loaded." << endl;
}

void checkLogicalSystemExists(const QString &systemName)
{
    if(!StorageManager::logicalSystemExists(systemName))
    {
        QString errorMsg;
        errorMsg += "There is no logical system named \"";
        errorMsg += systemName;
        errorMsg += "\".";
        throw invalid_argument(errorMsg.toStdString());
    }
}

void logicalSystemMenu()
{
    while(true)
    {
        try
        {
            cout << endl
                 << "Logical system menu:" << endl
                 << "   list              List theories" << endl
                 << "   new               Create theory" << endl
                 << "   load              Load theory" << endl
                 << "   delete            Delete theory" << endl
                 << "   rules             List current inference rules" << endl
                 << "   signature         Show currently loaded signature plugin" << endl
                 << "   proof             Show currently loaded proof plugin" << endl
                 << "   tactic            List inference tactic plugins" << endl
                 << "   pre               List pre processor plugins" << endl
                 << "   post              List post processor plugins" << endl
                 << "   unload            Unload logical system" << endl << endl;

            string command;
            getline(cin, command);
            parser.parse(command);
            const auto mainCommand = parser.getMainCommand();
            const auto options = parser.getOptions();
            const auto positionalArgs = parser.getPositionalArgs();

            if(mainCommand == "list")
            {
                listTheories(options, positionalArgs);
            }
            else if(mainCommand == "new")
            {
                createTheory(options);
            }
            else if(mainCommand == "load")
            {
                loadTheory(options, positionalArgs);
                theoryMenu();
            }
            else if(mainCommand == "delete")
            {
                deleteTheory(options, positionalArgs);
            }
            else if(mainCommand == "rules")
            {
                listCurrentlyLoadedInferenceRulesPlugins(QStringList({}));
            }
            else if(mainCommand == "signature")
            {
                listCurrentlyLoadedSignaturePlugin();
            }
            else if(mainCommand == "proof")
            {
                listCurrentlyLoadedProofPlugin();
            }
            else if(mainCommand == "tactic")
            {
                listInferenceTacticPlugins();
            }
            else if(mainCommand == "pre")
            {
                listPreProcessorPlugins();
            }
            else if(mainCommand == "post")
            {
                listPostProcessorsPlugins();
            }
            else if(mainCommand == "unload")
            {
                manager.unloadLogicalSystem();
                return;
            }
            else
            {
                invalidCommand(command.data());
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void listTheories(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "d", "description"}));
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl
             << "usage: list [<options>] [name]" << endl
             << "   -d, --description           Show theories with description" << endl;
        return;
    }

    const auto logicalSystemName = manager.getActiveLogicalSystem()->getName();
    auto records = StorageManager::retrieveTheoriesRecords(logicalSystemName);

    if(records.isEmpty())
    {
        throw runtime_error("There are currently no theories.");
    }

    QVector<TheoryRecord> filteredRecords;
    if(positionalArgs.isEmpty())
    {
        filteredRecords = records;
    }
    else
    {
        const auto theoryName = positionalArgs.first();
        filteredRecords = filterRecords(theoryName, records);
    }

    cout << endl;
    cout << "Theory list:" << endl;
    if(options.contains("d") || options.contains("description"))
    {
        for(const TheoryRecord &record : filteredRecords)
        {
            cout << "Name:" << record.getName() << endl
                 << "Description:" << record.getDescription() << endl;
        }
    }
    else
    {
        for(const TheoryRecord &record : filteredRecords)
        {
            cout << "Name:" << record.getName() << endl;
        }
    }
}

void createTheory(const QStringList &options)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "i", "id"}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: new" << endl;
        cout << "Begins theory creation setup" << endl;
        cout << "Enter \"QUIT\" to quit creation process" << endl;
        return;
    }

    cout << endl
         << "Enter theory name:" << endl;
    string name;
    getline(cin, name);
    if(name == "QUIT")
    {
        return;
    }

    cout << endl
         << "Enter theory description:" << endl;
    string description;
    getline(cin, description);
    if(description == "QUIT")
    {
        return;
    }

    //FIXME Fix theory builder
    const auto logicalSystemName = manager.getActiveLogicalSystem()->getName();
    const auto logicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(logicalSystemName);
    const auto signaturePluginName = logicalSystemPluginsRecord.getSignaturePluginName();
    auto signature = PluginManager::fetchPlugin<Signature>(StorageManager::signaturePluginPath(signaturePluginName));
    TheoryBuilder builder(manager.getActiveLogicalSystem(),
                          signature,
                          name.data(),
                          description.data());

    //TODO setup signature if necessary
    //TODO setup formatters and string processors

    setupAxioms(builder);
    manager.createTheory(builder, TheoryPluginsRecord());
}

QString setupSignaturePlugin()
{
    cout << endl
         << "Choose signature plugin:" << endl;
    listSignaturePlugins();

    while(true)
    {
        try
        {
            string signatureName;
            getline(cin, signatureName);
            QString wrappedName(signatureName.data());

            checkSignaturePluginExists(wrappedName);
            return wrappedName;
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void checkSignaturePluginExists(const QString &name)
{
    if(!StorageManager::signaturePluginExists(name))
    {
        QString errorMsg;
        errorMsg += "There is no signature plugin named \"";
        errorMsg += name;
        errorMsg += "\".";
        throw invalid_argument(errorMsg.toStdString());
    }
}

void setupAxioms(TheoryBuilder &builder)
{
    cout << endl
         << "Setup theory axioms:" << endl
         << "   add <axiom>         Add axiom" << endl
         << "   remove <axiom>      Remove axiom" << endl
         << "   remove -i, --id, <axiom-number> Remove axiom by number" << endl
         << "   done                Finishes axiom list" << endl;

    while(true)
    {
        try
        {
            cout << endl
                 << "Axioms list:" << endl;
            unsigned int count = 1;
            for(const Formula &axiom : builder.getAxioms())
            {
                cout << count << "." << axiom.toString() << endl;
            }

            string command;
            getline(cin, command);
            parser.parse(command);

            const auto mainCommand = parser.getMainCommand();
            const auto options = parser.getOptions();
            const auto positionalArgs = parser.getPositionalArgs();

            checkOptionsAdmissibility(options, QStringList());
            checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

            if(mainCommand == "add")
            {
                checkSetupAxiomsPositionalArgs(positionalArgs);
                builder.addAxiom(positionalArgs.first());
            }
            else if(mainCommand == "remove")
            {
                const auto arg = positionalArgs.first();
                if(options.contains("i") || options.contains("id"))
                {
                    const auto zeroIndexCompensation = 1;
                    const auto axiomIndex = arg.toInt() - zeroIndexCompensation;
                    if(axiomIndex >= builder.getAxioms().size())
                    {
                        throw invalid_argument("There is no axiom associated with this number in the list!");
                    }

                    builder.removeAxiom(axiomIndex);
                }
                else
                {
                    checkSetupAxiomsPositionalArgs(positionalArgs);
                    builder.removeAxiom(positionalArgs.first());
                }
            }
            else if(mainCommand == "done")
            {
                break;
            }
            else
            {
                invalidCommand(mainCommand);
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void checkSetupAxiomsPositionalArgs(const QStringList &positionalArgs)
{
    if(positionalArgs.isEmpty())
    {
        QString errorMsg;
        errorMsg += "No axiom was provided.";
        throw invalid_argument(errorMsg.toStdString());
    }
}

void loadTheory(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help"}));
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: load <exact-theory-name>" << endl;
        return;
    }

    const auto &theoryName = positionalArgs.first();
    manager.loadTheory(theoryName);

    cout << endl
         << "Theory " << theoryName << " loaded." << endl;
}

void deleteTheory(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "s", "silent"}));
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: delete [<options>] <theory-name>" << endl;
        cout << "   -s, --silent            Deletes theory silently" << endl;
    }

    const auto &theoryName = positionalArgs.first();
    if(options.contains("s") || options.contains("silent"))
    {
        manager.removeTheory(theoryName);
        cout << endl
             << "Theory \"" << theoryName << "\" deleted." << endl;
    }
    else
    {
        cout << endl;
        cout << "Are you sure you want to delete \"" + theoryName + "\"?(Y/N)" << endl;

        string command;
        getline(cin, command);
        if(command == "Y")
        {
            manager.removeTheory(theoryName);
            cout << endl
                 << "Theory \"" << theoryName << "\" deleted." << endl;
        }
    }
}

void listSignaturePlugins()
{
    cout << endl << "Signature plugins list:" << endl;
    cout << endl << StorageManager::signaturePluginsList();
}

void listInferenceTacticPlugins()
{
    cout << endl << "Inference tactic plugins list:" << endl;
    cout << endl << StorageManager::inferenceTacticsPluginsList();
}

void listPreProcessorPlugins()
{
    cout << endl << "Pre processor plugins list:" << endl;
    cout << endl << StorageManager::preProcessorPluginsList();
}

void listPostProcessorsPlugins()
{
    cout << endl << "Post processor plugins list" << endl;
    cout << endl << StorageManager::postProcessorPluginsList();
}

void theoryMenu()
{
    while(true)
    {
        try
        {
            cout << endl;
            cout << "Theory menu:" << endl;
            cout << "list           List proofs, axioms or plugins" << endl;
            cout << "add            Add proof or plugin" << endl;
            cout << "remove         Remove proof or plugin" << endl;
            cout << "load           Load proof" << endl;
            cout << "setup          Setup plugin" << endl;
            cout << "unload         Unload theory" << endl << endl;

            string command;
            getline(cin, command);
            parser.parse(command);
            auto mainCommand = parser.getMainCommand();
            auto options = parser.getOptions();
            auto positionalArgs = parser.getPositionalArgs();

            //TODO
            if(mainCommand == "list")
            {
                theoryMenuList(options, positionalArgs);
            }
            else if(mainCommand == "add")
            {
                theoryMenuAdd(options, positionalArgs);
            }
            else if(mainCommand == "remove")
            {

            }
            else if(mainCommand == "load")
            {
                loadProof(options, positionalArgs);
            }
            else if(mainCommand == "setup")
            {

            }
            else if(mainCommand == "unload")
            {
                manager.unloadTheory();
                return;
            }
            else
            {
                invalidCommand(mainCommand);
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}


void theoryMenuList(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "p", "proofs", "a", "axioms", "r", "rules", "e", "pre", "o", "post", "t", "tactics", "d", "description", "l", "loaded"}));
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: list [<options>] [<item-name>]" << endl;
        cout << "   -d, --description           Lists with descriptions" << endl;
        cout << "   -p, --proofs                Lists proofs" << endl;
        cout << "   -a, --axioms                Lists axioms" << endl;
        cout << "   -r, --rules                 Lists loaded inference rules" << endl;
        cout << "   -t, --tactics               List inference tactics" << endl;
        cout << "   -e, --pre                   Lists pre processors" << endl;
        cout << "   -o, --post                  Lists post processors" << endl;
        cout << "   -l, --loaded                Lists loaded items only (when appliable)" << endl;
        return;
    }


    //TODO
    if(options.contains("a") || options.contains("axioms"))
    {
        listCurrentTheoryAxioms();
    }
    else if(options.contains("r") || options.contains("rules"))
    {
        listCurrentlyLoadedInferenceRulesPlugins(options);
    }
    else if(options.contains("t") || options.contains("tactics"))
    {
        if(options.contains("l") || options.contains("loaded"))
        {
        }
        else
        {
            listPreProcessorPlugins();
        }
    }
    else if(options.contains("e") || options.contains("pre"))
    {

    }
    else if(options.contains("o") || options.contains("post"))
    {

    }
}

void listCurrentlyLoadedInferenceRulesPlugins(const QStringList &options)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "d", "description"}));

    if(options.contains("h"), options.contains("help"))
    {
        cout << endl;
        cout << "usage: rules TODO!" << endl;
    }

    const auto activeSystem = manager.getActiveLogicalSystem();
    const auto rules = activeSystem->getInferenceRules();
    cout << endl;
    cout << "Currently loaded inference rules:" << endl;
    QString output;
    for(const auto &rule : rules)
    {
        output += rule->name() += "\n";
    }
    if(options.contains("d") || options.contains("description"))
    {
        for(const auto &rule : rules)
        {
            //TODO Implement descriptions in inference rules
        }
    }
    cout << output;
}

void listCurrentTheoryAxioms()
{
    const auto currentTheory = manager.getActiveTheory();
    const auto axioms = currentTheory->getAxioms();
    cout << endl;
    for(const auto &axiom : axioms)
    {
        cout << axiom.formattedString() << endl;
    }
}

void theoryMenuAdd(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "p", "proof", "t", "tactic", "e", "pre", "o", "post"}));
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: add <options> <args>" << endl;
        cout << "   p, proof            Add proof" << endl;
        cout << "   t, tactic           Add inference tactics" << endl;
        cout << "   e, pre              Add pre processor" << endl;
        cout << "   o, post             Add post processor" << endl;
        return;
    }

    //TODO

    if(options.contains("p") || options.contains("proof"))
    {
        createProof();
    }
}

void createProof()
{
    cout << endl;
    cout << "Enter proof name:" << endl;

    string command;
    getline(cin, command);
    const QString name(command.data());
    if(command == "QUIT")
    {
        return;
    }

    cout << endl;
    cout << "Enter proof description:" << endl;
    getline(cin, command);
    const QString description(command.data());
    if(command == "QUIT")
    {
        return;
    }

    cout << endl;
    cout << "Enter proof premises:" << endl;
    cout << "   add         Add premiss" << endl;
    cout << "   remove      Remove premiss" << endl;
    cout << "   done        Done" << endl;

    QStringList premises;
    setupProofPremises(premises);

    cout << endl;
    cout << "Enter proof conclusion:" << endl;

    QString conclusion;
    setupProofConclusion(conclusion);

    manager.createProof(name, description, premises, conclusion);
    cout << endl;
    cout << "Proof created!" << endl; //FIXME Fix conclusion message
}

void setupProofPremises(QStringList &premises)
{
    const auto theory = manager.getActiveTheory();
    Parser formulaParser(theory->getSignature(), manager.getActiveLogicalSystem()->getWffType());
    while(true)
    {
        if(!premises.isEmpty())
        {
            cout << endl;
            for(const auto &premiss : premises)
            {
                cout << premiss << endl;
            }
        }
        try
        {
            string command;
            getline(cin, command);
            parser.parse(command);
            const auto mainCommand = parser.getMainCommand();
            const auto options = parser.getOptions();
            const auto positionalArgs = parser.getPositionalArgs();

            checkOptionsAdmissibility(options, QStringList({}));
            checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

            if(mainCommand == "add")
            {
                const auto premiss = positionalArgs.first();
                formulaParser.parse(premiss);
                premises << premiss;
            }
            else if(mainCommand == "remove")
            {
                const auto premiss = positionalArgs.first();
                premises.removeAll(premiss);
            }
            else if(mainCommand == "done")
            {
                return;
            }
            else
            {
                invalidCommand(mainCommand);
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void setupProofConclusion(QString &conclusion)
{
    const auto theory = manager.getActiveTheory();
    Parser formulaParser(theory->getSignature(), manager.getActiveLogicalSystem()->getWffType());
    while(true)
    {
        string command;
        getline(cin, command);
        auto formula = command.data();

        try
        {
            formulaParser.parse(formula);
            conclusion = formula;
            return;
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void loadProof(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "i", "id"}));
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    //TODO

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: load [<options>] <proof-name>" << endl;
        cout << "   -i, --id            Load proof by id" << endl;
    }

    if(options.contains("i") || options.contains("id"))
    {
        const auto id = positionalArgs.first().toUInt();
        auto assistant = manager.loadProof(id);
        proofAssistantMenu(assistant);
    }
    else
    {

    }

}

void proofAssistantMenu(ProofAssistant &assistant)
{
    //TODO
    cout << endl;
    cout << "Proof loaded!" << endl;

    while(true)
    {
        cout << endl;
        const auto &proof = assistant.getProof();
        const auto linesOfProofSize = proof.getLinesOfProof().size();
        for(auto index = 0; index < linesOfProofSize; index++)
        {
            const auto lineNumber = index + 1;
            cout << lineNumber << ". "
                 << proof.printLineOfProof(lineNumber) << ' '
                 << proof.getLineOfProof(lineNumber).getJustification().getInferenceRuleCallCommand()
                 << proof.getLineOfProof(lineNumber).getJustification().getArgumentList() << endl;
        }

        try
        {
            string command;
            getline(cin, command);
            parser.parse(command);
            const auto mainCommand = parser.getMainCommand();
            const auto options = parser.getOptions();
            const auto positionalArgs = parser.getPositionalArgs();

            if(mainCommand == "call")
            {
                if(positionalArgs.isEmpty())
                {
                   continue;
                }
                const auto callCommand = positionalArgs.first();
                const auto argumentList = positionalArgs.mid(1);
                assistant.applyInferenceRule(callCommand, argumentList);

                if(proof.isFinished())
                {
                    cout << endl;
                    cout << "Proof finished!" << endl;
                    manager.saveProof(assistant);
                    return;
                }
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void listProofPlugins()
{
    //Standardize
    cout << endl << "Proof Plugins:" << endl;
    const auto list = StorageManager::proofPluginsList();
    auto proofCount = 1;
    for(const auto &proof : list)
    {
        cout << proofCount << "." << proof.mid(0, proof.size() - 4) << endl; // Refactor magic number (take off .dll)
        proofCount++;
    }
}

QString setupProofPlugin()
{
    cout << endl
         << "Choose proof plugin:" << endl;
    listProofPlugins();

    while(true)
    {
        try
        {
            string proofName;
            getline(cin, proofName);
            QString wrappedName(proofName.data());

            checkProofPluginExists(wrappedName);
            return wrappedName;
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
        catch(const runtime_error &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}

void checkProofPluginExists(const QString &pluginName)
{
    if(!StorageManager::proofPluginExists(pluginName))
    {
        QString errorMsg;
        errorMsg += "There is no signature plugin named \"";
        errorMsg += pluginName;
        errorMsg += "\".";
        throw invalid_argument(errorMsg.toStdString());
    }
}

void listCurrentlyLoadedSignaturePlugin()
{
    cout << endl;
    cout << "Currently loaded signature plugin is: ";
    const auto logicalSystemName = manager.getActiveLogicalSystem()->getName();
    const auto logicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(logicalSystemName);
    cout << logicalSystemPluginsRecord.getSignaturePluginName() << endl;
}

void listCurrentlyLoadedProofPlugin()
{
    cout << endl;
    cout << "Currently loaded proof plugin is: ";
    const auto logicalSystemName = manager.getActiveLogicalSystem()->getName();
    const auto logicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(logicalSystemName);
    cout << logicalSystemPluginsRecord.getProofPluginName() << endl;
}
