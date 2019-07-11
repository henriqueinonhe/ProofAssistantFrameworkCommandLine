#ifndef SIGNATURECLIPLUGIN_H
#define SIGNATURECLIPLUGIN_H

#include "signature.h"

class SignatureCLIPlugin : public Signature
{
public:
    SignatureCLIPlugin();
    virtual void setup() = 0;
};

#endif // SIGNATURECLIPLUGIN_H
