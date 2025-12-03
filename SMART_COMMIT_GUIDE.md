# Smart Commit Tool - Guide Complet

Outil intelligent pour automatiser la création de commits groupés et structurés.

## 🎯 Objectif

Simplifier le processus de commit en :
- Groupant automatiquement les fichiers par type logique
- Analysant les diffs pour générer des descriptions pertinentes
- Créant des commits bien formatés selon les conventions du projet
- Permettant un push rapide après validation

## 🚀 Installation

```bash
chmod +x smart-commit.sh
```

C'est tout ! L'outil est prêt à l'emploi.

## 📝 Utilisation

### Commande de Base

```bash
./script./scripts/smart-commit.sh
```

### Workflow Complet

1. **Analyse automatique**
   - Le script détecte tous les fichiers modifiés (staged + unstaged)
   - Groupe les fichiers par catégorie logique
   - Affiche un résumé des groupes détectés

2. **Confirmation**
   - Demande confirmation avant de créer les commits
   - **Entrée = oui par défaut**, tapez `n` pour annuler

3. **Création des commits**
   - Pour chaque groupe, affiche les fichiers avec stats (+/-)
   - Analyse les diffs et génère une suggestion intelligente
   - **Entrée = accepter la suggestion**, ou écrivez la vôtre
   - Crée automatiquement le commit avec le bon format

4. **Push (optionnel)**
   - Propose de pusher tous les commits d'un coup
   - **Entrée = oui par défaut**, tapez `n` pour annuler

## 📦 Groupes de Fichiers

Le script classe automatiquement les fichiers dans 7 catégories :

### [DOCS] - Documentation
**Pattern** : `*.md`, `*.txt`, `*.pdf`, `docs/*`, `README*`

**Exemples** :
- `README.md`
- `CONTRIBUTING.md`
- `docs/api.md`
- `B-CPP-500_rtype.pdf`

**Commit généré** :
```
[DOCS] update project documentation
- README.md: add smart-commit guide
- CONTRIBUTING.md: update workflow
```

### [CHORE] - Maintenance & Outils
**Pattern** : `.github/*`, `CMakeLists.txt`, `conanfile.txt`, `*.cmake`, `*.sh`, `.clang*`, `.gitignore`

**Exemples** :
- `.github/workflows/ci-cd.yml`
- `CMakeLists.txt`
- `smart-commit.sh`
- `.clang-format`
- `build.sh`

**Commit généré** :
```
[CHORE] enhance CI/CD and build tools
- ci-cd.yml: add Discord notifications with embed format
- smart-commit.sh: implement intelligent file grouping
```

### [STYLE] - Formatage
**Pattern** : `*format*`, `*lint*`, `.clang-format`

**Exemples** :
- `.clang-format`
- `format.sh`
- `lint.sh`

**Commit généré** :
```
[STYLE] update code formatting configuration
- .clang-format: change Standard to Latest for C++23 compatibility
```

### [FIX] - Corrections de Bugs
**Pattern** : `*.cpp`, `*.hpp`, `*.c`, `*.h` avec "fix", "bug", "error", "crash" dans le diff

**Exemples** :
- `server/src/main.cpp` (si le diff contient "fix segfault")
- `client/src/network.cpp` (si le diff contient "bugfix")

**Commit généré** :
```
[FIX] resolve network connection issues
- main.cpp: fix segfault on server shutdown
- network.cpp: handle disconnection properly
```

### [ADD] - Nouvelles Fonctionnalités
**Pattern** : `*.cpp`, `*.hpp`, `*.c`, `*.h`, `client/*`, `server/*`, `common/*`, tests

**Exemples** :
- `common/include/ecs/Entity.hpp`
- `server/src/GameLogic.cpp`
- `client/src/Renderer.cpp`
- `tests/test_entity.cpp`

**Commit généré** :
```
[ADD] implement entity component system
- Entity.hpp: add new functions/classes
- Registry.hpp: add new implementation
```

