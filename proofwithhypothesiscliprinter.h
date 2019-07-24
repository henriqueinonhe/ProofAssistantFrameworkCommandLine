#ifndef PROOFWITHHYPOTHESISCLIPRINTER_H
#define PROOFWITHHYPOTHESISCLIPRINTER_H

#include "proofprintercliplugin.h"
#include "proofprinter.h"

class ProofWithHypothesisCLIPrinter : public ProofPrinter, public ProofPrinterCLIPlugin
{
public:
    ProofWithHypothesisCLIPrinter() = default;
    ProofWithHypothesisCLIPrinter(QDataStream &stream);
    QString printProof(const StringProcessorManager &postFormatter, const Proof &proof) const override;

protected:
    void serialize(QDataStream &) const override;
    void deserialize(QDataStream &) override;
};

#endif // PROOFWITHHYPOTHESISCLIPRINTER_H
