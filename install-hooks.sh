#!/bin/bash
# Script d'installation des Git hooks personnalisés

REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null)"

if [ -z "$REPO_ROOT" ]; then
    echo "❌ Erreur: Pas dans un dépôt Git"
    exit 1
fi

HOOKS_DIR="$REPO_ROOT/.githooks"
GIT_HOOKS_DIR="$REPO_ROOT/.git/hooks"

if [ ! -d "$HOOKS_DIR" ]; then
    echo "❌ Erreur: Dossier .githooks introuvable"
    exit 1
fi

echo "🔧 Installation des Git hooks..."
echo ""

# Installer chaque hook
for hook in "$HOOKS_DIR"/*; do
    if [ -f "$hook" ]; then
        hook_name=$(basename "$hook")
        target="$GIT_HOOKS_DIR/$hook_name"
        
        # Créer un lien symbolique
        ln -sf "$hook" "$target"
        chmod +x "$target"
        
        echo "✅ Installé: $hook_name"
    fi
done

echo ""
echo "✅ Installation terminée!"
echo ""
echo "Hooks installés:"
echo "  - commit-msg : Valide le format des messages de commit"
echo ""
echo "Format requis pour les commits:"
echo "  [TYPE] description"
echo ""
echo "Types valides:"
echo "  [ADD], [FIX], [CHORE], [DOCS], [STYLE], [REFACTOR]"
echo ""
echo "Exemples:"
echo "  [ADD] player connection handler to server"
echo "  [FIX] rendering bug on client startup"
echo "  [DOCS] build instructions in README"
echo ""
