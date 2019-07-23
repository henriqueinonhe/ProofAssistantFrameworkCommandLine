#include "proofwithhypothesiscliprinter.h"
#include "proofwithhypothesis.h"
#include "lineofproof.h"

QString ProofWithHypothesisCLIPrinter::printProof(const StringProcessorManager &postFormatter, const Proof &proof)
{
    const auto castProof = dynamic_cast<const ProofWithHypothesis &>(proof);
    QString output;
    unsigned int index = 1;
    const auto linesOfProof = proof.getLinesOfProof();
    for(const auto lineOfProof : linesOfProof)
    {
        output += index;
        output += ". ";
        output += proof.
    }
}
