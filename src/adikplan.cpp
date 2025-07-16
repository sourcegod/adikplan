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
#include <ncurses.h>

#include <iostream>
#include <string>
#include <memory>
#include <vector>

// std::shared_ptr<AdikPlayer> player = nullptr;
std::shared_ptr<AdikPlayer> gPlayer = std::make_shared<AdikPlayer>();
std::string _msgText = "";
void demo1() {
    // Jouer le premier instrument (Synth Sine, index 0)
    gPlayer->playInstrument(5);

    // Attendre un peu pour que le son puisse être traité par le callback audio
    // Dans une vraie application, cela se ferait dans votre boucle audio principale.
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Nécessite <chrono> et <thread>

    // Jouer le deuxième instrument (Synth Square, index 1)
    gPlayer->playInstrument(6);

}

void displayStatus(std::string& msg) {
    mvprintw(LINES - 1, 0, msg.c_str());
        refresh(); // Rafraîchir l'écran pour afficher les changements
}

void keyHandler() {
    initscr();             // Initialise la structure de l'écran
    cbreak();              // Permet de lire les caractères un par un, sans attendre Enter
    noecho();              // Ne pas afficher les caractères tapés
    keypad(stdscr, TRUE);  // Active la lecture des touches spéciales (flèches, F-keys)
    // nodelay(stdscr, 1);

    printw("--- AdikPlayer UI Alpha ---\n");
    printw("Appuyez sur 'Q' pour quitter.\n");
    printw("1: Jouer Sine Wave (440Hz)\n");
    printw("2: Jouer Square Wave (220Hz)\n");
    printw("3: Jouer Kick\n");
    printw("4: Jouer Snare\n");
    printw("d: Afficher status du mixeur\n");
    printw("s: Toggle Séquenceur Play/Stop\n");
    printw("p: Avancer dans la Séquence (si en mode STEP)\n");
    printw("\n--- Appuyez sur une touche ---\n");

    gPlayer->playInstrument(0);
    int key;
    while ((key = getch()) != 'Q') { // Lire les touches tant que 'Q' n'est pas pressé
        // beep(); 
        switch (key) {
            case '1':
                gPlayer->playInstrument(0); // Index du Sine Wave
                _msgText = "Joué: Sine Wave (440Hz)   "; // Efface la ligne précédente
                displayStatus(_msgText);
                break;
            case '2':
                gPlayer->playInstrument(1); // Index du Square Wave
                _msgText = "Joué: Square Wave (220Hz)";
                displayStatus(_msgText);
                break;
            case '3':
                gPlayer->playInstrument(2);
                break;
            case '4':
                gPlayer->playInstrument(4);
                break;
            case 'd':
                gPlayer->mixer.displayMixerStatus(); // Afficher l'état du mixeur
              _msgText = "Statut du mixeur affiché.";
              displayStatus(_msgText);
                break;
            case 's':
                // Ceci est une hypothèse de la fonction dans AdikPlayer pour le toggle
                // gPlayer->togglePlayback(); // Si vous avez une telle fonction
                beep();
                break;
            case 'p':
                demo1();
                break;

            default:
                _msgText = "Touche " + std::to_string(key) + " non reconnue. Appuyez sur 'Q' pour quitter.";
                displayStatus(_msgText);
                break;
        }
    
    } // End while loop

    // 3. Nettoyage de ncurses
    endwin(); // Restaure le terminal à son état original

    std::cout << "Application terminée. Au revoir !" << std::endl;

    // Joindre le thread audio avant de quitter (pour un arrêt propre)
    // C'est un exemple. Dans un vrai système RtAudio, vous feriez audio.stop() et audio.close()
    // Si simulateRealtimePlayback est une boucle infinie, vous devrez ajouter une condition de sortie.
    // audioThread.join(); // Ne peut pas joindre un thread détaché. Gérer la fin autrement.

    return;
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

    // Assigner le l'instance du player à AudioEngine
    audioEngine.setPlayer(gPlayer);
    
    // 4. Démarrer le flux audio physique via l'AudioEngine
    if (audioEngine.start()) { // start() n'a plus besoin des paramètres, ils sont stockés dans audioEngine.audioSetup
        std::cout << "Lecture en cours... (Appuyez sur Entrée pour arrêter)" << std::endl;
        for (size_t i=0; i < gPlayer->instrumentList.size(); i++) {
          const auto& instru = gPlayer->instrumentList[i];
          std::cout << "Index " << i << ": " << instru->id << " (" << instru->name << ")\n";
        }

        // 5. Lancer l'interface Text et la gestion des touches
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

