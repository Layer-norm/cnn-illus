#ifndef THEME_H
#define THEME_H

#include <QString>

struct Theme {
    // Backgrounds
    QString windowBg;
    QString baseBg;
    QString panelBg;
    QString border;
    // Text
    QString text;
    QString mutedText;    // #64748b
    QString labelText;
    // Accent
    QString accent;       // #3b82f6
    QString cyan;
    // Extra
    QString paddingBg;    // padded cell bg in convolution
    QString titleText;
    QString stepInfoColor;
    bool isDark;

    static Theme dark() {
        Theme t;
        t.windowBg    = "#0a0e1a";
        t.baseBg      = "#111827";
        t.panelBg     = "#1a2035";
        t.border      = "#232b42";
        t.text        = "#e2e8f0";
        t.mutedText   = "#64748b";
        t.labelText   = "#94a3b8";
        t.accent      = "#3b82f6";
        t.cyan        = "#06b6d4";
        t.paddingBg   = "#1a2035";
        t.titleText   = "#e2e8f0";
        t.stepInfoColor = "#06b6d4";
        t.isDark      = true;
        return t;
    }

    static Theme light() {
        Theme t;
        t.windowBg    = "#f8fafc";
        t.baseBg      = "#ffffff";
        t.panelBg     = "#f1f5f9";
        t.border      = "#e2e8f0";
        t.text        = "#1e293b";
        t.mutedText   = "#64748b";
        t.labelText   = "#475569";
        t.accent      = "#2563eb";
        t.cyan        = "#0891b2";
        t.paddingBg   = "#f1f5f9";
        t.titleText   = "#0f172a";
        t.stepInfoColor = "#0891b2";
        t.isDark      = false;
        return t;
    }

    // StyleSheet helpers
    QString labelStyle() const {
        return QString("font-size: 10px; font-weight: bold; color: %1;").arg(labelText);
    }
    QString descStyle() const {
        return QString("font-size: 11px; color: %1;").arg(mutedText);
    }
    QString titleStyle() const {
        return QString("font-size: 15px; font-weight: bold; color: %1;").arg(titleText);
    }
    QString ctrlLabelStyle() const {
        return QString("color: %1; font-size: 11px;").arg(labelText);
    }
    QString panelStyle() const {
        return QString("QFrame { background-color: %1; border: 1px solid %2; border-radius: 8px; }").arg(panelBg, border);
    }
    QString cardStyle() const {
        return QString("background-color: %1; border: 1px solid %2; border-radius: 8px;").arg(baseBg, border);
    }
    QString btnPrimStyle() const {
        return "QPushButton { background-color: #3b82f6; color: white; border: none; "
               "border-radius: 4px; padding: 6px 14px; font-weight: bold; font-size: 11px; }"
               "QPushButton:hover { background-color: #6366f1; }";
    }
    QString btnSecStyle() const {
        return QString("QPushButton { background-color: %1; color: %2; border: none; "
                       "border-radius: 4px; padding: 6px 14px; font-size: 11px; }"
                       "QPushButton:hover { background-color: %3; }")
               .arg(border, text, panelBg);
    }
    QString comboStyle() const {
        return QString(
            "QComboBox { background: %1; color: %2; border: 1px solid %3; "
            "border-radius: 4px; padding: 3px 6px; font-size: 11px; }"
            "QComboBox::drop-down { border: none; }"
            "QComboBox QAbstractItemView { background: %1; color: %2; selection-background-color: #3b82f6; }"
        ).arg(border, text, panelBg);
    }
    QString cellBorderStyle() const {
        return QString("border: 1px solid %1; border-radius: 3px;").arg(border);
    }
    QString paddingCellStyle() const {
        return QString("background-color: %1; border: 1px dashed %2; border-radius: 3px;").arg(paddingBg, mutedText);
    }
};

extern Theme g_theme;
extern bool g_isDark;
void toggleTheme();

#endif // THEME_H
