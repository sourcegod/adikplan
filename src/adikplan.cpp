/***
 * File: adikmain.cpp
 * Architecture test for adikdrum
 * Compile: g++ -Wall -pedantic -Wextra adikmain.cpp -o adikmain
 * Date: Tue, 24/06/2025
 * Author: Coolbrother
 *
 * ***/

#include "adikplan.h"

int main() {
    AdikPlayer player;

    // --- DÉMONSTRATION DU MODE SÉQUENCE EN TEMPS RÉEL (SIMULÉ) ---
    std::cout << "========== MODE SÉQUENCE (TEMPS RÉEL SIMULÉ) ==========" << std::endl;
    player.setPlaybackMode(AdikPlayer::SEQUENCE_MODE);

    player.selectSequenceInPlayer(0); // "Intro Groove (2 Mesures)"
    std::cout << "\n--- Simulation en temps réel de la Séquence 'Intro Groove' (Player Index 0) ---" << std::endl;
    // Jouons pendant 5 secondes pour voir plusieurs boucles si la séquence est courte
    player.simulateRealtimePlayback(5);

    std::cout << "\n--- Réduction du volume du Charley Fermé sur Séquence 0 du Player et relecture en temps réel ---" << std::endl;
    player.sequenceList[0]->getTrack(2).volume = 0.2f;
    player.simulateRealtimePlayback(5);

    player.selectSequenceInPlayer(1); // "Chorus Beat (1 Mesure)"
    std::cout << "\n--- Simulation en temps réel de la Séquence 'Chorus Beat' (Player Index 1) ---" << std::endl;
    player.simulateRealtimePlayback(3); // Jouons 3 secondes

    // --- DÉMONSTRATION DU MODE SONG EN TEMPS RÉEL (SIMULÉ) ---
    std::cout << "\n\n========== MODE SONG (TEMPS RÉEL SIMULÉ) ==========" << std::endl;
    player.setPlaybackMode(AdikPlayer::SONG_MODE);

    player.clearCurrentSong();
    player.currentSong->name = "Mon Morceau Final (Demo Temps Reel)";

    player.addSequenceFromPlayerToSong(0);
    player.addSequenceFromPlayerToSong(1, 2);
    player.addSequenceFromPlayerToSong(0);

    std::cout << "\nSéquences dans le morceau '" << player.currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player.currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player.currentSong->sequences[i]->name
                  << " (Addr: " << player.currentSong->sequences[i].get() << ")" << std::endl;
    }

    std::cout << "\n--- Lecture en temps réel simulée du Morceau '" << player.currentSong->name << "' ---" << std::endl;
    // Jouons le morceau pendant un temps suffisant pour qu'il boucle au moins une fois
    // Total steps: 32 (seq0) + 16 (seq1) + 16 (seq1) + 32 (seq0) = 96 steps
    // Samples per step = 44100 * 60 / 120 / 4 = 5512.5 samples/step
    // Total samples for song: 96 * 5512.5 = 529200 samples
    // Total time for song: 529200 / 44100 = 12 seconds
    player.simulateRealtimePlayback(15); // Joue pendant 15 secondes pour voir le bouclage

    std::cout << "\n--- Mute de la caisse claire dans la PREMIERE séquence du morceau et relecture en temps réel ---" << std::endl;
    if (!player.currentSong->sequences.empty()) {
        std::cout << "Muting snare track on sequence '" << player.currentSong->sequences[0]->name << "' (shared_ptr address: " << player.currentSong->sequences[0].get() << ")" << std::endl;
        player.currentSong->sequences[0]->getTrack(1).isMuted = true;
    }
    player.simulateRealtimePlayback(10); // Relecture pendant 10 secondes

    std::cout << "\n--- Suppression de la séquence à l'index 1 du morceau et relecture en temps réel ---" << std::endl;
    player.deleteSequenceFromCurrentSong(1);

    std::cout << "\nSéquences restantes dans le morceau '" << player.currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player.currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player.currentSong->sequences[i]->name
                  << " (Addr: " << player.currentSong->sequences[i].get() << ")" << std::endl;
    }
    player.simulateRealtimePlayback(10); // Relecture après suppression

    return 0;
}
