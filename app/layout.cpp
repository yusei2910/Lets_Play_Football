#include "layout.h"

Layout::Layout() // default constructor
{

}

Layout::~Layout() // destructor
{
    qDebug() << "Layout has been closed";
}

Layout* Layout::instance() // Create one and only one instance of the database
{
    static Layout instance;
    return &instance;
}

void Layout::ImportResources() // imports external assets
{
    QFontDatabase::addApplicationFont(":/resource/fonts/OldSportAthletic.ttf");
    QFontDatabase::addApplicationFont(":/resource/fonts/OldSansBlack.ttf");
    QFontDatabase::addApplicationFont(":/resource/fonts/Tommy Regular.otf");
}
