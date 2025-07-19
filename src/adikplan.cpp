/***
 * File: adikmain.cpp
 * Architecture test for adikdrum
 * Compile: g++ -Wall -pedantic -Wextra adikmain.cpp -o adikmain
 * Date: Tue, 24/06/2025
 * Author: Coolbrother
 *
 * ***/
#include "adikplan.h"
#include "audioinfo.h"
#include "audioengine.h"
#include "adikplayer.h"
#include "adiktransport.h" // Inclure la nouvelle classe AdikTransport

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <chrono> // Nécessaire pour std::this_thread::sleep_for
#include <thread> // Nécessaire pour std::this_thread::sleep_for

// Déclaration de gPlayer comme variable globale
std::shared_ptr<AdikPlayer> gPlayer = std::make_shared<AdikPlayer>();

// Variable globale pour les messages d'état (utilisée uniquement si nécessaire, sinon elle peut être locale à main)
std::string _msgText = "";
#include <chrono> // Pour std::chrono::seconds, milliseconds, nanoseconds
#include <thread> // Pour std::this_thread::sleep_for

// Fonction utilitaire pour la mise en pause
void sleep(float numSecs) {
    // Convertit les secondes flottantes en une durée de nanosecondes
    // Cela permet de gérer les parties décimales des secondes
    std::this_thread::sleep_for(std::chrono::duration<float>(numSecs));
}

// Fonction de démonstration (peut rester telle quelle, elle n'utilise pas ncurses)
void demo1() {
    // Jouer le premier instrument (Synth Sine, index 0)
    gPlayer->playInstrument(0);

    // Attendre un peu pour que le son puisse être traité par le callback audio
    sleep(2.0f);

    // Jouer le deuxième instrument (Synth Square, index 1)
    gPlayer->playInstrument(6);
}

