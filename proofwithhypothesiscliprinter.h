#ifndef PROOFWITHHYPOTHESISCLIPRINTER_H
#define PROOFWITHHYPOTHESISCLIPRINTER_H

#include "proofprintercliplugin.h"
#include "proofprinter.h"

class ProofWithHypothesisCLIPrinter : public ProofPrinter, public ProofPrinterCLIPlugin
{
public:
    ProofWithHypothesisCLIPrinter() = default;
    QString printProof(const StringProcessorManager &postFormatter, const Proof &proof) override;
};

#endif // PROOFWITHHYPOTHESISCLIPRINTER_H
