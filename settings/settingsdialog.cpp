
#include "settingsdialog.h"
#include "../themes/thememanager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QPainter>
#include <QMouseEvent>
#include <QGroupBox>
#include <QSizePolicy>

class ThemePreview : public QWidget
{
public:
    explicit ThemePreview(const QString &themeId, QWidget *parent = nullptr)
        : QWidget(parent),
        m_colors(ThemeManager::colors(themeId))
    {
        setFixedHeight(125);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect area = rect().adjusted(1, 1, -1, -1);

        painter.setPen(Qt::NoPen);
        painter.setBrush(m_colors.window);
        painter.drawRoundedRect(area, 7, 7);

        const QRect topBar(
            area.left(),
            area.top(),
            area.width(),
            17
            );

        painter.setBrush(m_colors.panel);
        painter.drawRoundedRect(topBar, 7, 7);

        const int sideWidth = area.width() * 0.22;

        QRect leftPanel(
            area.left(),
            area.top() + 17,
            sideWidth,
            area.height() - 17
            );

        painter.setBrush(m_colors.panel);
        painter.drawRect(leftPanel);

        QRect rightPanel(
            area.right() - sideWidth + 1,
            area.top() + 17,
            sideWidth,
            area.height() - 17
            );

        painter.drawRect(rightPanel);

        QRect editor(
            leftPanel.right() + 1,
            area.top() + 17,
            area.width() - sideWidth * 2,
            area.height() - 17
            );

        painter.setBrush(m_colors.editor);
        painter.drawRect(editor);

        const int lineHeight = 13;
        const int startX = editor.left() + 10;
        const int startY = editor.top() + 13;

        painter.setPen(Qt::NoPen);

        for (int i = 0; i < 6; ++i) {
            const int y = startY + i * lineHeight;

            if (y > editor.bottom() - 5)
                break;

            painter.setBrush(m_colors.lineNumber);
            painter.drawRect(
                editor.left() + 3,
                y,
                3,
                3
                );

            QColor codeColor = m_colors.text;

            if (i == 1 || i == 4)
                codeColor = m_colors.accent;

            painter.setBrush(codeColor);

            const int indent = (i % 3) * 9;
            const int width = 30 + (i * 13) % 55;

            painter.drawRoundedRect(
                startX + indent,
                y,
                qMin(width, editor.right() - startX - indent - 5),
                4,
                2,
                2
                );
        }

        painter.setBrush(m_colors.secondaryText);

        for (int i = 0; i < 5; ++i) {
            painter.drawRoundedRect(
                leftPanel.left() + 8,
                leftPanel.top() + 13 + i * 17,
                sideWidth - 18,
                4,
                2,
                2
                );
        }

        painter.setBrush(m_colors.accent);
        painter.drawRoundedRect(
            rightPanel.left() + 8,
            rightPanel.top() + 13,
            sideWidth - 16,
            5,
            2,
            2
            );

        painter.setBrush(m_colors.secondaryText);

        for (int i = 0; i < 3; ++i) {
            painter.drawRoundedRect(
                rightPanel.left() + 8,
                rightPanel.top() + 29 + i * 15,
                sideWidth - 20,
                4,
                2,
                2
                );
        }

        painter.setBrush(m_colors.accent);
        painter.drawRect(
            area.left(),
            area.bottom() - 2,
            area.width(),
            2
            );
    }

private:
    ThemeColors m_colors;
};

static QPushButton *createThemeCard(
    const QString &themeId,
    const QString &themeName,
    QWidget *parent
    )
{
    auto *card = new QPushButton(parent);

    card->setCursor(Qt::PointingHandCursor);
    card->setCheckable(true);
    card->setMinimumHeight(185);
    card->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
        );

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    auto *preview = new ThemePreview(themeId, card);

    auto *nameLabel = new QLabel(themeName, card);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    nameLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addWidget(preview);
    layout->addWidget(nameLabel);

    return card;
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Настройки ES Scenario Editor");
    setMinimumSize(750, 480);
    resize(850, 550);

    setupUi();
}

