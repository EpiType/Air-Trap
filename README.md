# Air-Trap - R-Type Clone

Un moteur de jeu multi-joueur en réseau inspiré de R-Type, développé en C++17 avec une architecture ECS (Entity-Component-System).

## 🏗️ Architecture

### Entity-Component-System (ECS)
- **Entités** : Identifiants uniques (`uint32_t`)
- **Composants** : Structures de données pures
  - `PositionComponent`
  - `VelocityComponent`
  - `SpriteComponent`
  - `NetworkIdComponent`
- **Systèmes** : Logique de jeu
  - `PhysicsSystem` : Mouvement et collisions
  - `RenderingSystem` : Affichage (client uniquement)
  - `NetworkSystem` : Communication réseau

### Architecture Réseau
- **Serveur Authoritaire** : Toute la logique de jeu est exécutée côté serveur
- **Client** : Envoie les inputs, reçoit et affiche l'état du jeu
- **Protocole** : UDP avec protocole binaire personnalisé
- **Multi-threading** : Le serveur est multi-threadé pour gérer les clients sans bloquer

## 🔧 Technologies

- **C++17** : Langage principal
- **CMake** : Système de build
- **Conan** : Gestionnaire de dépendances
- **ASIO** : Communication réseau (UDP)
- **SFML** : Rendu graphique et audio (client uniquement)
- **GitHub Actions** : CI/CD

## 📦 Build

### Prérequis

- CMake 3.20+
- Compilateur C++17 (GCC 9+, Clang 10+, MSVC 2019+)
- Python 3.11+ (pour Conan)
- Conan 2.x

### Linux / macOS

```bash
chmod +x build.sh
./build.sh
```

### Windows

```cmd
build.bat
```

### Build manuel

```bash
# Installer les dépendances
mkdir build && cd build
conan install .. --build=missing -s build_type=Release

# Compiler
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## 🚀 Exécution

### Lancer le serveur

```bash
# Linux/macOS
./build/r-type_server [port]

# Windows
.\build\Release\r-type_server.exe [port]
```

Par défaut, le serveur écoute sur le port 4242.

### Lancer le client

```bash
# Linux/macOS
./build/r-type_client [server_ip] [port]

# Windows
.\build\Release\r-type_client.exe [server_ip] [port]
```

Par défaut, le client se connecte à `127.0.0.1:4242`.

## 🔍 Outils de Développement

### Formatage du code

```bash
chmod +x format.sh
./format.sh
```

### Analyse statique

```bash
chmod +x lint.sh
./lint.sh
```

## 🔄 CI/CD

Le projet utilise GitHub Actions pour l'intégration continue :

1. **Style Check** : Vérifie le formatage avec clang-format et clang-tidy
2. **Build Linux** : Compilation sur Ubuntu (avec cache Conan)
3. **Build Windows** : Compilation sur Windows (génère des .exe)
4. **Build macOS** : Compilation sur macOS
5. **Tests** : Exécution des tests unitaires (optionnel)

Le pipeline s'exécute automatiquement sur chaque push et pull request vers les branches `main` et `develop`.

### Artifacts

Les binaires compilés sont disponibles en tant qu'artifacts GitHub Actions :
- `air-trap-linux` : Binaires Linux
- `air-trap-windows` : Exécutables Windows (.exe)
- `air-trap-macos` : Binaires macOS

## 📝 Workflow Git

Le projet utilise **Conventional Commits** :

```
<type>(<scope>): <description>

Types:
- [FEAT] Nouvelle fonctionnalité
- [FIX] Correction de bug
- [CHORE] Tâches de maintenance
- [DOCS] Documentation
- [STYLE] Formatage
- [REFACTOR] Refactoring
```

## 📂 Structure du Projet

```
Air-Trap/
├── .github/
│   └── workflows/
│       └── ci-cd.yml          # Pipeline CI/CD
├── common/
│   └── include/
│       ├── ecs/               # Entity-Component-System
│       │   ├── Entity.hpp
│       │   ├── Registry.hpp
│       │   ├── components/    # Composants
│       │   └── systems/       # Systèmes
│       ├── network/           # Protocole réseau
│       └── events/            # Event Bus
├── server/
│   ├── include/
│   │   └── network/
│   └── src/
│       └── main.cpp           # Point d'entrée serveur
├── client/
│   ├── include/
│   │   └── network/
│   └── src/
│       └── main.cpp           # Point d'entrée client
├── CMakeLists.txt             # Configuration CMake
├── conanfile.txt              # Dépendances Conan
├── .clang-format              # Configuration formatage
├── .clang-tidy                # Configuration linting
└── README.md
```

## 📄 Licence

Ce projet est développé dans un contexte éducatif (Epitech).
