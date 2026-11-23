#ifndef ASSETPREVIEW_H
#define ASSETPREVIEW_H

#include <QWidget>

class QLabel;
class QPushButton;
class QStackedWidget;
class QMediaPlayer;
class QAudioOutput;

class AssetPreview : public QWidget
{
    Q_OBJECT

public:
    explicit AssetPreview(QWidget *parent = nullptr);

public slots:
    void showAsset(const QString &filePath);
    void clear();

private slots:
    void togglePlayback();

private:
    void showPlaceholder(const QString &text = "Превью / звук");
    void showImage(const QString &filePath);
    void showAudio(const QString &filePath);

    QStackedWidget *m_stack = nullptr;

    QLabel *m_placeholder = nullptr;

    QLabel *m_imageLabel = nullptr;

    QWidget *m_audioPage = nullptr;
    QLabel  *m_audioLabel = nullptr;
    QPushButton *m_playButton = nullptr;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QString m_currentAudio;
};

#endif