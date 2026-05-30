#include "theme.h"

Theme g_theme = Theme::dark();
bool g_isDark = true;

void toggleTheme()
{
    g_isDark = !g_isDark;
    g_theme = g_isDark ? Theme::dark() : Theme::light();
}