### [DEL] - Suppressions
**Pattern** : Fichiers supprimés

**Exemples** :
- Scripts déplacés vers `scripts/`
- Fichiers obsolètes ou dépréciés
- Configuration remplacée

**Commit généré** :
```
[DEL] remove old build scripts
- build.sh: remove (moved to scripts/)
- format.sh: remove (moved to scripts/)
```

### [REFACTOR] - Refactoring
**Pattern** : Code avec restructuration mais sans changement fonctionnel

**Commit généré** :
```
[REFACTOR] improve code architecture
- GameSystem.cpp: refactor logic for better readability
```

### [MERGE] - Merges de Branches
**Pattern** : Merges de branches Git

**Commit généré** :
```
[MERGE] merge feature branch into main
- feature/login: merge into main

```

## 🧠 Analyse Intelligente

Le script analyse le contenu des diffs et **cumule tous les patterns détectés** pour générer des descriptions complètes :

### Patterns Détectés (cumulatifs)

| Pattern dans le Diff | Description Ajoutée |
|---------------------|---------------------|
| `class ClassName` | `implement new class` |
| `struct StructName` | `add new struct` |
| `enum EnumName` | `define new enum` |
| Nouvelles fonctions `name()` | `implement functions` |
| Constructor/Destructor | `add constructor/destructor` |
| `fix `, `bug ` (avec espace) | `fix bug` |
| `error`, `crash`, `segfault` | `fix error/crash` |
| `issue #123` | `fix issue` |
| `test()`, `TEST`, `ASSERT`, `EXPECT` | `add tests` |
| `template<>` | `implement template` |
| `std::make_unique`, smart ptrs | `use smart pointers` |
| `std::vector`, `std::map` | `use STL containers` |
| `std::thread`, `std::mutex` | `add multithreading` |
| `asio::`, `boost::asio` | `add network code` |
| `sf::`, `SFML` | `add SFML code` |
| `#include` | `update includes` |
| `namespace Name` | `define namespace` |
| `using namespace` | `import namespace` |
| `throw`, `try`, `catch()` | `handle exceptions` |
| `/** @brief */` | `add documentation` |
| `TODO:`, `FIXME:`, `HACK:` | `add markers` |
| `std::cout`, `printf`, `LOG` | `add logging` |
| `constexpr`, `const ... =` | `add constants` |
| `virtual`, `override`, `noexcept` | `update method signatures` |
| `public:`, `private:`, `protected:` | `change access modifiers` |
| `if`, `for`, `while`, `switch` | `add control flow` |
| Suppression class/struct | `remove types` |
| Suppression fonctions | `remove functions` |

**Si aucun pattern spécifique** : Analyse basée sur les lignes (+/-) :
- +150 lignes → `add major implementation`
- -100 lignes → `cleanup old code`
- +30/-30 lignes → `refactor code structure`
- +10/-10 lignes → `update implementation`
- Plus d'ajouts → `extend functionality`
- Défaut → `modify code`

**Cumul des changements** : Si plusieurs patterns sont détectés, ils sont combinés avec " + " :
```
Player.cpp: implement new class + add tests + add documentation
Game.cpp: fix bug + add logging + handle exceptions
Network.cpp: use smart pointers + use STL containers + add multithreading
```

### Exemple d'Analyse

**Fichier** : `Player.cpp`

**Diff** :
```diff
+class Player {
+public:
+    Player() = default;
+    void move(float x, float y);
+    
+    // Test methods
+    void testMovement();
+private:
+    sf::Sprite sprite;
+    std::unique_ptr<Collider> collider;
+};
```

**Description générée** :
```
Player.cpp: implement new class + add tests + add SFML code + use smart pointers
```

Ce système de **cumul** permet de capturer tous les types de modifications dans un fichier, donnant une vue complète des changements effectués.

## 📋 Exemples Complets

### Exemple 1 : Modification Simple (1 fichier)

