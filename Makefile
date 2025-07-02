# Makefile simple pour compiler un projet C++

# Compilateur C++
CXX = g++

# Options de compilation
# -Wall: Active tous les avertissements (warnings)
# -pedantic: Rend le compilateur plus strict sur la conformité au standard C++
# -Wextra: Active des avertissements supplémentaires
# -std=c++17: Utilise le standard C++17
CXXFLAGS = -Wall -pedantic -Wextra -std=c++17
LDFLAGS = -lrtaudio -lasound -lpthread

# Répertoire des fichiers sources (.cpp, .h)
SRCS_DIR = src

# Répertoire de sortie pour les fichiers objets (.o) et l'exécutable
BUILD_DIR = build

# Nom de l'exécutable final
EXEC_NAME = adikplan

# -----------------------------------------------------------------------------
# Ne modifiez pas les lignes ci-dessous, elles gèrent la logique de compilation

# Recherche de tous les fichiers sources .cpp dans le répertoire SRCS_DIR
SRCS = $(wildcard $(SRCS_DIR)/*.cpp)

# Dérivation des noms des fichiers objets (.o)
# Chaque fichier .cpp dans SRCS_DIR/ sera compilé en un fichier .o dans BUILD_DIR/
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Cible par défaut : construire l'exécutable
all: $(BUILD_DIR) $(BUILD_DIR)/$(EXEC_NAME)

# Règle pour lier les fichiers objets en exécutable
$(BUILD_DIR)/$(EXEC_NAME): $(OBJS)
	@echo "Liaison de l'exécutable $(EXEC_NAME)..."
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) # $@ est le nom de la cible (l'exécutable)

# Règle pour compiler chaque fichier .cpp en .o
# $< est le premier prérequis (le fichier .cpp)
# $@ est le nom de la cible (le fichier .o)
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
	@rm -rf $(BUILD_DIR) # Supprime tout le répertoire de build

.PHONY: all clean $(BUILD_DIR) # Déclare les cibles comme "phony" pour qu'elles soient toujours exécutées
