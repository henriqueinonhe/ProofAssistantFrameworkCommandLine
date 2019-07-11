#ifndef STRINGPROCESSORCLIPLUGIN_H
#define STRINGPROCESSORCLIPLUGIN_H

#include "stringprocessor.h"

class StringProcessorCLIPlugin : public StringProcessor
{
public:
    StringProcessorCLIPlugin();

    // StringProcessor interface
public:
    virtual void setup() = 0;
};

#endif // STRINGPROCESSORCLIPLUGIN_H