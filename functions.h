#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iosfwd>

template <class T> class QVector;
class QString;
class QStringList;
class Formula;
class TheoryBuilder;
class ProofAssistant;

//Menus
void entryMenu();
void logicalSystemMenu();
void theoryMenu();
void proofAssistantMenu(ProofAssistant &assistant);

//Other
void createRootDir();
void invalidCommand(const QString &string);
std::ostream &operator <<(std::ostream &stream, const QString &string);
std::ostream &operator <<(std::ostream &stream, const QStringList &list);
void listInferenceRulePlugins();
void listLogicalSystems(const QStringList &options, const QStringList &positionalArgs);
void checkOptionsAdmissibility(const QStringList &options, const QStringList &admissibleOptions);
void checkPositionalArgumentsExpectedNumber(const QStringList &positionalArgs, const QVector<int> &expectedNumbers);
template <class T> QVector<T> filterRecords(const QString &name, const QVector<T> &records);
void createLogicalSystem(const QStringList &options);
void setupInferenceRules(QStringList&);
void deleteLogicalSystem(const QStringList &options, const QStringList &positionalArgs);
void loadLogicalSystem(const QStringList &options, const QStringList &positionalArgs);
void checkLogicalSystemExists(const QString &systemName);
void listTheories(const QStringList &options, const QStringList &positionalArgs);
void createTheory(const QStringList &options);
void setupSignature(QString &signaturePluginName);
void checkSignatureExists(const QString &name);
void setupAxioms(TheoryBuilder &builder);
void checkSetupAxiomsPositionalArgs(const QStringList &positionalArgs);
void loadTheory(const QStringList &options, const QStringList &positionalArgs);
void deleteTheory(const QStringList &options, const QStringList &positionalArgs);
void listSignaturePlugins();
void listInferenceTacticPlugins();
void listPreProcessorPlugins();
void listPostProcessorsPlugins();
void theoryMenuList(const QStringList &options, const QStringList &positionalArgs);
void listCurrentlyLoadedInferenceRules(const QStringList &options);
void listCurrentTheoryAxioms();
void theoryMenuAdd(const QStringList &options, const QStringList &positionalArgs);
void createProof();
void setupProofPremises(QStringList &premises);
void setupProofConclusion(QString &conclusion);
void loadProof(const QStringList &options, const QStringList &positionalArgs);

#endif // FUNCTIONS_H
