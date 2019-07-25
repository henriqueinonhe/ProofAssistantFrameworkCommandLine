#ifndef PROOFWITHHYPOTHESISCLIPRINTER_H
#define PROOFWITHHYPOTHESISCLIPRINTER_H

#include "proofprintercliplugin.h"
#include "proofprinter.h"

class ProofWithHypothesisCLIPrinter final : public ProofPrinter, public ProofPrinterCLIPlugin
{
public:
    ProofWithHypothesisCLIPrinter() = default;
    ProofWithHypothesisCLIPrinter(QDataStream &stream);
    QString printProof(const StringProcessorManager &postFormatter, const Proof &proof) const override;

protected:
    void serialize(QDataStream &) const override;
    void deserialize(QDataStream &) override;

private:
    QString getSpacingBeforeFormula(const unsigned int lastLineNumber, const unsigned int currentLineNumber) const;
};

#endif // PROOFWITHHYPOTHESISCLIPRINTER_H
