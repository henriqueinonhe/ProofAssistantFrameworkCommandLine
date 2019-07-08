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
                 << "   rules          List inference rules" << endl
                 << "   quit           Quits" << endl << endl;

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
        stream << string << endl;
    }
    stream << endl;
    return stream;
}

void listInferenceRulePlugins()
{
    cout << endl << "Inferece Rule Plugins:" << endl;
    cout << StorageManager::inferenceRulesPluginsList();
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
    cout << "Choose inference rule plugins:" << endl;
    cout << "   add <name>          Adds inference rule." << endl;
    cout << "   remove <name>       Removes inference rule." << endl;
    cout << "   done                Finishes list." << endl;

    while(true)
    {
        try
        {
            cout << endl;
            listInferenceRulePlugins();
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

    if(options.contains("h") || options.contains("help"))
    {
        cout << endl;
        cout << "usage: list [<options>] [system-name]" << endl;
        cout << "   -s, --silent            Deletes system name silently" << endl;
        return;
    }

    const QString systemName = positionalArgs.first();
    checkLogicalSystemExists(systemName);

    if(options.contains("s") || options.contains("silent"))
    {
        manager.removeLogicalSystem(systemName);
        cout << endl;
        cout << "\"" + systemName + "\" removed!" << endl;
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
                 << "   signature         List signature plugins" << endl
                 << "   tactic            List inference tactic plugins" << endl
                 << "   pre               List pre processor plugins" << endl
                 << "   post              List post processor plugins" << endl
                 << "   unload            Unload logical system" << endl << endl;

            string command;
            getline(cin, command);
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
                theoryMenu();
            }
            else if(mainCommand == "delete")
            {
                deleteTheory(options, positionalArgs);
            }
            else if(mainCommand == "rules")
            {
                listCurrentlyLoadedInferenceRules(QStringList({}));
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

    cout << endl;
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
        //Setup signature Plugin //TODO
        //This probably should be done automatically using the plugin itself
    }

    setupAxioms(builder);
    manager.createTheory(builder, TheoryPluginsRecord(signaturePluginName));
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

    const QString &theoryName = positionalArgs.first();
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
            cout << "unload         Unload theory" << endl;

            string command;
            getline(cin, command);
            parser.parse(command);
            QString mainCommand = parser.getMainCommand();
            QStringList options = parser.getOptions();
            QStringList positionalArgs = parser.getPositionalArgs();

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
        listCurrentlyLoadedInferenceRules(options);
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

void listCurrentlyLoadedInferenceRules(const QStringList &options)
{
    const LogicalSystem * const activeSystem = manager.getActiveLogicalSystem();
    const auto rules = activeSystem->getInferenceRules();
    cout << endl;
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
    const Theory * const currentTheory = manager.getActiveTheory();
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

    cout << endl;
    cout << "Enter proof description:" << endl;

    getline(cin, command);
    const QString description(command.data());

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
    const Theory * const theory = manager.getActiveTheory();
    Parser formulaParser(theory->getSignature().get(), manager.getActiveLogicalSystem()->getWffType());
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
            const QString mainCommand = parser.getMainCommand();
            const QStringList options = parser.getOptions();
            const QStringList positionalArgs = parser.getPositionalArgs();

            checkOptionsAdmissibility(options, QStringList({}));
            checkPositionalArgumentsExpectedNumber(positionalArgs, QVector<int>({0, 1}));

            if(mainCommand == "add")
            {
                const QString premiss = positionalArgs.first();
                formulaParser.parse(premiss);
                premises << premiss;
            }
            else if(mainCommand == "remove")
            {
                const QString premiss = positionalArgs.first();
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
    const Theory * const theory = manager.getActiveTheory();
    Parser formulaParser(theory->getSignature().get(), manager.getActiveLogicalSystem()->getWffType());
    while(true)
    {
        string command;
        getline(cin, command);
        QString formula = command.data();

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
        const unsigned int id = positionalArgs.first().toUInt();
        ProofAssistant assistant = manager.loadProof(id);
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
        const Proof proof = assistant.getProof();
        const unsigned int linesOfProofSize = proof.getLinesOfProof().size();
        for(unsigned int index = 0; index < linesOfProofSize; index++)
        {
            const unsigned int lineNumber = index + 1;
            cout << lineNumber << ". "
                 << proof.getLineOfProof(lineNumber).getFormula().formattedString() << " "
                 << proof.getLineOfProof(lineNumber).getJustification().getInferenceRuleCallCommand() << " "
                 << proof.getLineOfProof(lineNumber).getJustification().getArgumentList();
        }

        try
        {
            string command;
            getline(cin, command);
            parser.parse(command);
            const QString mainCommand = parser.getMainCommand();
            const QStringList options = parser.getOptions();
            const QStringList positionalArgs = parser.getPositionalArgs();

            if(mainCommand == "call")
            {
                const QString callCommand = positionalArgs.first();
                const QStringList argumentList = positionalArgs.mid(1);
                assistant.applyInferenceRule(callCommand, argumentList);
            }
        }
        catch(const invalid_argument &e)
        {
            cerr << endl << e.what() << endl;
        }
    }
}
