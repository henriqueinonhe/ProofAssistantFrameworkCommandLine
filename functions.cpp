#include "functions.h"
#include <iostream>
#include <QDir>
#include "storagemanager.h"
#include "commandlineparser.h"
#include "logicalsystemrecord.h"
#include "theoryrecord.h"
#include "theorybuilder.h"
#include "pluginmanager.h"
#include "theorypluginsrecord.h"
#include "proofassistant.h"
#include "programassistant.h"
#include "logicalsystemassistant.h"
#include "theoryassistant.h"
#include "logicalsystempluginsrecord.h"
#include "lineofproof.h"
#include "proofwithhypothesiscliprinter.h"
#include "proofprintercliplugin.h"

ProgramAssistant programAssistant;
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
        dir.mkdir("Proofs");
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
                 << "   printers       List proof printer plugins" << endl
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
            else if(mainCommand == "printers")
            {
                listProofPrinterPlugins();
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
    cout << endl << "Inferece Rule Plugins:" << endl;
    const auto list = StorageManager::inferenceRulesPluginsList();
    outputOrderedPluginList(list);
}

void listLogicalSystems(const QStringList &options, const QStringList &positionalArgs)
{
    const QStringList admissibleOptions({"h", "help", "d", "description"});
    checkOptionsAdmissibility(options, admissibleOptions);

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: list [<options>] [system-name]" << endl;
        cout << "   -d, --description           Shows list with descriptions" << endl;
        return;
    }

    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));
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
        cout << "   Enters logical system creation setup" << endl;
        cout << "   You can quit the creation process by typing \"QUIT\"" << endl;
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
    if(signaturePluginName == "QUIT")
    {
        return;
    }

    const auto inferenceRulesPluginNames = setupInferenceRulesPlugins();
    if(!inferenceRulesPluginNames.isEmpty() && inferenceRulesPluginNames.first() == "QUIT")
    {
        return;
    }

    const auto proofPluginName = setupProofPlugin();
    if(proofPluginName == "QUIT")
    {
        return;
    }



    cout << endl;
    cout << "Write a type for the well formed formulas." << endl;
    string wffType;
    getline(cin, wffType);
    if(wffType == "QUIT")
    {
        return;
    }

    programAssistant.createLogicalSystem(name.data(),
                                         description.data(),
                                         inferenceRulesPluginNames,
                                         signaturePluginName,
                                         proofPluginName,
                                         Type(wffType.data()));
    cout << endl << "Logical system \"" << name << "\" created!" << endl;
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
            else if(mainCommand == "QUIT")
            {
                return QStringList({"QUIT"});
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
    checkOptionsAdmissibility(options, QStringList({"s", "silent", "h", "help"}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: list [<options>] [system-name]" << endl;
        cout << "   -s, --silent            Deletes system name silently" << endl;
        return;
    }

    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));
    const auto systemName = positionalArgs.first();
    checkLogicalSystemExists(systemName);

    if(options.contains("s") || options.contains("silent"))
    {
        programAssistant.removeLogicalSystem(systemName);
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
            programAssistant.removeLogicalSystem(systemName);
            cout << endl;
            cout << "\"" + systemName + "\" removed!" << endl;
        }
    }
}

void loadLogicalSystem(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help"}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: load <exact-system-name>" << endl;
        return;
    }

    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));
    const auto systemName = positionalArgs.first();
    checkLogicalSystemExists(systemName);
    auto logicalSystemAsssitant = programAssistant.loadLogicalSystem(systemName);
    cout << endl;
    cout << "Logical system \"" << logicalSystemAsssitant.getActiveLogicalSystem().getName() << "\" loaded." << endl;
    logicalSystemMenu(logicalSystemAsssitant);
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

