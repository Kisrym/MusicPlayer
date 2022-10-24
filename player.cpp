#include "player.h"
#include "./ui_player.h"
#include "ui_player.h"

#include <QMouseEvent>

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

    connect(ui->random, &QCheckBox::toggled, this, &Player::addToPlaylist);

    // changing the widget pages
    connect(ui->playlist_page, &QPushButton::clicked, this, &Player::changeToPlaylist);
    connect(ui->home_page, &QPushButton::clicked, this, &Player::changeToHome);

    // music
    connect(ui->progressBar, &QSlider::sliderPressed, this, &Player::sliderStart);
    connect(ui->progressBar, &QSlider::sliderReleased, this, &Player::sliderEnd);

    connect(ui->save, &QPushButton::pressed, this, &Player::editPlaylist);
    connect(this, &Player::musicEnded_signal, this, &Player::musicEnded_slot);
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
    ui->playlist_list->clear();

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
    for (const QString& c : playlist){
        ui->playlist_list->addItem(c);
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
        ui->current_music_label->setText(QString::fromStdString(currentMusic));
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
    ui->progressBar->setValue(value / 1000);

    if (ui->progressBar->value() == ui->progressBar->maximum() && !playlist.empty()){
        emit musicEnded_signal();
    }
}

void Player::musicEnded_slot(){
    ui->m_list->setCurrentItem(ui->m_list->findItems(playlist.front(), Qt::MatchExactly).at(0));
    ui->playlist_list->removeItemWidget(ui->playlist_list->findItems(playlist.front(), Qt::MatchExactly).at(0));

    if (!ui->playlist_list->findItems(playlist.front(), Qt::MatchExactly).empty()){
        delete ui->playlist_list->findItems(playlist.front(), Qt::MatchExactly).at(0);
    }

    this->play();
    playlist.erase(playlist.begin());
}

void Player::durationChanged(qint64 value){
    ui->progressBar->setMaximum(value / 1000);
}

void Player::volumeChanged(double value){ // convert to double (the setVolume function goes from 0 to 1)
    audioOutput->setVolume(value / 100);
}

void Player::sliderStart(){
    tocador->pause();
}

void Player::sliderEnd(){
    tocador->setPosition(ui->progressBar->value() * 1000);
    tocador->play();
}

/*Mexendo com páginas */

void Player::changeToPlaylist(){
    ui->stackedWidget->setCurrentIndex(1);
}

void Player::changeToHome(){
    ui->stackedWidget->setCurrentIndex(0);
}

/* Playlist Page */
void Player::editPlaylist(){
    playlist.clear();

    for (int c = 0; c < ui->playlist_list->count(); c++){
        playlist.push_back(ui->playlist_list->item(c)->text());
    }
}
