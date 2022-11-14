#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QFileDialog>

#include <string>
#include <fstream>
#include <filesystem>
#include <map>
#include <vector>
#include <array>

#include <numeric>
#include <algorithm>

QT_BEGIN_NAMESPACE
namespace Ui { class Player; }
QT_END_NAMESPACE

class Player : public QMainWindow
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

private slots:
    void play();
    void play_button();
    void positionChanged(qint64);
    void durationChanged(qint64);
    void musicEnded_slot(bool back = false);

    void volumeChanged(double);
    void addToPlaylist();
    void editPlaylist();
    void setLoop(bool);

    void changeToPlaylist();
    void changeToHome();

    void sliderStart();
    void sliderEnd();
    void back();
    void skip();

signals:
    void musicEnded_signal(bool back = false);

private:
    void addItems();
    void addSources();

    Ui::Player *ui;
    QMediaPlayer *tocador;
    QAudioOutput *audioOutput;

    bool isPaused;
    std::string currentMusic; // mudar pra QString depois
    std::map<std::string, std::string> path_to_music;
    std::vector<QString> playlist;
    QString last_song;
};
#endif // PLAYER_H
