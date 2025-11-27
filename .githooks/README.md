# Git Hooks

Ce dossier contient les hooks Git personnalisés pour le projet Air-Trap.

## Installation

Pour activer les hooks, exécutez depuis la racine du projet :

```bash
./scripts/install-hooks.sh
```

## Hooks disponibles

### commit-msg

Valide le format des messages de commit selon la norme du projet.

**Format requis:** `[TYPE] description`

**Types valides:**
- `[ADD]` : Ajout d'une feature
- `[FIX]` : Corriger soucis d'une feature
- `[CHORE]` : Configurer files
- `[DOCS]` : Ajouter des instructions de documentation
- `[STYLE]` : Appliquer format de style au projet
- `[REFACTOR]` : Reformater le code

**Exemples valides:**
```
[ADD] UDP connection handler to server
[FIX] sprite rendering issue on client
[DOCS] build instructions in README
[REFACTOR] ECS component registration system
[CHORE] CMakeLists configuration for new dependencies
[STYLE] clang-format on server codebase
```

**Exemples invalides:**
```
❌ Added new feature
❌ Fixed bug
❌ WIP
❌ update code
❌ feat: add new feature (mauvais format)
```
