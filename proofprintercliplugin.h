#ifndef PROOFPRINTERCLIPLUGIN_H
#define PROOFPRINTERCLIPLUGIN_H

class QString;
class StringProcessorManager;
class Proof;

class ProofPrinterCLIPlugin
{
public:
    ProofPrinterCLIPlugin();

    virtual QString printProof(const StringProcessorManager &postFormatter, const Proof &proof) const = 0;
};

#endif // PROOFPRINTERCLIPLUGIN_H
