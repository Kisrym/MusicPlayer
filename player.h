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
    void volumeChanged(double);
    void addToPlaylist();
    void changeToPlaylist();
    void changeToHome();

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

};
#endif // PLAYER_H