void logicalSystemMenu(LogicalSystemAssistant &logicalSystemAssistant)
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
                listTheories(logicalSystemAssistant, options, positionalArgs);
            }
            else if(mainCommand == "new")
            {
                createTheory(logicalSystemAssistant, options);
            }
            else if(mainCommand == "load")
            {
                loadTheory(logicalSystemAssistant, options, positionalArgs);
            }
            else if(mainCommand == "delete")
            {
                deleteTheory(logicalSystemAssistant, options, positionalArgs);
            }
            else if(mainCommand == "rules")
            {
                listCurrentlyLoadedInferenceRulesPlugins(logicalSystemAssistant);
            }
            else if(mainCommand == "signature")
            {
                listCurrentlyLoadedSignaturePlugin(logicalSystemAssistant);
            }
            else if(mainCommand == "proof")
            {
                listCurrentlyLoadedProofPlugin(logicalSystemAssistant);
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

void listTheories(const LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options, const QStringList &positionalArgs)
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

    const auto logicalSystemName = logicalSystemAssistant.getActiveLogicalSystem().getName();
    auto records = StorageManager::retrieveTheoriesRecords(logicalSystemName);

    if(records.isEmpty())
    {
        cout << endl << "There are currently no theories." << endl;
        return;
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

void createTheory(LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options)
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

    const auto logicalSystemName = logicalSystemAssistant.getActiveLogicalSystem().getName();
    const auto logicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(logicalSystemName);
    const auto signaturePluginName = logicalSystemPluginsRecord.getSignaturePluginName();
    auto signature = PluginManager::fetchPlugin<Signature>(StorageManager::signaturePluginPath(signaturePluginName));
    //Convert signature to SignatureCLIPlugin and setup!
    TheoryBuilder builder(&logicalSystemAssistant.getActiveLogicalSystem(),
                          signature,
                          name.data(),
                          description.data());

    const auto proofPrinterPluginName = setupProofPrinterPlugin();
    if(proofPrinterPluginName == "QUIT")
    {
        return;
    }

    //TODO setup formatters and string processors

    setupAxioms(builder);
    logicalSystemAssistant.createTheory(builder, TheoryPluginsRecord(proofPrinterPluginName));
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
            string signaturePluginName;
            getline(cin, signaturePluginName);
            if(signaturePluginName == "QUIT")
            {
                return "QUIT";
            }
            QString wrappedName(signaturePluginName.data());

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

void loadTheory(LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help"}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: load <exact-theory-name>" << endl;
        return;
    }

    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));
    const auto &theoryName = positionalArgs.first();
    auto theoryAssistant = logicalSystemAssistant.loadTheory(theoryName);

    cout << endl
         << "Theory " << theoryName << " loaded." << endl;
    theoryMenu(theoryAssistant);
}

void deleteTheory(LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"h", "help", "s", "silent"}));

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: delete [<options>] <theory-name>" << endl;
        cout << "   -s, --silent            Deletes theory silently" << endl;
        return;
    }

    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));
    const auto &theoryName = positionalArgs.first();
    if(options.contains("s") || options.contains("silent"))
    {
        logicalSystemAssistant.removeTheory(theoryName);
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
            logicalSystemAssistant.removeTheory(theoryName);
            cout << endl
                 << "Theory \"" << theoryName << "\" deleted." << endl;
        }
    }
}

void listSignaturePlugins()
{
    cout << endl << "Signature Plugins List:" << endl;
    outputOrderedPluginList(StorageManager::signaturePluginsList());
}

void listInferenceTacticPlugins()
{
    cout << endl << "Inference Tactic Plugins List:" << endl;
    outputOrderedPluginList(StorageManager::inferenceTacticsPluginsList());
}

void listPreProcessorPlugins()
{
    cout << endl << "Pre processor plugins list:" << endl;
    outputOrderedPluginList(StorageManager::preProcessorPluginsList());
}

void listPostProcessorsPlugins()
{
    cout << endl << "Post processor plugins list" << endl;
    outputOrderedPluginList(StorageManager::postProcessorPluginsList());
}

void theoryMenu(TheoryAssistant &theoryAssistant)
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
                theoryMenuAdd(theoryAssistant, options, positionalArgs);
            }
            else if(mainCommand == "remove")
            {

            }
            else if(mainCommand == "load")
            {
                loadProof(theoryAssistant, options, positionalArgs);
            }
            else if(mainCommand == "setup")
            {

            }
            else if(mainCommand == "unload")
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
        //listCurrentTheoryAxioms();
    }
    else if(options.contains("r") || options.contains("rules"))
    {
        //listCurrentlyLoadedInferenceRulesPlugins();
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

void listCurrentlyLoadedInferenceRulesPlugins(const LogicalSystemAssistant &logicalSystemAssistant)
{
    const auto loadedLogicalSystemName = logicalSystemAssistant.getActiveLogicalSystem().getName();
    const auto loadedLogicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(loadedLogicalSystemName);
    const auto loadedInferenceRulesPluginsNameList = loadedLogicalSystemPluginsRecord.getInferenceRulesNamesList();
    cout << endl << "Loaded Inference Rules Plugins List:" << endl;
    outputOrderedList(loadedInferenceRulesPluginsNameList);
}

void listCurrentTheoryAxioms(const TheoryAssistant &theoryAssistant)
{
    const auto &currentTheory = theoryAssistant.getActiveTheory();
    const auto axioms = currentTheory.getAxioms();
    cout << endl;
    for(const auto &axiom : axioms)
    {
        cout << axiom.formattedString() << endl;
    }
}

void theoryMenuAdd(TheoryAssistant &theoryAssistant, const QStringList &options, const QStringList &positionalArgs)
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
        createProof(theoryAssistant);
    }
}

