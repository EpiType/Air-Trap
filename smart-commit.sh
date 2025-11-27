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
if [ -n "$confirm" ] && [ "$confirm" != "o" ] && [ "$confirm" != "O" ]; then
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
    
    # Générer une description automatique basée sur les fichiers et leurs changements
    AUTO_DESC=""
    FILE_COUNT=$(echo $files | wc -w)
    CHANGES_DESC=""
    
    # Afficher les fichiers avec leurs changements
    for f in $files; do
        # Vérifier si le fichier est nouveau
        if git diff --cached --name-only 2>/dev/null | grep -q "^$f$"; then
            CHANGES=$(git diff --cached --numstat "$f" 2>/dev/null || echo "")
            DIFF_CONTENT=$(git diff --cached "$f" 2>/dev/null || echo "")
        else
            CHANGES=$(git diff --numstat "$f" 2>/dev/null || echo "")
            DIFF_CONTENT=$(git diff "$f" 2>/dev/null || echo "")
        fi
        
        if [ -n "$CHANGES" ]; then
            ADDED=$(echo "$CHANGES" | awk '{print $1}')
            REMOVED=$(echo "$CHANGES" | awk '{print $2}')
            echo "   - $f (+$ADDED -$REMOVED)"
            
            # Analyser le contenu du diff pour extraire les changements importants
            FILENAME=$(basename "$f")
            CHANGE_SUMMARY=""
            
            # Détecter les patterns dans le diff
            if echo "$DIFF_CONTENT" | grep -qE "^\+.*function |^\+.*def |^\+.*class |^\+.*struct |^\+.*enum "; then
                CHANGE_SUMMARY="add new functions/classes"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(fix|Fix|FIX|bug|Bug|BUG|error|Error|crash|Crash|issue|Issue)"; then
                CHANGE_SUMMARY="fix bugs"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(TODO|FIXME|XXX|HACK|NOTE|@todo|@fixme)"; then
                CHANGE_SUMMARY="add TODOs"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(\/\/|\/\*|\*\/|#|comment|Comment|@brief|@param|@return)"; then
                CHANGE_SUMMARY="improve comments"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(echo|print|printf|cout|cerr|STATUS_MSG|MESSAGE|LOG|log|Log|std::cout|std::cerr)"; then
                CHANGE_SUMMARY="update messages"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(test|Test|TEST|ASSERT|assert|expect|EXPECT|describe|it\()"; then
                CHANGE_SUMMARY="add tests"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(if |for |while |switch |case |else |elif |try |catch )"; then
                CHANGE_SUMMARY="refactor logic"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(const |static |inline |virtual |override |constexpr |noexcept |explicit )"; then
                CHANGE_SUMMARY="update implementation"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*#include|^\+.*import "; then
                CHANGE_SUMMARY="update includes"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(namespace|using |typedef |using namespace)"; then
                CHANGE_SUMMARY="update namespaces"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(std::|boost::|asio::|sf::)"; then
                CHANGE_SUMMARY="add library calls"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(auto |int |void |bool |char |float |double |string |vector |map |pair )"; then
                CHANGE_SUMMARY="add variables"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(public:|private:|protected:)"; then
                CHANGE_SUMMARY="update access modifiers"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(throw |exception|Exception|std::runtime_error|std::logic_error)"; then
                CHANGE_SUMMARY="add error handling"
            elif echo "$DIFF_CONTENT" | grep -qE "^\+.*(template|typename|<.*>)"; then
                CHANGE_SUMMARY="add template code"
            elif echo "$DIFF_CONTENT" | grep -qE "^\-.*(function|class|struct|enum)"; then
                CHANGE_SUMMARY="remove old code"
            elif echo "$DIFF_CONTENT" | grep -qE "^\-.*(if |for |while |switch )"; then
                CHANGE_SUMMARY="simplify logic"
            elif [ "$ADDED" -gt 100 ]; then
                CHANGE_SUMMARY="add new implementation"
            elif [ "$REMOVED" -gt 50 ]; then
                CHANGE_SUMMARY="remove old code"
            elif [ "$ADDED" -gt 20 ] && [ "$REMOVED" -gt 20 ]; then
                CHANGE_SUMMARY="refactor implementation"
            else
                CHANGE_SUMMARY="update implementation"
            fi
            
            if [ -z "$CHANGES_DESC" ]; then
                CHANGES_DESC="$FILENAME: $CHANGE_SUMMARY"
            else
                CHANGES_DESC="$CHANGES_DESC, $FILENAME: $CHANGE_SUMMARY"
            fi
        else
            echo "   - $f (nouveau)"
            FILENAME=$(basename "$f")
            CHANGES_DESC="$CHANGES_DESC, add $FILENAME"
        fi
    done
    
    # Suggérer une description automatique intelligente
    if [ $FILE_COUNT -eq 1 ]; then
        AUTO_DESC="$CHANGES_DESC"
    else
        # Formater avec bullet points pour multi-fichiers
        AUTO_DESC=""
        for f in $files; do
            FILENAME=$(basename "$f")
            # Extraire le résumé du changement pour ce fichier
            FILE_CHANGE=$(echo "$CHANGES_DESC" | grep -o "$FILENAME: [^,]*" | sed "s/$FILENAME: //")
            if [ -z "$AUTO_DESC" ]; then
                AUTO_DESC="- $FILENAME: $FILE_CHANGE"
            else
                AUTO_DESC="$AUTO_DESC\n- $FILENAME: $FILE_CHANGE"
            fi
        done
    fi
    
    echo ""
    echo -e "   💡 Suggestion:\n$AUTO_DESC" | sed 's/^/   /'
    
    read -p "   Description (Entrée pour accepter, ou écris la tienne): " description
    
    # Utiliser la suggestion si vide
    if [ -z "$description" ]; then
        description="$AUTO_DESC"
    fi
    
    # Git add les fichiers du groupe
    git add $files
    
    # Formater le message de commit
    if [ $FILE_COUNT -gt 1 ] && [[ "$description" == *$'\n'* ]]; then
        # Multi-ligne : titre + détails
        FIRST_LINE=$(echo -e "$description" | head -1)
        REST_LINES=$(echo -e "$description" | tail -n +2)
        git commit -m "[$type] $FIRST_LINE" -m "$REST_LINES"
    else
        # Simple ligne
        git commit -m "[$type] $description"
    fi
    
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
