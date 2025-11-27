# 📐 Architecture du Projet Air-Trap

## Table des matières

1. [Vue d'ensemble](#vue-densemble)
2. [Structure des dossiers](#structure-des-dossiers)
3. [Architecture ECS](#architecture-ecs)
4. [Architecture Réseau](#architecture-réseau)
5. [Système de Build](#système-de-build)
6. [Pipeline CI/CD](#pipeline-cicd)
7. [Flux de données](#flux-de-données)

---

## Vue d'ensemble

Air-Trap est un moteur de jeu multijoueur basé sur une **architecture Client-Serveur Authoritative** avec un système **Entity-Component-System (ECS)** pour gérer les entités du jeu.

### Principes fondamentaux

- **Séparation Client/Serveur** : Le serveur a l'autorité sur la logique de jeu
- **ECS** : Découplage entre données (composants) et logique (systèmes)
- **Event Bus** : Communication inter-systèmes sans dépendances directes
- **UDP Binaire** : Protocole réseau optimisé pour la performance
- **Multi-threading** : Le serveur gère les clients de manière asynchrone

---

## Structure des dossiers

```
Air-Trap/
│
├── .github/
│   └── workflows/
│       └── ci-cd.yml                 # Pipeline CI/CD (Linux/Windows/macOS)
│
├── common/                           # Code partagé entre client et serveur
│   └── include/
│       ├── ecs/                      # Système Entity-Component-System
│       │   ├── Entity.hpp            # Définition d'une entité (uint32_t)
│       │   ├── Registry.hpp          # Gestionnaire d'entités et composants
│       │   ├── components/           # Composants (données pures)
│       │   │   ├── PositionComponent.hpp
│       │   │   ├── VelocityComponent.hpp
│       │   │   ├── SpriteComponent.hpp
│       │   │   └── NetworkIdComponent.hpp
│       │   └── systems/              # Systèmes (logique)
│       │       ├── ISystem.hpp       # Interface de base
│       │       ├── PhysicsSystem.hpp # Mouvement et collisions
│       │       └── RenderingSystem.hpp # Rendu (client uniquement)
│       │
│       ├── network/                  # Protocole réseau
│       │   ├── Protocol.hpp          # Structures de paquets UDP
│       │   └── UdpSocket.hpp         # Abstraction socket
│       │
│       └── events/                   # Event Bus (Mediator pattern)
│           └── EventBus.hpp          # Pub/Sub pour découpler les systèmes
│
├── server/                           # Code spécifique au serveur
│   ├── include/
│   │   └── network/
│   │       └── NetworkServer.hpp    # Serveur UDP multi-threadé
│   └── src/
│       └── main.cpp                 # Point d'entrée serveur (boucle de jeu)
│
├── client/                           # Code spécifique au client
│   ├── include/
│   │   └── network/
│   │       └── NetworkClient.hpp    # Client UDP
│   └── src/
│       └── main.cpp                 # Point d'entrée client (rendu + inputs)
│
├── tests/                            # Tests unitaires (GTest)
│   ├── ecs/
│   │   ├── test_registry.cpp        # Tests du Registry
│   │   └── test_components.cpp      # Tests des composants
│   ├── network/
│   │   └── test_protocol.cpp        # Tests du protocole
│   └── CMakeLists.txt               # Configuration des tests
│
├── config/                           # Configuration runtime
│   ├── server.json                  # Config serveur (port, tick rate)
│   └── client.json                  # Config client (résolution, audio)
│
├── assets/                           # Ressources graphiques et sonores
│   ├── sprites/
│   │   ├── players/                 # Sprites des joueurs
│   │   ├── enemies/                 # Sprites des ennemis
│   │   ├── projectiles/             # Sprites des projectiles
│   │   └── effects/                 # Effets visuels
│   ├── sounds/                      # Effets sonores
│   ├── musics/                      # Musiques de fond
│   ├── fonts/                       # Polices pour l'UI
│   └── README.md                    # Organisation des assets
│
├── scripts/                          # Scripts utilitaires
│   ├── build.sh / build.bat         # Scripts de build
│   ├── format.sh                    # Formater le code
│   ├── lint.sh                      # Analyser le code
│   ├── smart-commit.sh              # Commits intelligents
│   └── install-hooks.sh             # Installer les git hooks
│
├── docs/                             # Documentation
│   ├── ARCHITECTURE.md              # Ce fichier
│   └── subject/                     # Sujet du projet
│
├── CMakeLists.txt                    # Configuration CMake principale
├── conanfile.txt                     # Dépendances (ASIO, SFML, GTest)
├── .clang-format                     # Style de code
├── .clang-tidy                       # Analyse statique
├── .gitignore                        # Fichiers ignorés par Git
└── README.md                         # Documentation principale
```

---

## Architecture ECS

### Concept

L'**Entity-Component-System** sépare :
- **Données** (Composants) : Structs contenant uniquement des attributs
- **Logique** (Systèmes) : Classes qui opèrent sur des groupes d'entités
- **Entités** : Simples identifiants (`uint32_t`) qui lient les composants

### Composants disponibles

| Composant | Description | Attributs |
|-----------|-------------|-----------|
| `PositionComponent` | Position dans le monde | `x`, `y` |
| `VelocityComponent` | Vitesse de déplacement | `vx`, `vy` |
| `SpriteComponent` | Apparence graphique | `texturePath`, `width`, `height`, `layer` |
| `NetworkIdComponent` | Identifiant réseau | `networkId`, `isOwned` |

### Systèmes disponibles

| Système | Responsabilité | Où |
|---------|---------------|-----|
| `PhysicsSystem` | Mouvement, collisions | Serveur |
| `RenderingSystem` | Affichage avec SFML | Client |
| `InputSystem` | Capture des inputs | Client |
| `NetworkSystem` | Envoi/réception de paquets | Client + Serveur |

### Registry

Le `Registry` est le cœur de l'ECS :

```cpp
ecs::Registry registry;

// Créer une entité
Entity player = registry.createEntity();

// Ajouter des composants
registry.addComponent(player, PositionComponent{100.0f, 200.0f});
registry.addComponent(player, VelocityComponent{5.0f, 0.0f});

// Récupérer un composant
auto& pos = registry.getComponent<PositionComponent>(player);

// Itérer sur des entités avec certains composants
auto entities = registry.view<PositionComponent, VelocityComponent>();
for (Entity e : entities) {
    // Traiter chaque entité
}
```

### Event Bus (Mediator Pattern)

Pour éviter les dépendances directes entre systèmes, on utilise un **Event Bus** :

```cpp
events::EventBus eventBus;

// Souscrire à un événement
eventBus.subscribe<EntityCreatedEvent>([](const EntityCreatedEvent& event) {
    std::cout << "Entity created: " << event.entityId << std::endl;
});

// Publier un événement
eventBus.publish(EntityCreatedEvent{42});
```

**Événements disponibles** :
- `EntityCreatedEvent` : Une entité a été créée
- `EntityDestroyedEvent` : Une entité a été détruite
- `PlayerInputEvent` : Input d'un joueur reçu

---

## Architecture Réseau

### Principe Client-Serveur Authoritaire

```
┌─────────┐                           ┌─────────┐
│ CLIENT  │                           │ SERVER  │
│         │                           │         │
│  Input  │ ── PlayerInputPacket ──> │ Physics │
│ Render  │ <── EntityUpdatePacket ── │ Logic   │
│         │                           │ Network │
└─────────┘                           └─────────┘
```

- **Client** : Envoie uniquement les **inputs** du joueur
- **Serveur** : Exécute **toute la logique**, envoie l'**état du monde**

### Protocole UDP Binaire

Toutes les communications utilisent des **paquets binaires** pour la performance.

#### Structure de paquet

```cpp
#pragma pack(push, 1)
struct PacketHeader {
    PacketType type;        // Type de paquet (1 byte)
    uint32_t sequence;      // Numéro de séquence
    uint16_t payloadSize;   // Taille du payload
};
#pragma pack(pop)
```

#### Types de paquets

| Type | Direction | Description |
|------|-----------|-------------|
| `PLAYER_INPUT` | Client → Serveur | Input du joueur |
| `PING` | Client → Serveur | Mesure de latence |
| `GAME_STATE` | Serveur → Client | État global du jeu |
| `ENTITY_UPDATE` | Serveur → Client | Mise à jour d'une entité |
| `ENTITY_SPAWN` | Serveur → Client | Création d'entité |
| `ENTITY_DESTROY` | Serveur → Client | Destruction d'entité |
| `PONG` | Serveur → Client | Réponse au ping |
| `DISCONNECT` | Bidirectionnel | Déconnexion |

#### Exemple de paquet

```cpp
struct PlayerInputPacket {
    PacketHeader header;
    uint32_t playerId;
    uint32_t inputFlags;  // Bitmask : UP, DOWN, LEFT, RIGHT, SHOOT
    float deltaTime;
};

struct EntityUpdatePacket {
    PacketHeader header;
    uint32_t entityId;
    float posX, posY;
    float velX, velY;
};
```

### Multi-threading Serveur

Le serveur utilise un **thread dédié pour le réseau** :

```
┌──────────────────────────────────────┐
│          SERVEUR                     │
├──────────────────────────────────────┤
│  Thread Principal                    │
│  - Boucle de jeu (60 FPS)           │
│  - PhysicsSystem.update()           │
│  - Logique de jeu                   │
│                                      │
│  Thread Réseau                       │
│  - Réception de paquets UDP         │
│  - Envoi de paquets UDP             │
│  - File d'attente thread-safe       │
└──────────────────────────────────────┘
```

**Avantage** : Les messages réseau ne bloquent jamais la boucle de jeu.

---

## Système de Build

### CMake

Le projet utilise **CMake 3.20+** pour la compilation multi-plateforme.

#### Cibles

- `air-trap-common` : Bibliothèque interface (headers partagés)
- `r-type_server` : Exécutable serveur
- `r-type_client` : Exécutable client

#### Dépendances

Gérées par **Conan 2.x** :

```
[requires]
asio/1.28.0      # Communication réseau (header-only)
sfml/2.6.1       # Rendu graphique (client uniquement)
gtest/1.14.0     # Tests unitaires
```

### Scripts de Build

#### Linux/macOS

```bash
./scripts/build.sh
```

1. Crée le dossier `build/`
2. Installe les dépendances avec Conan
3. Configure avec CMake
4. Compile avec tous les cores disponibles

#### Windows

```cmd
scripts\build.bat
```

Même processus, génère des `.exe` dans `build/Release/`.

### Tests Unitaires

```bash
# Lancer tous les tests
cd build && ctest

# Lancer un test spécifique
./build/test_ecs
./build/test_network
```

---

## Pipeline CI/CD

Le pipeline GitHub Actions s'exécute sur **chaque push** et **pull request** vers `main` ou `develop`.

### Jobs du Pipeline

```
┌─────────────────────────────────────────┐
│  1. STYLE CHECK (Ubuntu)               │
│     - clang-format --dry-run           │
│     - clang-tidy                       │
└─────────────────────────────────────────┘
              │
    ┌─────────┴─────────┬─────────────┐
    │                   │             │
    ▼                   ▼             ▼
┌──────────┐      ┌──────────┐  ┌──────────┐
│  2. BUILD│      │  3. BUILD│  │  4. BUILD│
│   LINUX  │      │  WINDOWS │  │   MACOS  │
│          │      │          │  │          │
│ Ubuntu   │      │ MSVC     │  │ Clang    │
│ GCC      │      │ .exe     │  │          │
└──────────┘      └──────────┘  └──────────┘
    │                   │             │
    └─────────┬─────────┴─────────────┘
              ▼
    ┌──────────────────┐
    │  5. TESTS        │
    │  (Optionnel)     │
    └──────────────────┘
```

### Cache Conan

Le pipeline met en cache `~/.conan2` pour accélérer les builds :

```yaml
- uses: actions/cache@v4
  with:
    path: ~/.conan2
    key: conan-${{ runner.os }}-${{ hashFiles('**/conanfile.txt') }}
```

**Gain de temps** : ~5-10 minutes par build.

### Artifacts

Les binaires compilés sont disponibles en téléchargement :
- `air-trap-linux` : ELF binaries
- `air-trap-windows` : `.exe` files
- `air-trap-macos` : Mach-O binaries

---

## Flux de données

### Boucle de jeu serveur (60 FPS)

```
┌─────────────────────────────────────────────┐
│  1. Recevoir inputs des clients (async)    │
│     - Thread réseau → File d'attente       │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  2. Traiter les inputs                     │
│     - Mettre à jour les composants         │
│     - PlayerInputEvent → EventBus          │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  3. Mettre à jour les systèmes             │
│     - PhysicsSystem.update(deltaTime)      │
│     - Calcul de collisions                 │
│     - Logique de jeu                       │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  4. Envoyer l'état aux clients             │
│     - Broadcast EntityUpdatePacket         │
│     - Thread réseau (non bloquant)         │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  5. Attendre le prochain tick (16ms)       │
└─────────────────────────────────────────────┘
```

### Boucle de jeu client (60 FPS)

```
┌─────────────────────────────────────────────┐
│  1. Capturer les inputs (SFML)             │
│     - Clavier, souris, manette             │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  2. Envoyer au serveur                     │
│     - PlayerInputPacket (UDP)              │
│     - Thread réseau (non bloquant)         │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  3. Recevoir l'état du serveur (async)     │
│     - EntityUpdatePacket                   │
│     - Mettre à jour le Registry local      │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  4. Rendre le jeu                          │
│     - RenderingSystem.update()             │
│     - SFML draw calls                      │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│  5. Attendre le prochain frame (16ms)      │
└─────────────────────────────────────────────┘
```

---

## Conventions de Code

### Style

Défini dans `.clang-format` :
- **Indent** : 4 espaces
- **Colonnes** : 120 caractères max
- **Style** : Google modifié
- **Pointeurs** : `Type* ptr` (aligné à gauche)

### Nommage

Défini dans `.clang-tidy` :
- **Classes/Structs** : `CamelCase` (ex: `PositionComponent`)
- **Fonctions** : `camelBack` (ex: `createEntity()`)
- **Variables** : `camelBack` (ex: `deltaTime`)
- **Membres privés** : `_camelBack` (ex: `_entityId`)
- **Constantes** : `UPPER_CASE` (ex: `MAX_PLAYERS`)
- **Namespaces** : `lower_case` (ex: `ecs`, `network`)

### Commits

**Conventional Commits** :

```
<type>(<scope>): <description>

feat(ecs): add CollisionComponent
fix(network): resolve packet deserialization bug
chore(ci): update GitHub Actions to v4
docs(readme): add build instructions
```

---

## Prochaines Étapes

1. **Implémenter les systèmes** : Compléter `PhysicsSystem`, `RenderingSystem`
2. **Tests unitaires** : Ajouter GTest pour tester les composants et systèmes
3. **Gameplay** : Ajouter ennemis, projectiles, power-ups
4. **Assets** : Intégrer sprites et musiques
5. **Optimisations** : Prediction côté client, interpolation

---

**Date de création** : 24 novembre 2025  
**Auteur** : Angel-42  
**Projet** : Air-Trap (Epitech)
