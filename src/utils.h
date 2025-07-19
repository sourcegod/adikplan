#ifndef UTILS_H
#define UTILS_H

#include <iostream>  // Pour std::cout, std::cerr
#include <string>    // Pour std::string
#include <chrono>    // Pour std::chrono::duration, seconds, milliseconds, etc.
#include <thread>    // Pour std::this_thread::sleep_for

// Définissez DEBUG à 1 pour activer les messages de débogage, ou à 0 pour les désactiver
#define DEBUG 1 // Changez à 0 pour désactiver les messages de débogage

// Fonction pour jouer un "beep"
// Note: Le "beep" standard de la console n'est pas garanti sur tous les systèmes
// et est souvent un caractère ASCII bell ('\a'). Pour un son système,
// il faudrait utiliser des bibliothèques spécifiques (ex: ncurses pour TUI, ou des APIs audio).
// Pour une implémentation console simple, on utilise '\a'.
void beep() {
    std::cout << "\a" << std::flush; // Écrit le caractère BELL et force l'affichage
}

// Fonction pour afficher des messages de débogage
void debugMsg(const std::string& message) {
#if DEBUG == 1
    std::cerr << "[DEBUG] " << message << std::endl;
#endif
}

// Fonction pour mettre le programme en pause pendant un nombre de secondes flottant
void sleep(float numSecs) {
    std::this_thread::sleep_for(std::chrono::duration<float>(numSecs));
}

#endif // UTILS_H
