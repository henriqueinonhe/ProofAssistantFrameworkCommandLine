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

ProgramManager manager;
CommandLineParser parser;

//Routines
void createRootDir()
{
    QDir dir;
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

    const QString currentWorkingDir = dir.absolutePath();
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
                 << "   rules          List inference rules" << endl << endl;

            string command;
            getline(cin, command);

            parser.parse(command);
            const QString mainCommand = parser.getMainCommand();
            const QStringList options = parser.getOptions();
            const QStringList positionalArgs = parser.getPositionalArgs();

            if(mainCommand == "list")
            {
                listLogicalSystems(options, positionalArgs);
            }
            else if(mainCommand == "new")
            {
                createLogicalSystem();
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
            else if(mainCommand == "rules")
            {
                listInferenceRulePlugins();
            }
            else
            {
                invalidCommand(mainCommand);
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << e.what();
        }
        catch(const runtime_error &e)
        {
            cerr << e.what();
        }

    }
}

void invalidCommand(const QString &string)
{
    QString errorMsg;
    errorMsg += "\"";
    errorMsg += string;
    errorMsg += "\" is not a valid command.\n";
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
        stream << string;
    }
    stream << endl;
    return stream;
}

void listInferenceRulePlugins()
{
    cout << endl << "Inferece Rule Plugins:" << endl;
    cout << StorageManager::inferenceRulesPluginsList() << endl << endl;
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

    QVector<LogicalSystemRecord> records = StorageManager::retrieveLogicalSystemsRecords();
    if(records.isEmpty())
    {
        cout << endl;
        cout << "There are currently no logical systems." << endl;
        return;
    }

    if(!positionalArgs.isEmpty())
    {
        const QString &systemName = positionalArgs.first();
        records = filterRecords(systemName, records);
    }

    cout << endl;
    cout << "Logical Systems:" << endl;
    if(options.contains("d") || options.contains("description"))
    {
        for(const LogicalSystemRecord &record : records)
        {
            cout << "Name: " << record.getName() << endl;
            cout << "Description: " << record.getDescription() << endl << endl;
        }
    }
    else
    {
        for(const LogicalSystemRecord &record : records)
        {
            cout << "Name: " << record.getName() << endl;
        }
    }
}

void checkOptionsAdmissibility(const QStringList &options, const QStringList &admissibleOptions)
{
    for(const QString &option : options)
    {
        bool isAdmissible = false;
        for(const QString &admissibleOption : admissibleOptions)
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
            errorMsg += "\" is not an admissible option.\n";
            throw invalid_argument(errorMsg.toStdString());
        }
    }
}

