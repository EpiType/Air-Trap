# Air-Trap - R-Type Clone

Un moteur de jeu multi-joueur en réseau inspiré de R-Type, développé en C++23 avec une architecture ECS (Entity-Component-System).

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

- **C++23** : Langage principal
- **CMake** : Système de build
- **Conan 2.x** : Gestionnaire de dépendances
- **ASIO** : Communication réseau (UDP)
- **SFML 2.6+** : Rendu graphique et audio (client uniquement)
- **GitHub Actions** : CI/CD avec mirroring Epitech

## 📦 Build

### Prérequis

- CMake 3.20+
- Compilateur C++23 (GCC 11+, Clang 14+, MSVC 2022+)
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
conan install .. --build=missing -s build_type=Release -c tools.system.package_manager:mode=install

# Compiler
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --config Release -j$(nproc)
```

## 🚀 Exécution

### Lancer le serveur

```bash
# Linux/macOS
./build/r-type_server [port]

# Windows
.\build\Release\r-type_server.exe [port]
```

Par défaut, le serveur écoute sur le port **4242**.

### Lancer le client

```bash
# Linux/macOS
./build/r-type_client [server_ip] [port]

# Windows
.\build\Release\r-type_client.exe [server_ip] [port]
```

Par défaut, le client se connecte à **127.0.0.1:4242**.

## 🔍 Outils de Développement

### Formatage du code

```bash
chmod +x format.sh
./format.sh
```

Utilise **clang-format-18** avec le style Google (IndentWidth: 4, ColumnLimit: 120).

### Analyse statique

```bash
chmod +x lint.sh
./lint.sh
```

Exécute **clang-tidy-18** pour détecter les problèmes potentiels.

### Smart Commit Tool

Outil intelligent pour créer des commits groupés automatiquement :

```bash
chmod +x smart-commit.sh
./smart-commit.sh
```

**Fonctionnalités** :
- ✅ Groupe automatiquement les fichiers par type ([DOCS], [CHORE], [STYLE], [FIX], [ADD], [REFACTOR])
- 📊 Analyse les diffs et génère des descriptions intelligentes
- 📝 Affiche les stats (+/-) pour chaque fichier
- 🎯 Format multi-ligne pour les commits avec plusieurs fichiers
- 🚀 Option de push automatique après les commits

**Exemple d'utilisation** :

```bash
$ ./smart-commit.sh

🔍 Analyse des fichiers modifiés...

📦 Groupes détectés:
  [CHORE] : 2 fichier(s)

🚀 Créer des commits groupés ? (o/n): o

📝 Groupe [CHORE]:
   - .github/workflows/ci-cd.yml (+25 -10)
   - smart-commit.sh (+30 -15)

   💡 Suggestion:
   - ci-cd.yml: update messages
   - smart-commit.sh: refactor logic

   Description (Entrée pour accepter, ou écris la tienne): 

   ✅ Commit créé

✅ 1 commit(s) créé(s)

🚀 Push maintenant ? (o/n): o
📤 Push vers origin/initArchi...
✅ Push terminé!

🎉 Terminé!
```

### Git Hooks

Installer les hooks locaux pour valider les messages de commit :

```bash
chmod +x install-hooks.sh
./install-hooks.sh
```

Le hook `commit-msg` vérifie que tous les commits suivent le format :
```
[TYPE] description

Types valides: [ADD], [FIX], [CHORE], [DOCS], [STYLE], [REFACTOR]
```

## 🔄 CI/CD Pipeline

Le projet utilise **GitHub Actions** avec une stratégie intelligente :

### Jobs

1. **commit-validation** (push uniquement)
   - Valide le format des messages de commit
   - Pattern : `[TYPE] description`
   - Skip les merge commits

2. **style-check**
   - Installe clang-format-18 et clang-tidy-18 depuis LLVM
   - Vérifie le code avec clang-tidy (si compile_commands.json existe)
   - clang-format désactivé jusqu'à formatage complet du code

3. **build-linux** (push sur `dev` uniquement)
   - Compile sur Ubuntu avec GCC
   - Cache Conan pour accélérer les builds
   - 5 tentatives de téléchargement (réseau instable)
   - Upload des binaires en artifacts

4. **build-windows** (PR vers `main` uniquement)
   - Compile sur Windows avec MSVC
   - Génère des `.exe`
   - Upload des exécutables en artifacts

5. **build-macos** (PR vers `main` uniquement)
   - Compile sur macOS avec Clang
   - Build parallèle avec sysctl
   - Upload des binaires en artifacts

6. **mirror** (tous les push)
   - Synchronise automatiquement vers le dépôt Epitech
   - SSH avec clé déployée
   - Push force de toutes les branches et tags

7. **notify** (toujours exécuté)
   - Envoie des notifications Discord avec embed
   - Webhook différent pour main/dev vs autres branches
   - Rapport détaillé : ✅/❌ pour chaque job
   - Lien vers le commit et informations contextuelles

### Stratégie de Branches

```
feature → dev (push direct) → PR → main (protection)
```

- **dev** : Build Linux systématique à chaque push
- **main** : Build multi-plateforme (Windows + macOS) uniquement sur PR

### Configuration Secrets

Ajouter dans **Settings → Secrets and variables → Actions** :

| Secret | Description | Exemple |
|--------|-------------|---------|
| `MIRROR_URL` | URL SSH du dépôt Epitech | `git@github.com:epitech/project.git` |
| `MIRROR_SSH_KEY` | Clé SSH privée (deploy key sur mirror) | Contenu de `~/.ssh/id_rsa` |
| `DISCORD_WEBHOOK_MAINDEV` | Webhook Discord pour main/dev | `https://discord.com/api/webhooks/...` |
| `DISCORD_WEBHOOK_OTHER` | Webhook Discord pour features | `https://discord.com/api/webhooks/...` |

