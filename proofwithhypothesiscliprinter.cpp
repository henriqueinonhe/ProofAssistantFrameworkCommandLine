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
    for(const auto lineOfProof : linesOfProof)
    {
        output += lineNumber;
        output += ". ";
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
    }
    return output;
}

void ProofWithHypothesisCLIPrinter::serialize(QDataStream &) const
{

}

void ProofWithHypothesisCLIPrinter::deserialize(QDataStream &)
{

}
