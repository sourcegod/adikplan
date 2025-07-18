# Makefile simple pour compiler un projet C++

# Compilateur C++
CXX = g++

# Options de compilation
# -Wall: Active tous les avertissements (warnings)
# -pedantic: Rend le compilateur plus strict sur la conformité au standard C++
# -Wextra: Active des avertissements supplémentaires
# -std=c++17: Utilise le standard C++17
CXXFLAGS = -Wall -pedantic -Wextra -std=c++17
LDFLAGS = -lrtaudio -lasound -lpthread -lncurses

# Répertoire des fichiers sources (.cpp, .h)
SRCS_DIR = src

# Répertoire de sortie pour les fichiers objets (.o) et l'exécutable
BUILD_DIR = build

# Noms des exécutables finaux
EXEC_NAME_ADIKPLAN = adikplan
EXEC_NAME_ADIKTUI = adiktui

# -----------------------------------------------------------------------------
# Définition des sources et objets pour AdikPlan
# Exclut adiktui.cpp car il contient le main de l'autre exécutable
SRCS_ADIKPLAN = $(filter-out $(SRCS_DIR)/adiktui.cpp, $(wildcard $(SRCS_DIR)/*.cpp))
OBJS_ADIKPLAN = $(patsubst $(SRCS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS_ADIKPLAN))

# Définition des sources et objets pour AdikTUI
# Inclut adiktui.cpp et les autres fichiers nécessaires pour AdikTUI,
# mais exclut adikplan.cpp si nécessaire (dans votre cas, adiktui.cpp suffit comme main)
# Nous devons compiler tous les autres fichiers .cpp sauf adikplan.cpp pour adiktui
SRCS_ADIKTUI = $(filter-out $(SRCS_DIR)/adikplan.cpp, $(wildcard $(SRCS_DIR)/*.cpp))
OBJS_ADIKTUI = $(patsubst $(SRCS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS_ADIKTUI))

# Cible par défaut : construire les deux exécutables
all: $(BUILD_DIR) $(BUILD_DIR)/$(EXEC_NAME_ADIKPLAN) $(BUILD_DIR)/$(EXEC_NAME_ADIKTUI)

# -----------------------------------------------------------------------------
# Règles pour l'exécutable AdikPlan

$(BUILD_DIR)/$(EXEC_NAME_ADIKPLAN): $(OBJS_ADIKPLAN)
	@echo "Liaison de l'exécutable $(EXEC_NAME_ADIKPLAN)..."
	$(CXX) $(OBJS_ADIKPLAN) -o $@ $(LDFLAGS)

# -----------------------------------------------------------------------------
# Règles pour l'exécutable AdikTUI

$(BUILD_DIR)/$(EXEC_NAME_ADIKTUI): $(OBJS_ADIKTUI)
	@echo "Liaison de l'exécutable $(EXEC_NAME_ADIKTUI)..."
	$(CXX) $(OBJS_ADIKTUI) -o $@ $(LDFLAGS)

# -----------------------------------------------------------------------------
# Règles de compilation génériques pour les fichiers objets
# Cette règle compile tout fichier .cpp en .o, indépendamment de l'exécutable final.
# Cela permet de réutiliser les fichiers objets compilés pour les deux exécutables.
$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp $(BUILD_DIR)
	@echo "Compilation de $< en $@..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle pour créer le répertoire de build s'il n'existe pas
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "Répertoire $(BUILD_DIR) créé."

# Cible 'clean' pour supprimer tous les fichiers générés
clean:
	@echo "Nettoyage des fichiers générés..."
	@rm -rf $(BUILD_DIR)

.PHONY: all clean $(BUILD_DIR)
