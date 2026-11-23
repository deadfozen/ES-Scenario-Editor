#include "assetpreview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QPixmap>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

AssetPreview::AssetPreview(QWidget *parent)
    : QWidget(parent)
{
    m_stack = new QStackedWidget(this);

    m_placeholder = new QLabel("Превью / звук", this);
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet("color: #888; font-size: 14px;");
    m_stack->addWidget(m_placeholder);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("background-color: #1c1c1c;");
    m_imageLabel->setMinimumHeight(150);
    m_stack->addWidget(m_imageLabel);

    m_audioPage = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(m_audioPage);
        layout->setAlignment(Qt::AlignCenter);

        m_audioLabel = new QLabel("🎵 Аудиофайл", m_audioPage);
        m_audioLabel->setAlignment(Qt::AlignCenter);
        m_audioLabel->setStyleSheet("color: #ddd; font-size: 13px;");

        m_playButton = new QPushButton("▶ Воспроизвести", m_audioPage);
        m_playButton->setFixedWidth(160);

        layout->addWidget(m_audioLabel);
        layout->addWidget(m_playButton, 0, Qt::AlignCenter);
    }
    m_stack->addWidget(m_audioPage);

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);

    connect(m_playButton, &QPushButton::clicked,
            this, &AssetPreview::togglePlayback);

    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, [this](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::PlayingState)
                    m_playButton->setText("⏸ Пауза");
                else
                    m_playButton->setText("▶ Воспроизвести");
            });

    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia) {
                    m_player->setPosition(0);
                    m_player->stop();
                }
            });

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(m_stack);

    showPlaceholder();
}

void AssetPreview::showAsset(const QString &filePath)
{
    if (m_player->playbackState() != QMediaPlayer::StoppedState)
        m_player->stop();

    if (filePath.isEmpty()) {
        showPlaceholder();
        return;
    }

    const QString suffix = QFileInfo(filePath).suffix().toLower();

    static const QStringList imageExts = {
        "png", "jpg", "jpeg", "bmp", "gif", "webp"
    };
    if (imageExts.contains(suffix)) {
        showImage(filePath);
        return;
    }

    static const QStringList audioExts = {
        "wav", "mp3", "ogg", "flac", "m4a"
    };
    if (audioExts.contains(suffix)) {
        showAudio(filePath);
        return;
    }

    showPlaceholder("Нет превью для этого файла");
}

void AssetPreview::clear()
{
    if (m_player->playbackState() != QMediaPlayer::StoppedState)
        m_player->stop();
    showPlaceholder();
}

void AssetPreview::showPlaceholder(const QString &text)
{
    m_placeholder->setText(text);
    m_stack->setCurrentWidget(m_placeholder);
}

void AssetPreview::showImage(const QString &filePath)
{
    QPixmap pix(filePath);
    if (pix.isNull()) {
        showPlaceholder("Не удалось загрузить изображение");
        return;
    }

    m_imageLabel->setPixmap(
        pix.scaled(m_imageLabel->size(),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation)
        );
    m_stack->setCurrentWidget(m_imageLabel);
}

void AssetPreview::showAudio(const QString &filePath)
{
    m_currentAudio = filePath;
    m_audioLabel->setText("🎵 " + QFileInfo(filePath).fileName());
    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_stack->setCurrentWidget(m_audioPage);
}

void AssetPreview::togglePlayback()
{
    if (m_currentAudio.isEmpty())
        return;

    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}