### Créer un Webhook Discord

1. Ouvrir les paramètres du canal Discord
2. **Intégrations** → **Webhooks** → **Nouveau Webhook**
3. Copier l'URL et l'ajouter dans GitHub Secrets

### Artifacts

Les binaires sont disponibles pendant **90 jours** :
- `air-trap-linux` : Exécutables Linux (serveur + client)
- `air-trap-windows` : Exécutables Windows (.exe)
- `air-trap-macos` : Exécutables macOS

Téléchargeables depuis l'onglet **Actions** → Run → **Artifacts**.

## 📝 Workflow Git & Commits

### Format des Commits

Le projet utilise un format personnalisé strict :

```
[TYPE] description courte (max 72 caractères)

Types valides:
- [ADD]      : Nouvelle fonctionnalité
- [FIX]      : Correction de bug
- [CHORE]    : Maintenance (CI/CD, config, outils)
- [DOCS]     : Documentation
- [STYLE]    : Formatage (whitespace, indentation)
- [REFACTOR] : Refactoring sans changement fonctionnel
```

**Pour plusieurs fichiers**, utiliser le format multi-ligne :

```
[CHORE] update CI/CD and commit tools
- ci-cd.yml: enhance Discord notifications with embed format
- smart-commit.sh: add intelligent file grouping and diff analysis
```

### Commits Automatiques avec smart-commit.sh

L'outil `smart-commit.sh` automatise ce processus :

1. **Analyse** tous les fichiers modifiés
2. **Groupe** par type selon les patterns :
   - `.md`, `.txt`, `docs/` → DOCS
   - `.github/`, `CMakeLists.txt`, `*.sh` → CHORE
   - `*.cpp`, `*.hpp` avec "fix/bug" dans le diff → FIX
   - `*.cpp`, `*.hpp` autres → ADD
3. **Génère** des descriptions basées sur l'analyse du diff
4. **Crée** un commit par groupe
5. **Push** optionnel à la fin

### Installation des Hooks Git

```bash
./install-hooks.sh
```

Valide automatiquement le format avant chaque commit local.

## 📂 Structure du Projet

```
Air-Trap/
├── .github/
│   ├── workflows/
│   │   └── ci-cd.yml              # Pipeline CI/CD complet (452 lignes)
│   └── BRANCH_PROTECTION.md       # Doc protection des branches
├── .githooks/
│   └── commit-msg                 # Hook validation commits
├── common/
│   └── include/
│       ├── ecs/                   # Entity-Component-System
│       │   ├── Entity.hpp
│       │   ├── Registry.hpp
│       │   ├── components/        # Composants
│       │   └── systems/           # Systèmes
│       ├── network/               # Protocole réseau
│       └── events/                # Event Bus
├── server/
│   ├── include/network/
│   └── src/main.cpp               # Point d'entrée serveur
├── client/
│   ├── include/network/
│   └── src/main.cpp               # Point d'entrée client
├── CMakeLists.txt                 # Configuration CMake (C++23)
├── conanfile.txt                  # Dépendances (SFML 2.6.1, asio 1.28.0)
├── .clang-format                  # Style Google + customizations
├── .clang-tidy                    # Règles analyse statique
├── smart-commit.sh                # Outil commits intelligents
├── install-hooks.sh               # Installation hooks Git
├── build.sh / build.bat           # Scripts de build
├── format.sh                      # Formatage automatique
├── lint.sh                        # Analyse statique
└── README.md
```

## 🧪 Tests (à implémenter)

Les tests unitaires sont désactivés dans la CI (`if: false`). Pour les activer :

1. Implémenter les tests (Google Test recommandé)
2. Ajouter `r-type_tests` dans CMakeLists.txt
3. Retirer `if: false` du job `tests` dans ci-cd.yml

## 📊 Métriques (futures)

- **Code Coverage** : Intégration avec Codecov/Coveralls
- **Performance** : Benchmarks automatiques
- **Documentation** : Génération Doxygen dans la CI

## 📄 Documentation

- [Sujet du projet (PDF)](./B-CPP-500_rtype.pdf)
- [Annexes (PDF)](./B-CPP-500_rtype_apendix.pdf)
- [Protection des branches](./.github/BRANCH_PROTECTION.md)

## 🤝 Contribution

1. Créer une branche feature : `git checkout -b feature/ma-fonctionnalite`
2. Faire vos modifications et commits avec le bon format
3. Push vers `dev` : `git push origin feature/ma-fonctionnalite`
4. Merger dans `dev` (pas de PR requise)
5. Créer une PR de `dev` vers `main` pour la validation finale

**Outils recommandés** :
- Utilisez `./smart-commit.sh` pour des commits propres
- Exécutez `./format.sh` avant de push
- Vérifiez avec `./lint.sh` pour éviter les erreurs de CI

## 📄 Licence

Ce projet est développé dans un contexte éducatif (Epitech B-CPP-500).

## 🎯 Roadmap

- [x] Architecture ECS de base
- [x] Protocole réseau UDP
- [x] CI/CD complet avec multi-plateforme
- [x] Outils de développement (format, lint, smart-commit)
- [x] Git hooks et validation automatique
- [ ] Implémentation du gameplay R-Type
- [ ] Tests unitaires et couverture de code
- [ ] Système de collision avancé
- [ ] Audio et effets visuels
- [ ] Menu et interface utilisateur
- [ ] Déploiement serveur automatique


