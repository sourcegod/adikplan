#include "adiktui.h"
#include "adikplayer.h" // Assuming this is needed for gPlayer->mixer.displayMixerStatus()
#include "audioinfo.h"
#include "audioengine.h"
#include "adiktransport.h" // Inclure la nouvelle classe AdikTransport

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

// Constructor
AdikTUI::AdikTUI(std::shared_ptr<AdikPlayer> player) : gPlayer(player) {
    // Initialize ncurses
    initscr();              // Initialize the screen structure
    cbreak();               // Read characters one by one, without waiting for Enter
    noecho();               // Don't display typed characters
    keypad(stdscr, TRUE);   // Enable reading of special keys (arrows, F-keys)
}

// Destructor
AdikTUI::~AdikTUI() {
    // Clean up ncurses
    endwin(); // Restore the terminal to its original state
    std::cout << "Application terminée. Au revoir !" << std::endl;
}

// Helper function to display status messages at the bottom of the screen
void AdikTUI::displayStatus(const std::string& msg) {
    mvprintw(LINES - 1, 0, "%s", msg.c_str());
    clrtoeol(); // Clear to the end of the line to erase previous messages
    refresh();  // Refresh the screen to display changes
}

// Helper function to print the main menu
void AdikTUI::printMenu() {
    clear(); // Clear the screen before printing the menu
    mvprintw(0, 0, "--- AdikPlayer UI Alpha ---\n");
    mvprintw(1, 0, "Appuyez sur 'Q' pour quitter.\n");
    mvprintw(2, 0, "1: Jouer Sine Wave (440Hz)\n");
    mvprintw(3, 0, "2: Jouer Square Wave (220Hz)\n");
    mvprintw(4, 0, "3: Jouer Kick\n");
    mvprintw(5, 0, "4: Jouer Snare\n");
    mvprintw(6, 0, "d: Afficher status du mixeur\n");
    mvprintw(7, 0, "s: Toggle Séquenceur Play/Stop\n");
    mvprintw(8, 0, "p: Avancer dans la Séquence (si en mode STEP)\n");
    mvprintw(9, 0, "\n--- Appuyez sur une touche ---\n");

    // Display instrument list if available
    if (gPlayer) {
        for (size_t i = 0; i < gPlayer->instrumentList.size(); i++) {
            const auto& instru = gPlayer->instrumentList[i];
            mvprintw(10 + i, 0, "Index %zu: %s (%s)\n", i, instru->id.c_str(), instru->name.c_str());
        }
    }
    refresh();
}

// Demo function (moved to AdikTUI as it's triggered by TUI)
void AdikTUI::demo1() {
    if (!gPlayer) return;

    // Play the first instrument (Synth Sine, index 0)
    gPlayer->playInstrument(5);
    displayStatus("Playing instrument 5...");
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Requires <chrono> and <thread>

    // Play the second instrument (Synth Square, index 1)
    gPlayer->playInstrument(6);
    displayStatus("Playing instrument 6...");
}

// Main key handler function
void AdikTUI::keyHandler() {
    printMenu(); // Display the initial menu

    if (gPlayer) {
        gPlayer->playInstrument(0);
    }
    
    int key;
    while ((key = getch()) != 'Q') { // Read keys until 'Q' is pressed
        switch (key) {
            case 'd':
                if (gPlayer) {
                    gPlayer->mixer.displayMixerStatus(); // Display mixer status
                    _msgText = "Statut du mixeur affiché.";
                } else {
                    _msgText = "Erreur: Player non initialisé.";
                }
                displayStatus(_msgText);
                break;
            case 's':
                // This is an assumption of the function in AdikPlayer for toggling
                // gPlayer->togglePlayback(); // If you have such a function
                beep(); // A simple beep for now
                _msgText = "Séquenceur: Fonctionnalité non implémentée (toggle Play/Stop).";
                displayStatus(_msgText);
                break;
            case 'p':
                demo1();
                _msgText = "Démonstration lancée.";
                displayStatus(_msgText);
                break;

            case 'v':
                gPlayer->stop();
                _msgText = "Séquenceur mis en pause.";
                displayStatus(_msgText);
                break;

            case ' ': // Nouvelle case pour la barre d'espace
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
                // Manage key from '0' to '9'
                if (key >= '0' && key <= '9') {
                    int instruIndex = key - '0';
                    if (gPlayer && instruIndex < gPlayer->instrumentList.size()) {
                        gPlayer->playInstrument(instruIndex);
                        _msgText = "Jouer l'instrument " + std::to_string(instruIndex) + ".";
                    } else {
                        _msgText = "Index d'instrument invalide ou player non initialisé.";
                    }
                } else {
                    _msgText = "Touche '" + std::string(1, (char)key) + "' (" + std::to_string(key) + ") non reconnue. Appuyez sur 'Q' pour quitter.";
                }
                displayStatus(_msgText);
                break;
        }
        printMenu(); // Redraw menu after each action
    }
}

// Main function (in adiktui.cpp as requested, but typically in a separate main.cpp)
int main() {
  // Global player shared_ptr to be used by the main function and passed to AdikTUI
  std::shared_ptr<AdikPlayer> gPlayer = std::make_shared<AdikPlayer>();

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
    if (audioEngine.start()) { // start() no longer needs parameters, they are stored in audioEngine.audioSetup
        std::cout << "Lecture en cours... (Appuyez sur Entrée pour arrêter)" << std::endl;

        // 5. Instancier la classe AdikTUI et lancer l'interface Text et la gestion des touches
        AdikTUI adikTUI(gPlayer);
        adikTUI.keyHandler();

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
