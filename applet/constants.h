#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QObject>

#define NUM_SLOTS 10
#define ALREADY_DOCKED -1
#define SLOTS_FULL -2

class Constants : public QObject
{
    // Q_OBJECT

public:
    static const QString APP_NAME;
    static const QString ORG_NAME;
    static const QString DOM_NAME;
    static const QString WEBSITE;

    static const QString APP_VERSION;

/*    static const char *OPTIONSTRING;

    static const QString ABOUT_MESSAGE;
*/
    explicit Constants(QObject *parent = nullptr);

signals:

};

#endif // CONSTANTS_H
