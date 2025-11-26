#!/bin/bash
# Smart Commit Tool - Groupe et commit automatiquement par logique

set -e

REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null)"
if [ -z "$REPO_ROOT" ]; then
    echo "❌ Pas dans un dépôt Git"
    exit 1
fi

cd "$REPO_ROOT"

# Vérifier s'il y a des changements
if git diff --quiet && git diff --cached --quiet; then
    echo "✅ Aucun changement à commit"
    exit 0
fi

echo "🔍 Analyse des fichiers modifiés..."
echo ""

# Récupérer tous les fichiers modifiés (staged + unstaged)
MODIFIED_FILES=$(git status --porcelain | grep -E "^(M| M|A| A|D| D|\?\?)" | awk '{print $2}')

if [ -z "$MODIFIED_FILES" ]; then
    echo "✅ Aucun fichier modifié"
    exit 0
fi

# Variables pour grouper les fichiers par catégorie
DOCS_FILES=""
CHORE_FILES=""
STYLE_FILES=""
ADD_FILES=""
FIX_FILES=""
REFACTOR_FILES=""

while IFS= read -r file; do
    case "$file" in
        *.md|*.txt|*.pdf|docs/*|README*)
            DOCS_FILES+="$file "
            ;;
        .github/*|CMakeLists.txt|conanfile.txt|*.cmake|*.sh|.clang*|.gitignore)
            CHORE_FILES+="$file "
            ;;
        *format*|*lint*|.clang-format)
            STYLE_FILES+="$file "
            ;;
        *test*.cpp|*test*.hpp|tests/*)
            ADD_FILES+="$file "
            ;;
        *.cpp|*.hpp|*.c|*.h|client/*|server/*|common/*)
            # Détecter si c'est un fix ou une feature
            if git diff "$file" 2>/dev/null | grep -qE "fix|bug|error|crash"; then
                FIX_FILES+="$file "
            else
                ADD_FILES+="$file "
            fi
            ;;
        *)
            ADD_FILES+="$file "
            ;;
    esac
done <<< "$MODIFIED_FILES"

# Afficher les groupes détectés
echo "📦 Groupes détectés:"
if [ -n "$DOCS_FILES" ]; then echo "  [DOCS] : $(echo $DOCS_FILES | wc -w) fichier(s)"; fi
if [ -n "$CHORE_FILES" ]; then echo "  [CHORE] : $(echo $CHORE_FILES | wc -w) fichier(s)"; fi
if [ -n "$STYLE_FILES" ]; then echo "  [STYLE] : $(echo $STYLE_FILES | wc -w) fichier(s)"; fi
if [ -n "$FIX_FILES" ]; then echo "  [FIX] : $(echo $FIX_FILES | wc -w) fichier(s)"; fi
if [ -n "$ADD_FILES" ]; then echo "  [ADD] : $(echo $ADD_FILES | wc -w) fichier(s)"; fi
if [ -n "$REFACTOR_FILES" ]; then echo "  [REFACTOR] : $(echo $REFACTOR_FILES | wc -w) fichier(s)"; fi
echo ""

# Demander confirmation
read -p "🚀 Créer des commits groupés ? (o/n): " confirm
if [ "$confirm" != "o" ] && [ "$confirm" != "O" ]; then
    echo "❌ Annulé"
    exit 0
fi

echo ""

# Créer les commits par groupe
COMMIT_COUNT=0

for type in DOCS CHORE STYLE FIX ADD REFACTOR; do
    # Récupérer les fichiers du groupe selon le type
    case "$type" in
        DOCS) files="$DOCS_FILES" ;;
        CHORE) files="$CHORE_FILES" ;;
        STYLE) files="$STYLE_FILES" ;;
        FIX) files="$FIX_FILES" ;;
        ADD) files="$ADD_FILES" ;;
        REFACTOR) files="$REFACTOR_FILES" ;;
    esac
    
    if [ -z "$files" ]; then
        continue
    fi
    
    echo "📝 Groupe [$type]:"
    
    # Générer une description automatique basée sur les fichiers
    AUTO_DESC=""
    FILE_COUNT=$(echo $files | wc -w)
    
    # Afficher les fichiers avec leurs changements
    for f in $files; do
        # Vérifier si le fichier est nouveau
        if git diff --cached --name-only | grep -q "^$f$"; then
            CHANGES=$(git diff --cached --numstat "$f" 2>/dev/null || echo "")
        else
            CHANGES=$(git diff --numstat "$f" 2>/dev/null || echo "")
        fi
        
        if [ -n "$CHANGES" ]; then
            ADDED=$(echo "$CHANGES" | awk '{print $1}')
            REMOVED=$(echo "$CHANGES" | awk '{print $2}')
            echo "   - $f (+$ADDED -$REMOVED)"
        else
            echo "   - $f (nouveau)"
        fi
    done
    
    # Suggérer une description automatique
    if [ $FILE_COUNT -eq 1 ]; then
        SINGLE_FILE=$(echo $files | xargs basename)
        AUTO_DESC="update $SINGLE_FILE"
    else
        # Extraire les noms de dossiers/patterns communs
        COMMON_DIR=$(echo $files | tr ' ' '\n' | xargs -n1 dirname | sort | uniq -c | sort -rn | head -1 | awk '{print $2}')
        if [ "$COMMON_DIR" = "." ]; then
            AUTO_DESC="update $FILE_COUNT files"
        else
            AUTO_DESC="update $(basename $COMMON_DIR) ($FILE_COUNT files)"
        fi
    fi
    
    echo ""
    echo "   💡 Suggestion: $AUTO_DESC"
    read -p "   Description (Entrée pour accepter): " description
    
    # Utiliser la suggestion si vide
    if [ -z "$description" ]; then
        description="$AUTO_DESC"
    fi
    
    # Git add les fichiers du groupe
    git add $files
    
    # Commit
    git commit -m "[$type] $description"
    
    COMMIT_COUNT=$((COMMIT_COUNT + 1))
    echo "   ✅ Commit créé"
    echo ""
done

if [ $COMMIT_COUNT -eq 0 ]; then
    echo "⚠️  Aucun commit créé"
    exit 0
fi

echo "✅ $COMMIT_COUNT commit(s) créé(s)"
echo ""

# Demander si on push
read -p "🚀 Push maintenant ? (o/n): " push_confirm

if [ "$push_confirm" = "o" ] || [ "$push_confirm" = "O" ]; then
    BRANCH=$(git branch --show-current)
    echo "📤 Push vers origin/$BRANCH..."
    git push origin "$BRANCH"
    echo "✅ Push terminé!"
else
    echo "ℹ️  Push annulé. Utilise 'git push' plus tard."
fi

echo ""
echo "🎉 Terminé!"