void createProof(TheoryAssistant &theoryAssistant)
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
    setupProofPremises(theoryAssistant, premises);

    cout << endl;
    cout << "Enter proof conclusion:" << endl;

    QString conclusion;
    setupProofConclusion(theoryAssistant, conclusion);

    theoryAssistant.createProof(name, description, premises, conclusion);
    cout << endl;
    cout << "Proof created!" << endl; //FIXME Fix conclusion message
}

void setupProofPremises(TheoryAssistant &theoryAssistant, QStringList &premises)
{
    const auto &theory = theoryAssistant.getActiveTheory();
    Parser formulaParser(theory.getSignature(), theoryAssistant.getActiveLogicalSystem().getWffType());
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

void setupProofConclusion(TheoryAssistant &theoryAssistant, QString &conclusion)
{
    const auto &theory = theoryAssistant.getActiveTheory();
    Parser formulaParser(theory.getSignature(), theoryAssistant.getActiveLogicalSystem().getWffType());
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

void loadProof(TheoryAssistant &theoryAssistant, const QStringList &options, const QStringList &positionalArgs)
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
        auto assistant = theoryAssistant.loadProof(id);
        proofAssistantMenu(theoryAssistant, assistant);
    }
}

void proofAssistantMenu(TheoryAssistant &theoryAssistant, ProofAssistant &assistant)
{
    //TODO
    cout << endl;
    cout << "Proof loaded!" << endl;

    while(true)
    {
        const auto &proof = assistant.getProof();
        const auto &proofPrinter = dynamic_cast<const ProofPrinterCLIPlugin &>(theoryAssistant.getProofPrinter());
        auto stringnizedProof = proofPrinter.printProof(theoryAssistant.getActiveTheory().getPostFormatter(), proof);
        cout << endl;
        cout << stringnizedProof;

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
                    theoryAssistant.saveProof(assistant);
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
    cout << endl << "Proof Plugins List:" << endl;
    outputOrderedPluginList(StorageManager::proofPluginsList());
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
            string proofPluginName;
            getline(cin, proofPluginName);
            if(proofPluginName == "QUIT")
            {
                return "QUIT";
            }

            QString wrappedName(proofPluginName.data());

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
        errorMsg += "There is no proof plugin named \"";
        errorMsg += pluginName;
        errorMsg += "\".";
        throw invalid_argument(errorMsg.toStdString());
    }
}

void listCurrentlyLoadedSignaturePlugin(const LogicalSystemAssistant &logicalSystemAssistant)
{
    cout << endl;
    cout << "Currently Loaded Signature Plugin:" << endl;
    const auto logicalSystemName = logicalSystemAssistant.getActiveLogicalSystem().getName();
    const auto logicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(logicalSystemName);
    cout << logicalSystemPluginsRecord.getSignaturePluginName() << endl;
}

void listCurrentlyLoadedProofPlugin(const LogicalSystemAssistant &logicalSystemAssistant)
{
    cout << endl;
    cout << "Currently Loaded Proof Plugin:" << endl;
    const auto logicalSystemName = logicalSystemAssistant.getActiveLogicalSystem().getName();
    const auto logicalSystemPluginsRecord = StorageManager::retrieveLogicalSystemPluginsRecord(logicalSystemName);
    cout << logicalSystemPluginsRecord.getProofPluginName() << endl;
}

void outputOrderedPluginList(const QStringList &pluginList)
{
    auto entryCount = 1;
    for(const auto &entry : pluginList)
    {
        const auto entryWithoutDllEnding = entry.mid(0, entry.size() - 4);
        cout << entryCount << ". " << entryWithoutDllEnding << endl;
        entryCount++;
    }
}

void outputOrderedList(const QStringList &list)
{
    auto entryCount = 1;
    for(const auto &entry : list)
    {
        cout << entryCount << ". " << entry << endl;
        entryCount++;
    }
}

QString setupProofPrinterPlugin()
{
    cout << endl
         << "Choose Proof Printer Plugin:" << endl;
    listProofPrinterPlugins();

    while(true)
    {
        try
        {
            string proofPrinterPluginName;
            getline(cin, proofPrinterPluginName);
            if(proofPrinterPluginName == "QUIT")
            {
                return "QUIT";
            }

            QString wrappedName(proofPrinterPluginName.data());

            checkProofPrinterPluginExists(wrappedName);
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

void listProofPrinterPlugins()
{
    cout << endl << "Proof Printer Plugins List:" << endl;
    outputOrderedPluginList(StorageManager::proofPrinterPluginsList());
}

void checkProofPrinterPluginExists(const QString &pluginName)
{
    if(!StorageManager::proofPrinterPluginExists(pluginName))
    {
        QString errorMsg;
        errorMsg += "There is no proof printer plugin named \"";
        errorMsg += pluginName;
        errorMsg += "\".";
        throw invalid_argument(errorMsg.toStdString());
    }
}
