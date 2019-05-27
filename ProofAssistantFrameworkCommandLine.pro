QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../ProofAssistantFramework \
               ../Language/Parser \
               ../Language/Type \
               ../Language/Utils \
               ../Language/Processors \
               ../Language/PreProcessors \
               ../Language/PostProcessors \
               ../Language/Token \
               ../CommandLineParser

SOURCES += \
        ../CommandLineParser/commandlineparser.cpp \
        ../Language/Parser/formula.cpp \
        ../Language/Parser/lexer.cpp \
        ../Language/Parser/parser.cpp \
        ../Language/Parser/parsingtree.cpp \
        ../Language/Parser/parsingtreeconstiterator.cpp \
        ../Language/Parser/parsingtreeiterator.cpp \
        ../Language/Parser/parsingtreenode.cpp \
        ../Language/Parser/signature.cpp \
        ../Language/Parser/tablesignature.cpp \
        ../Language/Processors/PostProcessors/basicpostprocessor.cpp \
        ../Language/Processors/PreProcessors/basicpreprocessor.cpp \
        ../Language/Processors/PreProcessors/classicfunctionnotationpreprocessor.cpp \
        ../Language/Processors/basicprocessor.cpp \
        ../Language/Processors/basicprocessortokenrecord.cpp \
        ../Language/Processors/formatter.cpp \
        ../Language/Processors/stringprocessor.cpp \
        ../Language/Token/bindingrecord.cpp \
        ../Language/Token/bindingtoken.cpp \
        ../Language/Token/coretoken.cpp \
        ../Language/Token/punctuationtoken.cpp \
        ../Language/Token/token.cpp \
        ../Language/Token/tokenstring.cpp \
        ../Language/Token/variabletoken.cpp \
        ../Language/Type/type.cpp \
        ../Language/Type/typeparser.cpp \
        ../Language/Type/typeparsingtree.cpp \
        ../Language/Type/typeparsingtreeiterator.cpp \
        ../Language/Type/typeparsingtreenode.cpp \
        ../Language/Type/typetoken.cpp \
        ../Language/Type/typetokenstring.cpp \
        ../Language/Utils/dirtyfix.cpp \
        ../ProofAssistantFramework/inferenceprocedure.cpp \
        ../ProofAssistantFramework/inferencerule.cpp \
        ../ProofAssistantFramework/inferencetactic.cpp \
        ../ProofAssistantFramework/justification.cpp \
        ../ProofAssistantFramework/lineofproof.cpp \
        ../ProofAssistantFramework/lineofproofsection.cpp \
        ../ProofAssistantFramework/lineofproofsectionmanager.cpp \
        ../ProofAssistantFramework/logicalsystem.cpp \
        ../ProofAssistantFramework/logicalsystemrecord.cpp \
        ../ProofAssistantFramework/logosprogrammanager.cpp \
        ../ProofAssistantFramework/pluginmanager.cpp \
        ../ProofAssistantFramework/programmanager.cpp \
        ../ProofAssistantFramework/proof.cpp \
        ../ProofAssistantFramework/proofassistant.cpp \
        ../ProofAssistantFramework/prooflinks.cpp \
        ../ProofAssistantFramework/proofrecord.cpp \
        ../ProofAssistantFramework/signatureplugin.cpp \
        ../ProofAssistantFramework/storagemanager.cpp \
        ../ProofAssistantFramework/stringprocessorplugin.cpp \
        ../ProofAssistantFramework/theory.cpp \
        ../ProofAssistantFramework/theorybuilder.cpp \
        ../ProofAssistantFramework/theorypluginsrecord.cpp \
        ../ProofAssistantFramework/theoryrecord.cpp \
        functions.cpp \
        main.cpp \
        tests.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../CommandLineParser/commandlineparser.h \
    ../Language/Parser/formula.h \
    ../Language/Parser/lexer.h \
    ../Language/Parser/parser.h \
    ../Language/Parser/parsingerrorexception.hpp \
    ../Language/Parser/parsingtree.h \
    ../Language/Parser/parsingtreeconstiterator.h \
    ../Language/Parser/parsingtreeiterator.h \
    ../Language/Parser/parsingtreenode.h \
    ../Language/Parser/signature.h \
    ../Language/Parser/tablesignature.h \
    ../Language/Processors/PostProcessors/basicpostprocessor.h \
    ../Language/Processors/PreProcessors/basicpreprocessor.h \
    ../Language/Processors/PreProcessors/classicfunctionnotationpreprocessor.h \
    ../Language/Processors/basicprocessor.h \
    ../Language/Processors/basicprocessortokenrecord.h \
    ../Language/Processors/formatter.h \
    ../Language/Processors/stringprocessor.h \
    ../Language/Token/bindingrecord.h \
    ../Language/Token/bindingtoken.h \
    ../Language/Token/coretoken.h \
    ../Language/Token/punctuationtoken.h \
    ../Language/Token/token.h \
    ../Language/Token/tokenstring.h \
    ../Language/Token/variabletoken.h \
    ../Language/Type/type.h \
    ../Language/Type/typeparser.h \
    ../Language/Type/typeparsingerrorexception.h \
    ../Language/Type/typeparsingtree.h \
    ../Language/Type/typeparsingtreeiterator.h \
    ../Language/Type/typeparsingtreenode.h \
    ../Language/Type/typetoken.h \
    ../Language/Type/typetokenstring.h \
    ../Language/Utils/containerauxiliarytools.h \
    ../Language/Utils/dirtyfix.h \
    ../Language/Utils/parsingauxiliarytools.h \
    ../Language/Utils/pool.h \
    ../ProofAssistantFramework/dummyclasses.h \
    ../ProofAssistantFramework/inferenceprocedure.h \
    ../ProofAssistantFramework/inferencerule.h \
    ../ProofAssistantFramework/inferenceruleplugin.h \
    ../ProofAssistantFramework/inferencetactic.h \
    ../ProofAssistantFramework/inferencetacticplugin.h \
    ../ProofAssistantFramework/justification.h \
    ../ProofAssistantFramework/lineofproof.h \
    ../ProofAssistantFramework/lineofproofsection.h \
    ../ProofAssistantFramework/lineofproofsectionmanager.h \
    ../ProofAssistantFramework/logicalsystem.h \
    ../ProofAssistantFramework/logicalsystemrecord.h \
    ../ProofAssistantFramework/logosprogrammanager.h \
    ../ProofAssistantFramework/pluginfactoryinterface.h \
    ../ProofAssistantFramework/pluginmanager.h \
    ../ProofAssistantFramework/programmanager.h \
    ../ProofAssistantFramework/proof.h \
    ../ProofAssistantFramework/proofassistant.h \
    ../ProofAssistantFramework/prooflinks.h \
    ../ProofAssistantFramework/proofrecord.h \
    ../ProofAssistantFramework/signatureplugin.h \
    ../ProofAssistantFramework/smartpointersserialization.h \
    ../ProofAssistantFramework/storagemanager.h \
    ../ProofAssistantFramework/stringprocessorplugin.h \
    ../ProofAssistantFramework/theory.h \
    ../ProofAssistantFramework/theorybuilder.h \
    ../ProofAssistantFramework/theorypluginsrecord.h \
    ../ProofAssistantFramework/theoryrecord.h \
    catch.hpp \
    functions.h
