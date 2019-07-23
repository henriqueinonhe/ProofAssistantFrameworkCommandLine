#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iosfwd>

template <class T> class QVector;
class QString;
class QStringList;
class Formula;
class TheoryBuilder;
class LogicalSystemAssistant;
class TheoryAssistant;
class ProofAssistant;

/* Menu */
void entryMenu();
void logicalSystemMenu(LogicalSystemAssistant &logicalSystemAssistant);
void theoryMenu(TheoryAssistant &theoryAssistant);
void proofAssistantMenu(TheoryAssistant &theoryAssistant, ProofAssistant &assistant);

//Other
void createRootDir();
void invalidCommand(const QString &string);
std::ostream &operator <<(std::ostream &stream, const QString &string);
std::ostream &operator <<(std::ostream &stream, const QStringList &list);
void checkOptionsAdmissibility(const QStringList &options, const QStringList &admissibleOptions);
void checkPositionalArgumentsExpectedNumber(const QStringList &positionalArgs, const QVector<int> &expectedNumbers);
template <class T> QVector<T> filterRecords(const QString &name, const QVector<T> &records);
void createLogicalSystem(const QStringList &options);
QStringList setupInferenceRulesPlugins();
void deleteLogicalSystem(const QStringList &options, const QStringList &positionalArgs);
void loadLogicalSystem(const QStringList &options, const QStringList &positionalArgs);
void checkLogicalSystemExists(const QString &systemName);
void createTheory(LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options);
QString setupSignaturePlugin();
QString setupProofPlugin();
QString setupProofPrinterPlugin();
void checkSignaturePluginExists(const QString &name);
void checkProofPluginExists(const QString &pluginName);
void checkProofPrinterPluginExists(const QString &pluginName);
void setupAxioms(TheoryBuilder &builder);
void checkSetupAxiomsPositionalArgs(const QStringList &positionalArgs);
void loadTheory(LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options, const QStringList &positionalArgs);
void deleteTheory(LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options, const QStringList &positionalArgs);
void theoryMenuList(const QStringList &options, const QStringList &positionalArgs);
void theoryMenuAdd(TheoryAssistant &theoryAssistant, const QStringList &options, const QStringList &positionalArgs);
void createProof(TheoryAssistant &theoryAssistant);
void setupProofPremises(TheoryAssistant &theoryAssistant, QStringList &premises);
void setupProofConclusion(TheoryAssistant &theoryAssistant, QString &conclusion);
void loadProof(TheoryAssistant &theoryAssistant, const QStringList &options, const QStringList &positionalArgs);

/* Listing */

//Core Components
void listLogicalSystems(const QStringList &options, const QStringList &positionalArgs);
void listTheories(const LogicalSystemAssistant &logicalSystemAssistant, const QStringList &options, const QStringList &positionalArgs);
void listCurrentTheoryAxioms(const TheoryAssistant &theoryAssistant);

//Plugins
void outputOrderedList(const QStringList &list);
void outputOrderedPluginList(const QStringList &pluginList);
void listInferenceRulePlugins();
void listSignaturePlugins();
void listProofPlugins();
void listProofPrinterPlugins();
void listInferenceTacticPlugins();
void listPreProcessorPlugins();
void listPostProcessorsPlugins();

//Loaded Plugins
void listCurrentlyLoadedInferenceRulesPlugins(const LogicalSystemAssistant &logicalSystemAssistant);
void listCurrentlyLoadedSignaturePlugin(const LogicalSystemAssistant &logicalSystemAssistant);
void listCurrentlyLoadedProofPlugin(const LogicalSystemAssistant &logicalSystemAssistant);


#endif // FUNCTIONS_H
