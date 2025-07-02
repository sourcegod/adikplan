/***
 * File: adikmain.cpp
 * Architecture test for adikdrum
 * Compile: g++ -Wall -pedantic -Wextra adikmain.cpp -o adikmain
 * Date: Tue, 24/06/2025
 * Author: Coolbrother
 *
 * ***/

#include "adikplan.h"
#include "rtaudio_driver.h" // Incluez votre nouveau driver RtAudio
#include "audioengine.h"
#include "adikplayer.h"
#include "adiktransport.h" // Inclure la nouvelle classe AdikTransport
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Démarrage de la simulation AdikDrumMachine avec RtAudio." << std::endl;

    // Ordre d'inclusion des .h pour les définitions complètes dans une application réelle
    // Si vous utilisez un seul adikplan.h, assurez-vous de l'ordre interne.
    // #include "adiksound.h"
    // #include "adikinstrument.h"
    // #include "adikevent.h"
    // #include "adiktrack.h"
    // #include "adiksequence.h"
    // #include "adiksong.h"
    // #include "adikchannel.h"
    // #include "adikmixer.h"
    // #include "adikplayer.h" // Si AdikPlayer est dans son propre .h, il doit inclure toutes ses dépendances

    // Créer une instance de AdikPlayer
    AdikPlayer player;

    // Configurer les paramètres audio
    unsigned int sampleRate = 44100;
    unsigned int bufferSize = 512; // Taille du buffer à passer à RtAudio

    // Mettre à jour le player avec les paramètres audio réels
    player.sampleRate = sampleRate;
    player.bufferSizeSamples = bufferSize; // Important pour que player.calculateTimingParameters() soit correct
    player.calculateTimingParameters();

    // Créer l'instance du driver RtAudio
    RtAudioDriver audioDriver;

    // Démarrer le flux audio, en passant l'instance de 'player' comme userData
    if (audioDriver.startStream(sampleRate, bufferSize, &player)) {
        player.setPlaybackMode(AdikPlayer::SEQUENCE_MODE); // Ou SONG_MODE
        player.selectSequenceInPlayer(0); // Sélectionner la première séquence de démo

        player.start(); // Démarrer la lecture logique du player

        // Garder le programme en cours d'exécution pendant un certain temps pour la lecture audio
        std::cout << "Lecture en cours... (Appuyez sur Entrée pour arrêter)" << std::endl;
        std::cin.get(); // Attend que l'utilisateur appuie sur Entrée

        player.stop(); // Arrêter la lecture logique du player

        // Arrêter et fermer le flux audio
        audioDriver.closeStream();
    } else {
        std::cerr << "Échec du démarrage du moteur audio. Impossible de lire." << std::endl;
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

