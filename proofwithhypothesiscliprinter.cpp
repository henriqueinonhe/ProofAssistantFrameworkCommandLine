#include "proofwithhypothesiscliprinter.h"
#include "proofwithhypothesis.h"
#include "lineofproof.h"
#include "stringprocessormanager.h"

ProofWithHypothesisCLIPrinter::ProofWithHypothesisCLIPrinter(QDataStream &)
{

}

QString ProofWithHypothesisCLIPrinter::printProof(const StringProcessorManager &postFormatter, const Proof &proof) const
{
    const auto castProof = dynamic_cast<const ProofWithHypothesis &>(proof);
    QString output;
    auto lineNumber = 1;
    const auto linesOfProof = proof.getLinesOfProof();
    const auto lastLineNumber = linesOfProof.size();
    for(const auto lineOfProof : linesOfProof)
    {
        output += QString::number(lineNumber);
        output += ". ";
        output += getSpacingBeforeFormula(lastLineNumber, lineNumber);
        const auto hypothesisDepth = castProof.accessHypothesisManager().hypothesisDepth(lineNumber);
        for(unsigned int hypothesisDepthCounter = 0; hypothesisDepthCounter < hypothesisDepth; hypothesisDepthCounter++)
        {
            output += "|";
        }
        output += postFormatter.format(lineOfProof->getFormula().formattedString());
        output += " ";
        output += lineOfProof->getJustification().getInferenceRuleCallCommand();
        output += ",";
        const auto arguments = lineOfProof->getJustification().getArgumentList();
        for(const auto argument : arguments)
        {
            output += argument;
            output += " ";
        }
        output += "\n";
        lineNumber++;
    }
    return output;
}

void ProofWithHypothesisCLIPrinter::serialize(QDataStream &) const
{

}

void ProofWithHypothesisCLIPrinter::deserialize(QDataStream &)
{

}

QString ProofWithHypothesisCLIPrinter::getSpacingBeforeFormula(const unsigned int lastLineNumber, const unsigned int currentLineNumber) const
{
    const auto stringnizedLastLineNumber = QString::number(lastLineNumber);
    const auto stringnizedCurrentLineNumber = QString::number(currentLineNumber);
    const auto spaceDifference = stringnizedLastLineNumber.size() - stringnizedCurrentLineNumber.size();
    QString spacing;
    for(auto i = 0; i < spaceDifference; i++)
    {
        spacing += " ";
    }
    return spacing;
}
