#include "myqsettings.h"

static const char g_ini_filename[] = "Information.ini";

MyQSettings::MyQSettings() : QSettings(g_ini_filename, QSettings::IniFormat)
{

}
