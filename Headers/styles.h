#ifndef STYLES_H
#define STYLES_H

#include <QString>

static const QString BUTTON_UNCHECKED_STYLE =
        (QString)"QPushButton {" +
        (QString)"   background-color: rgb(234, 234, 234);" +
        (QString)"   border: 1px solid  rgb(217, 217, 217);" +
        (QString)"   outline: none;" +
        (QString)"}" +
        (QString)"QPushButton:hover {" +
        (QString)"   background-color: rgb(245, 245, 245);" +
        (QString)"}";
static const QString BUTTON_CHECKED_STYLE =
        (QString)"border: 1px solid  rgb(217, 217, 217);" +
        (QString)"border-right: 0px;" +
        (QString)"outline: none;";
static const QString RAM_EVEN_STYLE =
        (QString)"QLineEdit {" +
        (QString)"   border: 0px;" +
        (QString)"   background-color: rgb(250,250,250);" +
        (QString)"   color: rgb(75,75,75);" +
        (QString)"   font: 10pt;" +
        (QString)"   font-weight: bold;" +
        (QString)"}" +
        (QString)"QLineEdit:focus{" +
        (QString)"   border: 1px solid rgb(100,100,100);" +
        (QString)"}";
static const QString RAM_ODD_STYLE=
        (QString)"QLineEdit {" +
        (QString)"   border: 0px solid;" +
        (QString)"   color: rgb(75,75,75);" +
        (QString)"   font: 10pt;" +
        (QString)"   font-weight: bold;" +
        (QString)"}" +
        (QString)"QLineEdit:focus {" +
        (QString)"   border: 1px solid rgb(100,100,100);" +
        (QString)"}";
static const QString TT_LINE_EDIT_STYLE =
        (QString)"background-color: #f2f2f2;" +
        (QString)"border-radius: 6px;" +
        (QString)"font: 10pt;" +
        (QString)"font-weight: bold;" +
        (QString)"color: #4C5866;";
static const QString TT_LINE_EDIT_ERROR_STYLE =
        (QString)"background-color: #f2f2f2;" +
        (QString)"border-radius: 6px;" +
        (QString)"border: 1px solid #E28383;" +
        (QString)"font: 10pt;" +
        (QString)"font-weight: bold;" +
        (QString)"color: #4C5866;";
static const QString STRING_NUMBER_LABEL_STYLE =
        (QString)"background-color: rgb(101, 220, 177);" +
        (QString)"border-radius: 10px;" +
        (QString)"font: 10pt;" +
        (QString)"font-weight: bold;" +
        (QString)"color: rgb(255, 255, 255);";
static const QString RAM_LABEL_STYLE =
        (QString)"border-radius: 8px;" +
        (QString)"background-color: rgb(87, 161, 217);" +
        (QString)"color: white;" +
        (QString)"font: 10pt;" +
        (QString)"font-weight: bold;";

#define START_NEXT_UNPRESSED_SIZE 51
#define START_NEXT_PRESSED_SIZE 49

#endif // STYLES_H