void SettingsDialog::setupUi()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_navigation = new QListWidget(this);
    m_navigation->setFixedWidth(180);

    m_navigation->addItem("Общие");
    m_navigation->addItem("Темы");

    mainLayout->addWidget(m_navigation);

    auto *rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(20, 20, 20, 20);

    m_pages = new QStackedWidget(this);

    auto *generalPage = new QWidget(this);
    auto *generalLayout = new QVBoxLayout(generalPage);

    auto *generalLabel = new QLabel("Общие настройки");

    generalLayout->addWidget(generalLabel);

    generalLayout->addWidget(
        new QLabel("Дополнительные настройки появятся позже.")
        );

    generalLayout->addStretch();

    m_pages->addWidget(generalPage);

    setupThemesPage();

    rightLayout->addWidget(m_pages);

    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    auto *closeButton = new QPushButton("Закрыть", this);

    buttonsLayout->addWidget(closeButton);
    rightLayout->addLayout(buttonsLayout);

    connect(
        closeButton,
        &QPushButton::clicked,
        this,
        &QDialog::accept
        );

    mainLayout->addLayout(rightLayout, 1);

    connect(
        m_navigation,
        &QListWidget::currentRowChanged,
        m_pages,
        &QStackedWidget::setCurrentIndex
        );

    m_navigation->setCurrentRow(1);
}

void SettingsDialog::setupThemesPage()
{
    auto *page = new QWidget(this);
    auto *pageLayout = new QVBoxLayout(page);

    auto *title = new QLabel("Темы оформления (пока не работает ;( )");
    title->setStyleSheet(
        "font-size: 20px; font-weight: bold;"
        );

    auto *description = new QLabel(
        "Выбери оформление для ES Scenario Editor."
        );

    pageLayout->addWidget(title);
    pageLayout->addWidget(description);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget(scrollArea);
    auto *contentLayout = new QVBoxLayout(content);

    auto *defaultTitle = new QLabel("Стандартные темы");
    defaultTitle->setStyleSheet(
        "font-size: 15px; font-weight: bold;"
        );

    contentLayout->addWidget(defaultTitle);

    auto *defaultGrid = new QGridLayout();
    defaultGrid->setSpacing(12);

    const QStringList defaultThemes = {
        "dark",
        "light"
    };

    for (int i = 0; i < defaultThemes.size(); ++i) {
        const QString themeId = defaultThemes[i];

        auto *card = createThemeCard(
            themeId,
            ThemeManager::themeName(themeId),
            content
            );

        m_themeCards.insert(themeId, card);
        defaultGrid->addWidget(card, 0, i);

        connect(
            card,
            &QPushButton::clicked,
            this,
            [this, themeId]() {
                selectTheme(themeId);
            }
            );
    }

    contentLayout->addLayout(defaultGrid);

    auto *animeTitle = new QLabel("Аниме-темы");
    animeTitle->setStyleSheet(
        "font-size: 15px; font-weight: bold;"
        );

    contentLayout->addWidget(animeTitle);

    auto *animeGrid = new QGridLayout();
    animeGrid->setSpacing(12);

    const QStringList animeThemes = {
        "anime_sakura",
        "anime_night",
        "anime_neon"
    };

    for (int i = 0; i < animeThemes.size(); ++i) {
        const QString themeId = animeThemes[i];

        auto *card = createThemeCard(
            themeId,
            ThemeManager::themeName(themeId),
            content
            );

        m_themeCards.insert(themeId, card);

        animeGrid->addWidget(
            card,
            i / 2,
            i % 2
            );

        connect(
            card,
            &QPushButton::clicked,
            this,
            [this, themeId]() {
                selectTheme(themeId);
            }
            );
    }

    contentLayout->addLayout(animeGrid);
    contentLayout->addStretch();

    scrollArea->setWidget(content);

    pageLayout->addWidget(scrollArea);

    m_pages->addWidget(page);

    m_selectedTheme = ThemeManager::currentTheme();

    updateThemeCards();
}

void SettingsDialog::selectTheme(const QString &themeId)
{
    if (!ThemeManager::availableThemes().contains(themeId))
        return;

    m_selectedTheme = themeId;

    ThemeManager::setTheme(themeId);

    updateThemeCards();

    emit themeChanged(themeId);
}

void SettingsDialog::updateThemeCards()
{
    for (auto it = m_themeCards.begin(); it != m_themeCards.end(); ++it) {
        const QString themeId = it.key();
        QPushButton *card = it.value();

        const bool selected = themeId == m_selectedTheme;

        card->setChecked(selected);

        if (selected) {
            card->setStyleSheet(R"(
                QPushButton {
                    border: 2px solid #008b8b;
                    border-radius: 8px;
                    background-color: palette(base);
                    text-align: left;
                }

                QPushButton:hover {
                    border-color: #00aaaa;
                }
            )");
        } else {
            card->setStyleSheet(R"(
                QPushButton {
                    border: 1px solid palette(mid);
                    border-radius: 8px;
                    background-color: palette(base);
                    text-align: left;
                }

                QPushButton:hover {
                    border: 1px solid #008b8b;
                }
            )");
        }
    }
}
