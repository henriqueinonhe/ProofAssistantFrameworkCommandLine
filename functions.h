#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iosfwd>

template <class T> class QVector;
class QString;
class QStringList;
class Formula;
class TheoryBuilder;
class ProofAssistant;

/* Menu */
void entryMenu();
void logicalSystemMenu();
void theoryMenu();
void proofAssistantMenu(ProofAssistant &assistant);

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
void createTheory(const QStringList &options);
QString setupSignaturePlugin();
QString setupProofPlugin();
void checkSignaturePluginExists(const QString &name);
void checkProofPluginExists(const QString &pluginName);
void setupAxioms(TheoryBuilder &builder);
void checkSetupAxiomsPositionalArgs(const QStringList &positionalArgs);
void loadTheory(const QStringList &options, const QStringList &positionalArgs);
void deleteTheory(const QStringList &options, const QStringList &positionalArgs);
void theoryMenuList(const QStringList &options, const QStringList &positionalArgs);
void theoryMenuAdd(const QStringList &options, const QStringList &positionalArgs);
void createProof();
void setupProofPremises(QStringList &premises);
void setupProofConclusion(QString &conclusion);
void loadProof(const QStringList &options, const QStringList &positionalArgs);

/* Listing */

//Core Components
void listLogicalSystems(const QStringList &options, const QStringList &positionalArgs);
void listTheories(const QStringList &options, const QStringList &positionalArgs);
void listCurrentTheoryAxioms();

//Plugins
void outputOrderedList(const QStringList &list);
void outputOrderedPluginList(const QStringList &pluginList);
void listInferenceRulePlugins();
void listSignaturePlugins();
void listProofPlugins();
void listInferenceTacticPlugins();
void listPreProcessorPlugins();
void listPostProcessorsPlugins();

//Loaded Plugins
void listCurrentlyLoadedInferenceRulesPlugins();
void listCurrentlyLoadedSignaturePlugin();
void listCurrentlyLoadedProofPlugin();


#endif // FUNCTIONS_H
