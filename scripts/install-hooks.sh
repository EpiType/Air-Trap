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
        # L'option -f force l'écrasement si le lien existe déjà
        ln -sf "$hook" "$target"
        chmod +x "$target"
        
        echo "✅ Installé: $hook_name"
    fi
done

echo ""
echo "✅ Installation terminée!"
echo ""
echo "Hooks actifs :"
echo "---------------------------------------------------"
if [ -f "$GIT_HOOKS_DIR/commit-msg" ]; then
    echo "📝 commit-msg :"
    echo "   Valide que le message respecte : [TYPE] description"
fi
if [ -f "$GIT_HOOKS_DIR/pre-commit" ]; then
    echo "🏗️  pre-commit :"
    echo "   Compile le projet avant chaque commit."
    echo "   Si la compilation échoue, le commit est bloqué."
fi
if [ -f "$GIT_HOOKS_DIR/pre-push" ] && [ -x "$GIT_HOOKS_DIR/pre-push" ]; then
    echo "🚀 pre-push   :"
    echo "   Compile le projet avant de push vers 'dev'."
    echo "   (Actuellement désactivé au profit de pre-commit)"
fi
echo "---------------------------------------------------"
echo ""
echo "Rappel des types de commits valides:"
echo "  [ADD], [FIX], [CHORE], [DOCS], [STYLE], [DEL], [REFACTOR], [MERGE]"
echo ""