void demo2() {
    // Créer une instance de AdikPlayer (qui sera gérée par shared_ptr)
    // std::shared_ptr<AdikPlayer> player = std::make_shared<AdikPlayer>();

    // Créer une instance de AdikTransport en lui passant le player
    AdikTransport transport(gPlayer);

    // --- DÉMONSTRATION DU MODE SÉQUENCE EN TEMPS RÉEL (SIMULÉ) ---
    std::cout << "\n========== MODE SÉQUENCE (TEMPS RÉEL SIMULÉ) ==========" << std::endl;
    gPlayer->setPlaybackMode(AdikPlayer::SEQUENCE_MODE);

    gPlayer->selectSequenceInPlayer(0); // "Intro Groove (2 Mesures)"
    transport.printInfo();
    std::cout << "\n--- Simulation en temps réel de la Séquence 'Intro Groove' (Player Index 0) ---" << std::endl;
    transport.play();
    sleep(5); // Jouons pendant 5 secondes pour voir plusieurs boucles si la séquence est courte
    transport.stop();
    transport.printInfo();

    std::cout << "\n--- Réduction du volume du Charley Fermé sur Séquence 0 du Player et relecture en temps réel ---" << std::endl;
    gPlayer->sequenceList[0]->getTrack(2).volume = 0.2f;
    transport.play();
    sleep(5);
    transport.stop();

    gPlayer->selectSequenceInPlayer(1); // "Chorus Beat (1 Mesure)"
    transport.printInfo();
    std::cout << "\n--- Simulation en temps réel de la Séquence 'Chorus Beat' (Player Index 1) ---" << std::endl;
    transport.play();
    sleep(3); // Jouons 3 secondes
    transport.stop();


    // --- DÉMONSTRATION DES NOUVELLES FONCTIONS DE TRANSPORT ---
    std::cout << "\n\n========== DÉMONSTRATION DES FONCTIONS DE TRANSPORT ==========" << std::endl;
    gPlayer->setPlaybackMode(AdikPlayer::SEQUENCE_MODE);
    gPlayer->selectSequenceInPlayer(0); // On revient sur la séquence 0 (Intro Groove)

    std::cout << "\n--- Test de Pause ---" << std::endl;
    transport.play();
    sleep(2); // Joue pendant 2 secondes
    transport.pause();
    transport.printInfo();
    sleep(1); // Ne devrait rien jouer
    transport.play(); // Reprendre la lecture
    sleep(2);
    transport.stop();
    transport.printInfo();

    std::cout << "\n--- Test de setPosition ---" << std::endl;
    transport.setPosition(8); // Déplacer au 9ème pas (index 8)
    transport.printInfo();
    transport.play();
    sleep(3); // Jouer à partir de là
    transport.stop();

    std::cout << "\n--- Test de Rewind et Forward ---" << std::endl;
    transport.setPosition(10);
    transport.printInfo();
    transport.rewind(); // Reculer de 1 pas
    transport.printInfo();
    transport.forward(5); // Avancer de 5 pas
    transport.printInfo();
    transport.play();
    sleep(3);
    transport.stop();


    // --- DÉMONSTRATION DU MODE SONG EN TEMPS RÉEL (SIMULÉ) AVEC TRANSPORT ---
    std::cout << "\n\n========== MODE SONG (TEMPS RÉEL SIMULÉ) AVEC TRANSPORT ==========" << std::endl;
    gPlayer->setPlaybackMode(AdikPlayer::SONG_MODE);

    gPlayer->clearCurrentSong();
    gPlayer->currentSong->name = "Mon Morceau Final (Demo Temps Reel)";

    gPlayer->addSequenceFromPlayerToSong(0); // Intro Groove
    gPlayer->addSequenceFromPlayerToSong(1, 2); // Chorus Beat x2
    gPlayer->addSequenceFromPlayerToSong(0); // Intro Groove

    std::cout << "\nSéquences dans le morceau '" << gPlayer->currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < gPlayer->currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << gPlayer->currentSong->sequences[i]->name
                  << " (Longueur: " << gPlayer->currentSong->sequences[i]->lengthInSteps << " pas)" << std::endl;
    }
    transport.printInfo();

    std::cout << "\n--- Lecture en temps réel simulée du Morceau '" << gPlayer->currentSong->name << "' ---" << std::endl;
    transport.play();
    sleep(15); // Joue pendant 15 secondes pour voir le bouclage
    transport.stop();
    transport.printInfo();

    std::cout << "\n--- Mute de la caisse claire dans la PREMIERE séquence du morceau et relecture en temps réel ---" << std::endl;
    if (!gPlayer->currentSong->sequences.empty()) {
        gPlayer->currentSong->sequences[0]->getTrack(1).isMuted = true;
        std::cout << "Muting snare track on sequence '" << gPlayer->currentSong->sequences[0]->name << "'" << std::endl;
    }
    transport.play();
    sleep(10); // Relecture pendant 10 secondes
    transport.stop();

    std::cout << "\n--- Suppression de la séquence à l'index 1 du morceau et relecture en temps réel ---" << std::endl;
    gPlayer->deleteSequenceFromCurrentSong(1);
    transport.printInfo(); // Afficher l'état après suppression
    std::cout << "\nSéquences restantes dans le morceau '" << gPlayer->currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < gPlayer->currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << gPlayer->currentSong->sequences[i]->name
                  << " (Addr: " << gPlayer->currentSong->sequences[i].get() << ")" << std::endl;
    }
    transport.play();
    sleep(10); // Relecture après suppression
    transport.stop();

}

// Fonction displayStatus simplifiée pour utiliser std::cout
void displayStatus(const std::string& msg) {
    std::cout << "Status: " << msg << std::endl;
}