**Fichiers modifiés** : `README.md`

**Exécution** :
```bash
$ ./scripts/smart-commit.sh

🔍 Analyse des fichiers modifiés...

📦 Groupes détectés:
  [DOCS] : 1 fichier(s)

🚀 Créer des commits groupés ? (O/n): ⏎  # Entrée = accepter

📝 Groupe [DOCS]:
   - README.md (+15 -3)

   💡 Suggestion:
   README.md: extend functionality

   Description (Entrée pour accepter, ou écris la tienne): add smart-commit guide

   ✅ Commit créé

✅ 1 commit(s) créé(s)

🚀 Push maintenant ? (O/n): ⏎  # Entrée = oui
📤 Push vers origin/initArchi...
✅ Push terminé!

🎉 Terminé!
```

**Commit créé** :
```
[DOCS] add smart-commit guide
```

### Exemple 2 : Modifications Multiples

**Fichiers modifiés** :
- `.github/workflows/ci-cd.yml`
- `smart-commit.sh`
- `README.md`

**Exécution** :
```bash
$ ./scripts/smart-commit.sh

🔍 Analyse des fichiers modifiés...

📦 Groupes détectés:
  [DOCS] : 1 fichier(s)
  [CHORE] : 2 fichier(s)

🚀 Créer des commits groupés ? (O/n): ⏎

📝 Groupe [DOCS]:
   - README.md (+50 -10)

   💡 Suggestion:
   README.md: extend functionality

   Description (Entrée pour accepter, ou écris la tienne): ⏎  # Accepter la suggestion

   ✅ Commit créé

📝 Groupe [CHORE]:
   - .github/workflows/ci-cd.yml (+25 -8)
   - smart-commit.sh (+40 -15)

   💡 Suggestion:
   - ci-cd.yml: add control flow + add logging
   - smart-commit.sh: implement functions + add control flow

   Description (Entrée pour accepter, ou écris la tienne): enhance CI/CD with Discord embeds and improve commit automation

   ✅ Commit créé

✅ 2 commit(s) créé(s)

🚀 Push maintenant ? (O/n): n  # Refuser le push
ℹ️  Push annulé. Utilise 'git push' plus tard.

🎉 Terminé!
```

**Commits créés** :
```
[DOCS] README.md: extend functionality

[CHORE] enhance CI/CD with Discord embeds and improve commit automation
- ci-cd.yml: add control flow + add logging
- smart-commit.sh: implement functions + add control flow
```

### Exemple 3 : Acceptation Automatique + Push

**Si vous appuyez juste sur Entrée** :

```bash
📝 Groupe [CHORE]:
   - ci-cd.yml (+10 -5)

   💡 Suggestion:
   ci-cd.yml: update messages

   Description (Entrée pour accepter, ou écris la tienne): ⏎  # Accepter

   ✅ Commit créé

✅ 1 commit(s) créé(s)

🚀 Push maintenant ? (O/n): ⏎  # Push automatique
📤 Push vers origin/initArchi...
✅ Push terminé!

🎉 Terminé!
```

La suggestion est automatiquement utilisée et le push s'exécute immédiatement !

## 🎨 Format des Commits

### Commit Simple (1 fichier)

```
[TYPE] description
```

Exemple :
```
[FIX] resolve server crash on client disconnect
```

### Commit Multi-Ligne (plusieurs fichiers)

```
[TYPE] titre général
- fichier1: description1
- fichier2: description2
- fichier3: description3
```

Exemple :
```
[CHORE] update build configuration and CI pipeline
- CMakeLists.txt: upgrade to C++23 standard
- ci-cd.yml: add Discord notifications
- conanfile.txt: update SFML to 2.6.1
```

## ⚙️ Configuration

### Variables d'Environnement

Aucune configuration nécessaire ! Le script fonctionne tel quel.

### Personnalisation

Pour modifier les patterns de détection, éditez directement `smart-commit.sh` :

