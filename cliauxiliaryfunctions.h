#ifndef CLIAUXILIARYFUNCTIONS_H
#define CLIAUXILIARYFUNCTIONS_H

class QStringList;
class QString;
template <class> class QVector;

namespace CliAuxiliaryFunctions
{
    void checkOptionsAdmissibility(const QStringList &options, const QStringList &admissibleOptions);
    void checkPositionalArgumentsExpectedNumber(const QStringList &positionalArgs, const QVector<int> &expectedNumbers);
}


#endif // CLIAUXILIARYFUNCTIONS_H
