#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iosfwd>

template <class T> class QVector;
class QString;
class QStringList;
class Formula;
class TheoryBuilder;

//Menus
void entryMenu();
void logicalSystemMenu();
void theoryMenu();

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
void createLogicalSystem();
void setupInferenceRules(QStringList&);
void deleteLogicalSystem(const QStringList &options, const QStringList &positionalArgs);
void loadLogicalSystem(const QStringList &options, const QStringList &positionalArgs);
void checkLogicalSystemExists(const QString &systemName);
void listTheories(const QStringList &options, const QStringList &positionalArgs);
void createTheory();
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

#endif // FUNCTIONS_H
