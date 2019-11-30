#ifndef CONFIG_H
#define CONFIG_H
#include <QDate>
#include <QLocale>

static const QDate buildDate = QLocale( QLocale::English ).toDate( QString( __DATE__ ).replace( "  ", " 0" ), "MMM dd yyyy");
static const QTime buildTime = QTime::fromString( __TIME__, "hh:mm:ss" );

#endif // CONFIG_H
