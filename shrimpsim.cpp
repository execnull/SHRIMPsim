#include "qshrimp.h"
#include "modelshrimp.h"

#include <QDebug>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineHelpRequested,
    CommandLineVersionRequested
};

/*! \brief Parse the command line
 */
CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *errorMessage)
{
    //Load the options
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    //gui option
    QCommandLineOption guiSetOption(QStringList() << "gui",
                                    "Create a demonstrative GUI. Ignores CLI options");
    parser.addOption(guiSetOption);

    //self training
    QCommandLineOption pbSetOption(QStringList() << "pb",
                                   "Dir/Filelist/json for the Pattern Bank", "patternbank");
    QCommandLineOption npattSetOption(QStringList() << "npatt",
                                      "Number of patterns for self-training: 0 - 512", "npatt", "50");
    QCommandLineOption bwidthSetOption(QStringList() << "bwidth",
                                       "Number of patterns for self-training: 0 - 1", "bwidth", "0.05");
    QCommandLineOption thrSetOption(QStringList() << "thr",
                                    "Number of patterns for self-training: 1/3 values", "thr", "150");
    parser.addOption(pbSetOption);
    parser.addOption(npattSetOption);
    parser.addOption(bwidthSetOption);
    parser.addOption(thrSetOption);

    //patternbank
    QCommandLineOption savePBSetOption(QStringList() << "savepb",
                                       "Save Pattern Bank", "pbname", "PB");
    QCommandLineOption saveResultSetOption(QStringList() << "saveres",
                                           "Save Results Path", "path", ".");

    parser.addOption(savePBSetOption);
    parser.addOption(saveResultSetOption);

    //pb and input
    parser.addPositionalArgument("input", "Files to be processed","Files to be processed");

    //Options not recognized error
    if (!parser.parse(QApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    //help requested
    if (parser.isSet(helpOption))
        return CommandLineHelpRequested;

    //version requested
    if (parser.isSet(versionOption))
        return CommandLineVersionRequested;

    //Generation && Self-training value checking
    if (parser.values("thr").size() != 1 && parser.values("thr").size() != 3) {
        *errorMessage = "Wrong number of thresholds specified. Must be 1 or 3.";
        return CommandLineError;
    }

    //check for validity of the number of patterns to be saved
    if (parser.value("npatt").toInt() < 1 || parser.value("npatt").toInt() > 512) {
        *errorMessage = "For 1 thr, npatt must be 1 - 512";
        return CommandLineError;
    }

    //check bandwidth validity
    if (parser.value("bwidth").toInt() < 0 || parser.value("bwidth").toInt() > 1) {
        *errorMessage = "Bandidth value must be between 0 and 1. Default is 0.05";
        return CommandLineError;
    }

    //check list of images
    const QStringList args = parser.positionalArguments();
    if (args.size() == 0 && !parser.isSet("gui") && !parser.isSet("savepb")) {
        *errorMessage = "To process an image, an image must be provided, abort";
        return CommandLineError;
    }

    for (auto arg: args) {
        QFileInfo fileinfo = arg;
        if (!fileinfo.exists()) {
            *errorMessage = "The file " + arg + " does not exist, aborting";
            return CommandLineError;
        }
        else if (!fileinfo.isReadable()) {
            *errorMessage = "Dude, " + arg + " is not readable, aborting";
            return CommandLineError;
        }
    }

    //save the resultss
    if(!QDir(parser.value("saveres")).exists())
        QDir().mkpath(parser.value("saveres"));

    if(QFileInfo(parser.value("saveres")).isFile()) {
        *errorMessage = "Save location " + parser.value("saveres") + " is not a directory, aborting";
        return CommandLineError;
    }
    if(!QFileInfo(parser.value("saveres")).isWritable()) {
        *errorMessage = "Save location " + parser.value("saveres") + " is not writable, aborting";
        return CommandLineError;
    }

    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("qSHRIMP");
    QApplication::setApplicationVersion("0.1");

    //Command line parsing
    QCommandLineParser parser;
    parser.setApplicationDescription("Simulating the Human bRain for IMage Processing");
    QString errorMessage;

    ModelShrimp ms; ///The model that is controlling everything
    qshrimp gui(&ms); ///The graphical user interface for the model

    //Here we parse the command line and then examine the error message.
    //The later is filled by the command line parser. It's different possibilities
    //are examined here
    switch (parseCommandLine(parser, &errorMessage)) {
    case CommandLineError: //The command line could not be parsed correctly
        fputs("qShrimp error: ", stderr);
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n", stderr);
        //fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    case CommandLineHelpRequested: //user added the --help flag, all other options are ignored
        parser.showHelp();
        Q_UNREACHABLE();
    case CommandLineVersionRequested: //user added the --version flag, all other options are ignored
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
        return 0;
    case CommandLineOk: //somehow, it worked!
        break;
    }

    //examine if the user requested the gui. If no, the model loads the settings,
    //loads the Pattern Bank and then executes the simulation
    if (!parser.isSet("gui")) {
        ms.parseSettings(parser);
        ms.runMS();
        return 0;
    }
    else { //load the GUI
        gui.show(); //hic svnt dracones
        return a.exec();
    }

    return 0;
}
