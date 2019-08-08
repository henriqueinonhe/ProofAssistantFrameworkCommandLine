#include "cliauxiliaryfunctions.h"
#include <QStringList>
#include <QString>
#include <QVector>
#include <exception>

namespace CliAuxiliaryFunctions
{

using namespace std;

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

}
