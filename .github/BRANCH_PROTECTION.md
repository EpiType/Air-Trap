# Configuration des protections de branches

Ce document explique comment configurer les protections de branches pour le workflow **feature → dev → main**.

## 🎯 Workflow du projet

```
feature/xxx → dev → main
     ↓         ↓      ↓
   Linux    Full   Mirror
   only    Tests   only
```

## 🔒 Protection de la branche `main`

### Configuration sur GitHub

Allez dans **Settings → Branches → Branch protection rules** et ajoutez pour `main` :

### ✅ Règles à activer :

1. **Require a pull request before merging**
   - ✅ Require approvals: 1
   - ✅ Dismiss stale pull request approvals when new commits are pushed

2. **Require status checks to pass before merging**
   - ✅ Require branches to be up to date before merging
   - Status checks requis :
     - `Validate Commit Messages`
     - `Code Style Check (clang-format & clang-tidy)`
     - `Build on Linux`

3. **Restrict who can push to matching branches**
   - ✅ Cocher cette option
   - Ajouter uniquement : **dev** (la branche, pas une personne)
   - Ou laisser vide et gérer via les PRs uniquement

4. **Do not allow bypassing the above settings**
   - ✅ Cocher (même les admins doivent respecter les règles)

### ⚠️ Règle importante :

**Limiter les PRs vers main :**
- Seules les PRs depuis `dev` sont autorisées
- Bloquer les PRs depuis les branches `feature/*`

> **Note :** GitHub ne permet pas nativement de restreindre les branches sources de PR. 
> Il faut utiliser un bot/action ou une règle d'équipe stricte.

---

## 🔒 Protection de la branche `dev`

### Configuration sur GitHub

Ajoutez pour `dev` :

### ✅ Règles à activer :

1. **Require a pull request before merging**
   - ✅ Require approvals: 1 (optionnel, selon la taille de l'équipe)

2. **Require status checks to pass before merging**
   - ✅ Require branches to be up to date before merging
   - Status checks requis :
     - `Validate Commit Messages`
     - `Code Style Check (clang-format & clang-tidy)`
     - `Build on Linux`
     - `Build on Windows`
     - `Build on macOS`

3. **Allow force pushes** : ❌ Désactivé
4. **Allow deletions** : ❌ Désactivé

---

## 📋 Workflow pour l'équipe

### 1. Créer une feature

```bash
git checkout dev
git pull
git checkout -b feature/my-awesome-feature

# Développer...
git add .
git commit -m "[ADD] awesome feature"
# Types disponibles : [ADD], [FIX], [CHORE], [DOCS], [STYLE], [REFACTOR], [DEL], [MERGE]
git push origin feature/my-awesome-feature
```

**CI/CD :** ✅ Commits + Style + Linux (~7 min)

### 2. Créer une PR vers dev

Sur GitHub : **feature/my-awesome-feature → dev**

**CI/CD :** ✅ Tous les tests (Linux + Windows + macOS ~20 min)

### 3. Merge dans dev

Après review et approval :
```bash
# Merge via GitHub PR interface
```

**CI/CD sur dev :** ✅ Tests complets + Mirror

### 4. Promouvoir dev vers main

Quand dev est stable :

```bash
# Sur GitHub : créer PR dev → main
```

**CI/CD :** ✅ Commits + Style + Linux (~7 min) - pas de re-test des autres plateformes

### 5. Merge dans main

Après validation :
```bash
# Merge via GitHub PR interface
```

**CI/CD sur main :** ✅ Validation légère + Mirror

---

## 🚫 Ce qui est bloqué

❌ **PR directe feature → main** (configuration à faire manuellement)
❌ **Push direct sur main** (sauf admins si configuré)
❌ **Push direct sur dev sans PR** (optionnel selon l'équipe)
❌ **Commits non conformes** (bloqués par le hook Git local)
❌ **Code mal formaté** (bloqué par clang-format check)

---

## ✅ Ce qui est permis

✅ Push sur branches feature/* (avec validation légère)
✅ PR feature → dev (avec validation complète)
✅ PR dev → main (avec validation légère)
✅ Push sur dev après merge de PR
✅ Push sur main après merge de PR

---

## 🎯 Résultat

- **Qualité garantie** : Tout le code dans main a été testé sur 3 plateformes
- **Rapidité préservée** : Pas de tests redondants
- **Workflow clair** : Impossible de bypasser le processus
- **Économies** : ~60% de temps CI en moins