// Fonction keyHandler simplifiée pour utiliser std::cin.get()
void keyHandler() {
    std::cout << "--- AdikPlayer Console UI Alpha ---" << std::endl;
    std::cout << "Appuyez sur 'Q' pour quitter." << std::endl;
    std::cout << "1: Jouer Sine Wave (440Hz)" << std::endl;
    std::cout << "2: Jouer Square Wave (220Hz)" << std::endl;
    std::cout << "3: Jouer Kick" << std::endl;
    std::cout << "4: Jouer Snare" << std::endl;
    std::cout << "d: Afficher status du mixeur" << std::endl;
    std::cout << "s: Toggle Séquenceur Play/Stop" << std::endl;
    std::cout << "p: Avancer dans la Séquence (si en mode STEP)" << std::endl;
    std::cout << "\n--- Appuyez sur une touche et ENTER ---" << std::endl;

    if (gPlayer) {
        gPlayer->playInstrument(0); // Jouer un son initial si le joueur est prêt
    }

    char key;
    while (std::cin.get(key) && key != 'Q') { // Lire les caractères ligne par ligne
        switch (key) {
            case 'd':
                if (gPlayer) {
                    gPlayer->mixer.displayMixerStatus(); // Afficher l'état du mixeur
                    _msgText = "Statut du mixeur affiché.";
                } else {
                    _msgText = "Erreur: Player non initialisé.";
                }
                displayStatus(_msgText);
                break;
            case 's':
                // gPlayer->togglePlayback(); // Si vous avez une telle fonction
                _msgText = "Séquenceur: Fonctionnalité non implémentée (toggle Play/Stop).";
                displayStatus(_msgText);
                break;
            case 'p':
                demo1();
                _msgText = "Démonstration 1 lancée.";
                displayStatus(_msgText);
                break;
            
            case 'P':
                demo2();
                _msgText = "Démonstration 2 lancée.";
                displayStatus(_msgText);
                break;
            case 'v':
                gPlayer->stop();
                _msgText = "Séquenceur mis en pause.";
                displayStatus(_msgText);
                break;

            case ' ':
                if (gPlayer) {
                    if (gPlayer->isPlaying()) {
                        gPlayer->stop();
                        _msgText = "Séquenceur mis en pause.";
                    } else {
                        gPlayer->start();
                        _msgText = "Séquenceur démarré.";
                    }
                } else {
                    _msgText = "Erreur: Player non initialisé.";
                }
                displayStatus(_msgText);
                break;


            default:
                // Gérer les touches de '0' à '9'
                if (key >= '0' && key <= '9') {
                    int instruIndex = key - '0';
                    if (gPlayer && instruIndex < gPlayer->instrumentList.size()) {
                        gPlayer->playInstrument(instruIndex);
                        _msgText = "Jouer l'instrument " + std::to_string(instruIndex) + ".";
                    } else {
                        _msgText = "Index d'instrument invalide ou player non initialisé.";
                    }
                } else {
                    _msgText = "Touche '" + std::string(1, key) + "' non reconnue. Appuyez sur 'Q' pour quitter.";
                }
                displayStatus(_msgText);
                break;
        }
        // Pour gérer le caractère de nouvelle ligne laissé par std::cin.get()
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cout << "\n--- Appuyez sur une touche et ENTER ---" << std::endl; // Réafficher l'invite
    }

    std::cout << "Application terminée. Au revoir !" << std::endl;
}

int main() {
    std::cout << "Démarrage de la simulation AdikDrumMachine." << std::endl;

    // 1. Définir les paramètres audio via la structure AudioInfo
    AudioInfo globalAudioInfo(44100, 2, 32, 512); // sr=44100, ch=2, bd=32, bs=512
    globalAudioInfo.display(); // Pour confirmation

    // 2. Créer une instance de AdikPlayer
    gPlayer->initParams(globalAudioInfo); // Initialiser AdikPlayer avec AudioInfo

    // 3. Créer une instance du moteur audio
    AudioEngine audioEngine;
    // Initialiser AudioEngine avec AudioInfo et le pointeur vers le player
    if (!audioEngine.init(globalAudioInfo)) {
        std::cerr << "Échec de l'initialisation du moteur audio. Sortie." << std::endl;
        return 1;
    }

    // Assigner l'instance du player à AudioEngine
    audioEngine.setPlayer(gPlayer);
    
    // 4. Démarrer le flux audio physique via l'AudioEngine
    if (audioEngine.start()) {
        std::cout << "Lecture en cours..." << std::endl;
        for (size_t i = 0; i < gPlayer->instrumentList.size(); i++) {
            const auto& instru = gPlayer->instrumentList[i];
            std::cout << "Index " << i << ": " << instru->id << " (" << instru->name << ")" << std::endl;
        }

        // 5. Lancer l'interface Console et la gestion des touches
        keyHandler();

        gPlayer->stop();
        audioEngine.stop();
        audioEngine.close();
    } else {
        std::cerr << "Échec du démarrage du flux audio. Impossible de lire." << std::endl;
        audioEngine.close();
        return 1;
    }

    std::cout << "Simulation terminée." << std::endl;
    return 0;
}

