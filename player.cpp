#include "player.h"
#include "./ui_player.h"
#include "ui_player.h"

#include <iostream> // tirar dps

Player::Player(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Player)
    , tocador(new QMediaPlayer)
    , audioOutput(new QAudioOutput)
{
    ui->setupUi(this);
    this->addItems();

    tocador->setAudioOutput(audioOutput); // setting audio output
    isPaused = false;
    currentMusic = "";

    connect(ui->play_button, &QPushButton::clicked, this, &Player::play_button); // play/pause
    connect(ui->m_list, &QListWidget::itemDoubleClicked, this, &Player::play); // doubleclick play
    connect(ui->m_list, &QListWidget::itemDoubleClicked, this, &Player::addToPlaylist); // adding the music in the playlist

    connect(ui->volume, &QSlider::valueChanged, this, &Player::volumeChanged); // changing volume
    connect(ui->add_source, &QPushButton::clicked, this, &Player::addSources);

    connect(tocador, &QMediaPlayer::durationChanged, this, &Player::durationChanged); // setting progress bar limit
    connect(tocador, &QMediaPlayer::positionChanged, this, &Player::positionChanged); // changing progress bar value

    connect(ui->playlist_page, &QPushButton::clicked, this, &Player::changeToPlaylist);
    connect(ui->home_page, &QPushButton::clicked, this, &Player::changeToHome);
}

Player::~Player()
{
    delete ui;
    delete tocador;
    delete audioOutput;
}

void Player::addItems(){
    std::ifstream fin("/home/kaio/MusicPlayer/paths.txt");

    for (std::string path; getline(fin, path);){
        for (const auto& c : std::filesystem::directory_iterator(path)){
            if (c.path().string().find(".mp3") != std::string::npos){ // detecting if the file is a mp3
                if (ui->m_list->findItems(QString::fromStdString(c.path().filename().string()), Qt::MatchExactly).toVector().size() == 0){ // searching in the widget
                    ui->m_list->addItem(QString::fromStdString(c.path().filename().string()));
                    path_to_music[c.path().filename().string()] = c.path().string();
                }
            }
        }
    }
    fin.close();
}

void Player::addToPlaylist(){
    playlist.clear();

    if (ui->random->isChecked()) { // random
        std::vector<int> indexes;
        int size = ui->m_list->count();

        for (int i = 0; i < size; i++){
            if (i != ui->m_list->currentRow()){ // removing the current song index
                indexes.push_back(i);
            }
        }

        std::random_shuffle(indexes.begin(), indexes.end());

        for (int c : indexes){
            playlist.push_back(ui->m_list->item(c)->text());
        }

    }
    else { // non-random
        for (int c = ui->m_list->currentRow() + 1; c < ui->m_list->count(); c++){
            playlist.push_back(ui->m_list->item(c)->text());
        }
    }
}

void Player::addSources(){
    std::fstream fou("/home/kaio/MusicPlayer/paths.txt", std::fstream::out | std::fstream::app);
    std::string source_path;

    source_path = QFileDialog::getExistingDirectory(this, tr("Abrir pasta"), "/home", QFileDialog::ShowDirsOnly).toStdString();

    if (source_path != "") {
        fou << source_path << std::endl;
        this->addItems();
    }
    fou.close();
}

void Player::play(){
    ui->play_button->setStyleSheet("border-image: url(:image/icons/pause.png);");
    isPaused = false;

    if (!isPaused && ui->m_list->currentItem()->text() != QString::fromStdString(currentMusic)){
        tocador->setSource(QUrl::fromLocalFile(QString::fromStdString(path_to_music[ui->m_list->currentItem()->text().toStdString()])));
        tocador->play();

        currentMusic = ui->m_list->currentItem()->text().toStdString();
    }
}

void Player::play_button() {
    if (ui->play_button->styleSheet() == "border-image: url(:image/icons/play.png);"){
        tocador->play();
        isPaused = false;
        ui->play_button->setStyleSheet("border-image: url(:image/icons/pause.png);");
    }
    else {
        tocador->pause();
        isPaused = true;
        ui->play_button->setStyleSheet("border-image: url(:image/icons/play.png);");
    }
}

void Player::positionChanged(qint64 value){
    ui->progressBar->setValue(value / 1000); // mudar para QSlider depois
    if (ui->progressBar->value() == ui->progressBar->maximum()){
        ui->m_list->setCurrentItem(ui->m_list->findItems(playlist.back(), Qt::MatchExactly).back());
        this->play();
        playlist.pop_back();
    }
}

void Player::durationChanged(qint64 value){
    ui->progressBar->setMaximum(value / 1000);
}

void Player::volumeChanged(double value){ // convert to double (the setVolume function goes from 0 to 1)
    audioOutput->setVolume(value / 100);
}

// Mexendo com páginas

void Player::changeToPlaylist(){
    ui->stackedWidget->setCurrentIndex(1);
}

void Player::changeToHome(){
    ui->stackedWidget->setCurrentIndex(0);
}
