/***
 * File: adikmain.cpp
 * Architecture test for adikdrum
 * Compile: g++ -Wall -pedantic -Wextra adikmain.cpp -o adikmain
 * Date: Tue, 24/06/2025
 * Author: Coolbrother
 *
 * ***/

#include "adikplan.h"

// --- Exemple d'utilisation dans main.cpp ---
int main() {
    AdikPlayer player; // Changé de DrumPlayer à AdikPlayer

    // --- DÉMONSTRATION DU MODE SÉQUENCE ---
    std::cout << "========== MODE SÉQUENCE ==========" << std::endl;
    player.setPlaybackMode(AdikPlayer::SEQUENCE_MODE); // Utilise AdikPlayer::SEQUENCE_MODE

    // Jouer la première séquence (index 0) du Player, qui a 2 mesures (32 pas au total)
    player.selectSequenceInPlayer(0); // "Intro Groove (2 Mesures)"
    std::cout << "\n--- Simulation de la Séquence 'Intro Groove' (Player Index 0, 2 mesures) ---" << std::endl;
    // Note: On accède à la séquence via le shared_ptr pour obtenir sa longueur
    player.simulatePlayback(player.sequenceList[0]->lengthInSteps); 

    // Modifier la séquence actuelle (Piste 3, Charley Fermé) et rejouer
    std::cout << "\n--- Réduction du volume du Charley Fermé sur Séquence 0 du Player et relecture ---" << std::endl;
    player.sequenceList[0]->getTrack(2).volume = 0.2f; // Modification via shared_ptr
    player.simulatePlayback(player.sequenceList[0]->lengthInSteps);

    // Sélectionner et jouer la deuxième séquence (index 1) du Player, qui a 1 mesure (16 pas au total)
    std::cout << "\n--- Simulation de la Séquence 'Chorus Beat' (Player Index 1, 1 mesure) ---" << std::endl;
    player.selectSequenceInPlayer(1); // "Chorus Beat (1 Mesure)"
    player.simulatePlayback(player.sequenceList[1]->lengthInSteps); 

    // --- DÉMONSTRATION DU MODE SONG ---
    std::cout << "\n\n========== MODE SONG ==========" << std::endl;
    player.setPlaybackMode(AdikPlayer::SONG_MODE); // Utilise AdikPlayer::SONG_MODE

    // Créer un nouveau morceau
    player.clearCurrentSong();
    player.currentSong->name = "Mon Morceau Final (Demo Shared Ptr)";

    // Construire le morceau à partir des séquences du Player
    // Sequence 0: 2 mesures (32 pas)
    // Sequence 1: 1 mesure (16 pas)
    player.addSequenceFromPlayerToSong(0);     // Ajoute "Intro Groove (2 Mesures)" (1 fois par défaut)
    player.addSequenceFromPlayerToSong(1, 2);  // Ajoute "Chorus Beat (1 Mesure)" 2 fois
    player.addSequenceFromPlayerToSong(0);     // Ajoute "Intro Groove (2 Mesures)" à nouveau

    // Aperçu des séquences dans le morceau
    std::cout << "\nSéquences dans le morceau '" << player.currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player.currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player.currentSong->sequences[i]->name 
                  << " (Addr: " << player.currentSong->sequences[i].get() << ")" << std::endl;
    }
    // Notez que la séquence "Intro Groove" (index 0 et 3) pointe vers la même adresse mémoire,
    // montrant qu'il s'agit du même objet partagé.

    // Calculer la longueur totale du morceau pour la simulation
    int totalSongSteps = 0;
    if (player.currentSong) {
        for (const auto& seq_ptr : player.currentSong->sequences) {
            totalSongSteps += seq_ptr->lengthInSteps;
        }
    }

    std::cout << "\n--- Lecture du Morceau '" << player.currentSong->name << "' (Total " << totalSongSteps << " pas) ---" << std::endl;
    player.simulatePlayback(totalSongSteps); // Joue le morceau complet

    // Rejouer le morceau pour montrer le bouclage
    std::cout << "\n--- Relecture du Morceau (bouclage) ---" << std::endl;
    player.simulatePlayback(totalSongSteps + player.currentSong->sequences[0]->lengthInSteps); // Joue plus que le total pour voir le bouclage

    // Exemple: Muter une piste spécifique DANS la première séquence du morceau (affecte la copie dans le song, pas l'originale du player)
    // Ici, puisque les séquences sont partagées, muter la piste de la première séquence du song affectera aussi la dernière séquence du song
    // car elles pointent vers le même objet AdikSequence.
    std::cout << "\n--- Mute de la caisse claire dans la PREMIERE séquence du morceau et relecture de TOUT le morceau ---" << std::endl;
    if (!player.currentSong->sequences.empty()) {
        std::cout << "Muting snare track on sequence '" << player.currentSong->sequences[0]->name << "' (shared_ptr address: " << player.currentSong->sequences[0].get() << ")" << std::endl;
        player.currentSong->sequences[0]->getTrack(1).isMuted = true; // Mute la piste de Snare de la première séquence du morceau
    }
    player.simulatePlayback(totalSongSteps); // Relecture complète du morceau

    std::cout << "\n--- Suppression de la séquence à l'index 1 du morceau ---" << std::endl;
    player.deleteSequenceFromCurrentSong(1); // Supprime la deuxième séquence (Chorus Beat)

    std::cout << "\nSéquences restantes dans le morceau '" << player.currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player.currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player.currentSong->sequences[i]->name 
                  << " (Addr: " << player.currentSong->sequences[i].get() << ")" << std::endl;
    }

    std::cout << "\n--- Relecture du morceau après suppression ---" << std::endl;
    totalSongSteps = 0; // Recalculer la longueur totale après suppression
    if (player.currentSong) {
        for (const auto& seq_ptr : player.currentSong->sequences) {
            totalSongSteps += seq_ptr->lengthInSteps;
        }
    }
    player.simulatePlayback(totalSongSteps);


    return 0;
}
 
