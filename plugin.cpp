#include "plugin.h"

#include "constants.h"
#include "tabbar.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <utils/stylehelper.h>
#include <utils/theme/theme.h>
#include <utils/stylehelper.h>

#include <QBoxLayout>
#include <QFile>
#include <QMainWindow>

using namespace TabbedEditor::Internal;

TabbedEditorPlugin::TabbedEditorPlugin() :
    m_tabBar(nullptr),
    m_styleUpdatedToBaseColor(false)
{
}

bool TabbedEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    connect(Core::EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)), SLOT(showTabBar()));

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(Core::ICore::mainWindow());
    mainWindow->layout()->setSpacing(0);

    QWidget *wrapper = new QWidget(mainWindow);
    wrapper->setMinimumHeight(0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabBar = new TabBar();
    layout->addWidget(m_tabBar);
    layout->addWidget(mainWindow->centralWidget());

    wrapper->setLayout(layout);

    mainWindow->setCentralWidget(wrapper);

    return true;
}

QString TabbedEditorPlugin::getStylesheetPatternFromFile(const QString &filepath)
{
    QFile stylesheetFile(filepath);
    if (!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(stylesheetFile.readAll());
}

void TabbedEditorPlugin::updateStyleToBaseColor()
{
    Utils::Theme *theme = Utils::creatorTheme();

    auto tabbarBackgroundColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyTabBarSelectedBackgroundColor));
    auto tabbarBorderColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyToolBarSeparatorColor));

    auto tabBackgroundColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyToolBarSeparatorColor));
    auto tabBorderColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyToolBarSeparatorColor));
    auto tabTextColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyTabWidgetEnabledUnselectedTextColor));

    auto selectedTabBackgroundColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyTabBarSelectedBackgroundColor));
    auto selectedTabBorderColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyToolBarSeparatorColor));
    auto selectedTabTextColorQss = getQssStringFromColor(
        theme->color(Utils::Theme::FancyTabWidgetEnabledSelectedTextColor));

    QString stylesheetPattern = getStylesheetPatternFromFile(QStringLiteral(":/styles/default.qss"));

    stylesheetPattern = stylesheetPattern.replace("%TABBAR_BACKGROUND_COLOR%", tabbarBackgroundColorQss);
    stylesheetPattern = stylesheetPattern.replace("%TABBAR_BORDER_COLOR%", tabbarBorderColorQss);

    stylesheetPattern = stylesheetPattern.replace("%TAB_BACKGROUND_COLOR%", tabBackgroundColorQss);
    stylesheetPattern = stylesheetPattern.replace("%TAB_BORDER_COLOR%", tabBorderColorQss);
    stylesheetPattern = stylesheetPattern.replace("%TAB_TEXT_COLOR%", tabTextColorQss);

    stylesheetPattern = stylesheetPattern.replace("%TAB_SELECTED_BACKGROUND_COLOR%", selectedTabBackgroundColorQss);
    stylesheetPattern = stylesheetPattern.replace("%TAB_SELECTED_BORDER_COLOR%", selectedTabBorderColorQss);
    stylesheetPattern = stylesheetPattern.replace("%TAB_SELECTED_TEXT_COLOR%", selectedTabTextColorQss);

    stylesheetPattern = stylesheetPattern.replace("%CLOSE_PNG%",
                                                  Utils::StyleHelper::dpiSpecificImageFile(
                                                      ":/icons/close_button_dark.png"));
    stylesheetPattern = stylesheetPattern.replace("%CLOSE_GRAY_PNG%",
                                                  Utils::StyleHelper::dpiSpecificImageFile(
                                                      ":/icons/close_button_light_grey.png"));
    m_tabBar->setStyleSheet(stylesheetPattern);
}

void TabbedEditorPlugin::showTabBar()
{
    updateStyleToBaseColor();

    if (m_styleUpdatedToBaseColor) {
        disconnect(Core::EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)),
                   this, SLOT(showTabBar()));
        return;
    }

    m_styleUpdatedToBaseColor = true;
}

QString TabbedEditorPlugin::getQssStringFromColor(const QColor &color)
{
    return QString::fromLatin1("rgba(%1, %2, %3, %4)").arg(
                QString::number(color.red()),
                QString::number(color.green()),
                QString::number(color.blue()),
                QString::number(color.alpha()));
}