void checkPositionalArgumentsExpectedNumber(const QStringList &positionalArgs, const QVector<int> &expectedNumbers)
{
    const int providedArgumentsNumber = positionalArgs.size();
    if(!expectedNumbers.contains(providedArgumentsNumber))
    {
        QString errorMsg;
        for(auto expectedNumber = expectedNumbers.begin(); expectedNumber != expectedNumbers.end() - 1; expectedNumber++)
        {
            errorMsg += QString::number(*expectedNumber);
            errorMsg += ", or";
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
    for(const T &record : records)
    {
        if(record.getName().contains(name))
        {
            filteredRecords << record;
        }
    }
    return filteredRecords;
}

void createLogicalSystem()
{
    cout << endl;
    cout << "Enter logical system name:" << endl;
    string name;
    getline(cin, name);

    cout << endl;
    cout << "Enter logical system description:" << endl;
    string description;
    getline(cin, description);

    QStringList inferenceRulesNames;
    setupInferenceRules(inferenceRulesNames);

    cout << endl;
    cout << "Write a type for the well formed formulas." << endl;
    string wffType;
    getline(cin, wffType);
    manager.createLogicalSystem(name.data(), description.data(), inferenceRulesNames, Type(wffType.data()));
    cout << "Logical system created!" << endl;
}

void setupInferenceRules(QStringList &inferenceRulesNames)
{
    listInferenceRulePlugins();
    cout << endl;
    cout << "Choose inference rule plugins:" << endl;
    cout << "   add <name>          Adds inference rule." << endl;
    cout << "   remove <name>       Removes inference rule." << endl;
    cout << "   done                Finishes list." << endl;

    while(true)
    {
        try
        {
            cout << endl;
            cout << "Chosen inference rules:" << endl;
            cout << inferenceRulesNames << endl;
            string command;
            getline(cin, command);
            parser.parse(command);

            const QString mainCommand = parser.getMainCommand();
            const QStringList options = parser.getOptions();
            const QStringList positionalArgs = parser.getPositionalArgs();
            checkOptionsAdmissibility(options, QStringList());
            checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

            QString ruleName;
            if(!positionalArgs.isEmpty())
            {
                ruleName = positionalArgs.first();
            }
            if(mainCommand == "add")
            {
                if(inferenceRulesNames.contains(ruleName))
                {
                    QString errorMsg;
                    errorMsg += "There is already an inference rule named \"";
                    errorMsg += ruleName;
                    errorMsg += "\" in the list.\n";
                    throw invalid_argument(errorMsg.toStdString());
                }
                if(!StorageManager::inferenceRulePluginExists(ruleName))
                {
                    QString errorMsg;
                    errorMsg += "There is no inference rule plugin named \"";
                    errorMsg += ruleName;
                    errorMsg += "\".\n";
                    throw invalid_argument(errorMsg.toStdString());
                }
                inferenceRulesNames << ruleName;
            }
            else if(mainCommand == "remove")
            {
                if(!inferenceRulesNames.contains(ruleName))
                {
                    QString errorMsg;
                    errorMsg += "There is no inference rule named \"";
                    errorMsg += ruleName;
                    errorMsg += "\"in the list.\n";
                    throw invalid_argument(errorMsg.toStdString());
                }
                inferenceRulesNames.removeAll(positionalArgs.first());
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
            cerr << e.what();
        }
        catch(runtime_error &e)
        {
            cerr << e.what();
        }
    }
}

void deleteLogicalSystem(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList({"s", "silent", "h", "help"})); //TODO
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    const QString systemName = positionalArgs.first();
    checkLogicalSystemExists(systemName);

    cout << endl;
    cout << "Are you sure you want to delete \"" << systemName << "\"? (Y/N)" << endl;

    string command;
    getline(cin, command);
    if(command == "Y")
    {
        manager.removeLogicalSystem(systemName);
    }
}

void loadLogicalSystem(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList());
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    const QString systemName = positionalArgs.first();
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
    try
    {
        while(true)
        {
            string command;
            getline(cin, command);

            cout << endl
                 << "Logical system menu:" << endl
                 << "   list              List theories" << endl
                 << "   new               Create theory" << endl
                 << "   load              Load theory" << endl
                 << "   delete            Delete theory" << endl
                 << "   signature         List signature plugins" << endl
                 << "   tactic            List inference tactic plugins" << endl
                 << "   pre               List pre processor plugins" << endl
                 << "   post              List post processor plugins" << endl
                 << "   unload            Unload logical system" << endl;

            parser.parse(command);
            const QString mainCommand = parser.getMainCommand();
            const QStringList options = parser.getOptions();
            const QStringList positionalArgs = parser.getPositionalArgs();

            if(mainCommand == "list")
            {
                listTheories(options, positionalArgs);
            }
            else if(mainCommand == "new")
            {
                createTheory();
            }
            else if(mainCommand == "load")
            {
                loadTheory(options, positionalArgs);
            }
            else if(mainCommand == "delete")
            {
                deleteTheory(options, positionalArgs);
            }
            else if(mainCommand == "signature")
            {
                listSignaturePlugins();
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

    const QString logicalSystemName = manager.getActiveLogicalSystem()->getName();
    QVector<TheoryRecord> records = StorageManager::retrieveTheoriesRecords(logicalSystemName);

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
        const QString theoryName = positionalArgs.first();
        filteredRecords = filterRecords(theoryName, records);
    }

    cout << "Theory list:" << endl;
    if(options.contains("d") || options.contains("description"))
    {
        for(const TheoryRecord &record : filteredRecords)
        {
            cout << record.getName() << endl
                 << record.getDescription() << endl;
        }
    }
    else
    {
        for(const TheoryRecord &record : filteredRecords)
        {
            cout << record.getName() << endl;
        }
    }
}

void createTheory()
{
    cout << endl
         << "Enter theory name:" << endl;
    string name;
    getline(cin, name);

    cout << endl
         << "Enter theory description:" << endl;
    string description;
    getline(cin, description);

    QString signaturePluginName;
    setupSignature(signaturePluginName);

    shared_ptr<Signature> signature = PluginManager::fetchPlugin<Signature>(StorageManager::signaturePluginPath(signaturePluginName));
    TheoryBuilder builder(manager.getActiveLogicalSystem(),
                          name.data(),
                          description.data(),
                          signature);

    cout << endl
         << "Do you need to setup the signature plugin?(Y/N)" << endl;
    string command;
    getline(cin, command);
    if(command == "Y")
    {
        //Setup signature Plugin
    }

    setupAxioms(builder);
    manager.createTheory(builder, TheoryPluginsRecord()); //TODO
}

void setupSignature(QString &signaturePluginName)
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

            checkSignatureExists(wrappedName);
            signaturePluginName = wrappedName;
            return;
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

void checkSignatureExists(const QString &name)
{
    if(!StorageManager::signaturePluginExists(name))
    {
        QString errorMsg;
        errorMsg += "There is no signature plugin named \"";
        errorMsg += name;
        errorMsg += "\".\n";
        throw invalid_argument(errorMsg.toStdString());
    }
}

void setupAxioms(TheoryBuilder &builder)
{
    cout << endl
         << "Setup theory axioms:" << endl
         << "   add <axiom>         Add axiom" << endl
         << "   remove <axiom>      Remove axiom" << endl
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

            const QString mainCommand = parser.getMainCommand();
            const QStringList options = parser.getOptions();
            const QStringList positionalArgs = parser.getPositionalArgs();

            checkOptionsAdmissibility(options, QStringList());
            checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

            if(mainCommand == "add")
            {
                checkSetupAxiomsPositionalArgs(positionalArgs);
                builder.addAxiom(positionalArgs.first());
            }
            else if(mainCommand == "remove")
            {
                checkSetupAxiomsPositionalArgs(positionalArgs);
                builder.removeAxiom(positionalArgs.first());
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
    checkOptionsAdmissibility(options, QStringList());
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    const QString &theoryName = positionalArgs.first();
    manager.loadTheory(theoryName);

    cout << endl
         << "Theory " << theoryName << " loaded." << endl;

    //Begin theory routine
}

void deleteTheory(const QStringList &options, const QStringList &positionalArgs)
{
    checkOptionsAdmissibility(options, QStringList());
    checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({1}));

    const QString &theoryName = positionalArgs.first();
    manager.removeTheory(theoryName);

    cout << endl
         << "Theory " << theoryName << " deleted." << endl;
}

void listSignaturePlugins()
{
    cout << endl << "Signature plugins list:" << endl;
    cout << endl << StorageManager::signaturePluginsList() << endl;
}

void listInferenceTacticPlugins()
{
    cout << endl << "Inference tactic plugins list:" << endl;
    cout << endl << StorageManager::inferenceTacticsPluginsList() << endl;
}

void listPreProcessorPlugins()
{
    cout << endl << "Pre processor plugins list:" << endl;
    cout << endl << StorageManager::preProcessorPluginsList() << endl;
}

void listPostProcessorsPlugins()
{
    cout << endl << "Post processor plugins list" << endl;
    cout << endl << StorageManager::postProcessorPluginsList() << endl;
}

void theoryMenu()
{

}
