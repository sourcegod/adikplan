#ifndef ADIKTUI_H
#define ADIKTUI_H

#include "adikplayer.h" // Include AdikPlayer
#include <ncurses.h>
#include <string>
#include <memory>

class AdikTUI {
public:
    // Constructor
    AdikTUI(std::shared_ptr<AdikPlayer> player);

    // Destructor
    ~AdikTUI();

    // Function to handle keyboard input and manage the UI
    void keyHandler();

private:
    std::shared_ptr<AdikPlayer> gPlayer;
    std::string _msgText;

    // Private helper functions for UI display
    void displayStatus(const std::string& msg);
    void printMenu();
    void demo1(); // If demo1 specifically relates to TUI actions
};

#endif // ADIKTUI_H