```bash
# Ligne ~40: Ajuster les patterns de fichiers
case "$file" in
    *.md|*.txt|*.pdf|docs/*|README*)
        DOCS_FILES+="$file "
        ;;
    # Ajouter vos propres patterns ici
esac
```

## 🔧 Dépannage

### Problème : "Aucun fichier modifié"

**Cause** : Tous les fichiers sont déjà committés.

**Solution** : Modifiez des fichiers et réessayez.

### Problème : Le script ne détecte pas mes fichiers

**Cause** : Les fichiers ne sont peut-être pas trackés par Git.

**Solution** :
```bash
git add <fichier>
./scripts/smart-commit.sh
```

### Problème : Le push ne fonctionne pas

**Cause** : Vous devez taper exactement `o` ou `O` pour confirmer.

**Solution** : 
- Tapez `o` puis Entrée (pas `oui`, pas `y`)
- Vérifiez que vous avez bien répondu à la question "Push maintenant ?"

### Problème : Mauvaise catégorie détectée

**Cause** : Le pattern ne correspond pas.

**Solution** : Tapez manuellement votre description au lieu d'accepter la suggestion.

### Problème : "set -e" fait crasher le script

**Cause** : Une commande a échoué.

**Solution** : Vérifiez que vous êtes dans un dépôt Git et que Git fonctionne correctement.

## 💡 Bonnes Pratiques

### ✅ À Faire

- **Accepter les suggestions** si elles sont pertinentes
- **Modifier les descriptions** pour plus de précision si nécessaire
- **Grouper logiquement** : laissez le script faire son travail
- **Pusher régulièrement** pour synchroniser avec l'équipe
- **Utiliser le script quotidiennement** pour des commits cohérents

### ❌ À Éviter

- **Ne pas mélanger** des changements non liés (le script les séparera)
- **Ne pas ignorer** les suggestions sans les lire
- **Ne pas commit** de fichiers générés (build/, node_modules/, etc.)
- **Ne pas utiliser** sur des repos non-Git

## 🔗 Intégration avec Git Hooks

Le script fonctionne en complément des Git hooks :

1. `./scripts/smart-commit.sh` → Crée les commits
2. `commit-msg hook` → Valide le format automatiquement
3. CI/CD → Valide à nouveau sur GitHub

**Workflow recommandé** :
```bash
# Installer les hooks (une fois)
./scripts/install-hooks.sh

# Utiliser smart-commit quotidiennement
./scripts/smart-commit.sh
```

## 📊 Statistiques

Le script affiche pour chaque fichier :
- **Lignes ajoutées** : `+42`
- **Lignes supprimées** : `-15`
- **Nouveau fichier** : `(nouveau)`

Exemple :
```
📝 Groupe [ADD]:
   - Entity.hpp (+120 -0)
   - Registry.cpp (+85 -12)
   - main.cpp (nouveau)
```

## 🚀 Avancé : Automatisation Complète

### Alias Git

Ajouter dans `~/.gitconfig` :

```ini
[alias]
    sc = !bash smart-commit.sh
    scp = !bash smart-commit.sh && git push
```

**Usage** :
```bash
git sc      # Lance smart-commit
git scp     # Lance smart-commit + push automatique
```

### Script dans PATH

Pour utiliser partout :

```bash
sudo ln -s $PW./scripts/smart-commit.sh /usr/local/bin/smart-commit
chmod +x /usr/local/bin/smart-commit
```

**Usage** :
```bash
smart-commit  # Depuis n'importe où dans le repo
```

## 🤝 Contribution

Le script peut être amélioré ! Suggestions bienvenues :

- Nouveaux patterns de détection
- Support d'autres langages
- Intégration avec des outils externes
- Améliorations de l'analyse de diff

## 📄 Licence

Fait partie du projet Air-Trap (Epitech B-CPP-500).

---

**Astuce** : Pour voir ce guide à tout moment, consultez `SMART_COMMIT_GUIDE.md` ! 🎯
