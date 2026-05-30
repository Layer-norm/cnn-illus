#include "theme.h"

Theme g_theme = Theme::light();
bool g_isDark = false;

void toggleTheme()
{
    g_isDark = !g_isDark;
    g_theme = g_isDark ? Theme::dark() : Theme::light();
}