/*
int main() {
    // Créer une instance de AdikPlayer (qui sera gérée par shared_ptr)
    std::shared_ptr<AdikPlayer> player = std::make_shared<AdikPlayer>();

    // Créer une instance de AdikTransport en lui passant le player
    AdikTransport transport(player);

    // --- DÉMONSTRATION DU MODE SÉQUENCE EN TEMPS RÉEL (SIMULÉ) ---
    std::cout << "\n========== MODE SÉQUENCE (TEMPS RÉEL SIMULÉ) ==========" << std::endl;
    player->setPlaybackMode(AdikPlayer::SEQUENCE_MODE);

    player->selectSequenceInPlayer(0); // "Intro Groove (2 Mesures)"
    transport.printInfo();
    std::cout << "\n--- Simulation en temps réel de la Séquence 'Intro Groove' (Player Index 0) ---" << std::endl;
    transport.play();
    player->simulateRealtimePlayback(5); // Jouons pendant 5 secondes pour voir plusieurs boucles si la séquence est courte
    transport.stop();
    transport.printInfo();

    std::cout << "\n--- Réduction du volume du Charley Fermé sur Séquence 0 du Player et relecture en temps réel ---" << std::endl;
    player->sequenceList[0]->getTrack(2).volume = 0.2f;
    transport.play();
    player->simulateRealtimePlayback(5);
    transport.stop();

    player->selectSequenceInPlayer(1); // "Chorus Beat (1 Mesure)"
    transport.printInfo();
    std::cout << "\n--- Simulation en temps réel de la Séquence 'Chorus Beat' (Player Index 1) ---" << std::endl;
    transport.play();
    player->simulateRealtimePlayback(3); // Jouons 3 secondes
    transport.stop();


    // --- DÉMONSTRATION DES NOUVELLES FONCTIONS DE TRANSPORT ---
    std::cout << "\n\n========== DÉMONSTRATION DES FONCTIONS DE TRANSPORT ==========" << std::endl;
    player->setPlaybackMode(AdikPlayer::SEQUENCE_MODE);
    player->selectSequenceInPlayer(0); // On revient sur la séquence 0 (Intro Groove)

    std::cout << "\n--- Test de Pause ---" << std::endl;
    transport.play();
    player->simulateRealtimePlayback(2); // Joue pendant 2 secondes
    transport.pause();
    transport.printInfo();
    player->simulateRealtimePlayback(1); // Ne devrait rien jouer
    transport.play(); // Reprendre la lecture
    player->simulateRealtimePlayback(2);
    transport.stop();
    transport.printInfo();

    std::cout << "\n--- Test de setPosition ---" << std::endl;
    transport.setPosition(8); // Déplacer au 9ème pas (index 8)
    transport.printInfo();
    transport.play();
    player->simulateRealtimePlayback(3); // Jouer à partir de là
    transport.stop();

    std::cout << "\n--- Test de Rewind et Forward ---" << std::endl;
    transport.setPosition(10);
    transport.printInfo();
    transport.rewind(); // Reculer de 1 pas
    transport.printInfo();
    transport.forward(5); // Avancer de 5 pas
    transport.printInfo();
    transport.play();
    player->simulateRealtimePlayback(3);
    transport.stop();


    // --- DÉMONSTRATION DU MODE SONG EN TEMPS RÉEL (SIMULÉ) AVEC TRANSPORT ---
    std::cout << "\n\n========== MODE SONG (TEMPS RÉEL SIMULÉ) AVEC TRANSPORT ==========" << std::endl;
    player->setPlaybackMode(AdikPlayer::SONG_MODE);

    player->clearCurrentSong();
    player->currentSong->name = "Mon Morceau Final (Demo Temps Reel)";

    player->addSequenceFromPlayerToSong(0); // Intro Groove
    player->addSequenceFromPlayerToSong(1, 2); // Chorus Beat x2
    player->addSequenceFromPlayerToSong(0); // Intro Groove

    std::cout << "\nSéquences dans le morceau '" << player->currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player->currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player->currentSong->sequences[i]->name
                  << " (Longueur: " << player->currentSong->sequences[i]->lengthInSteps << " pas)" << std::endl;
    }
    transport.printInfo();

    std::cout << "\n--- Lecture en temps réel simulée du Morceau '" << player->currentSong->name << "' ---" << std::endl;
    transport.play();
    player->simulateRealtimePlayback(15); // Joue pendant 15 secondes pour voir le bouclage
    transport.stop();
    transport.printInfo();

    std::cout << "\n--- Mute de la caisse claire dans la PREMIERE séquence du morceau et relecture en temps réel ---" << std::endl;
    if (!player->currentSong->sequences.empty()) {
        player->currentSong->sequences[0]->getTrack(1).isMuted = true;
        std::cout << "Muting snare track on sequence '" << player->currentSong->sequences[0]->name << "'" << std::endl;
    }
    transport.play();
    player->simulateRealtimePlayback(10); // Relecture pendant 10 secondes
    transport.stop();

    std::cout << "\n--- Suppression de la séquence à l'index 1 du morceau et relecture en temps réel ---" << std::endl;
    player->deleteSequenceFromCurrentSong(1);
    transport.printInfo(); // Afficher l'état après suppression
    std::cout << "\nSéquences restantes dans le morceau '" << player->currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player->currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player->currentSong->sequences[i]->name
                  << " (Addr: " << player->currentSong->sequences[i].get() << ")" << std::endl;
    }
    transport.play();
    player->simulateRealtimePlayback(10); // Relecture après suppression
    transport.stop();

    return 0;
}
*/

