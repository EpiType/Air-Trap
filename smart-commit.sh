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
            CHANGES_LIST=()
            
            # Analyse selon le type de fichier
            if [[ "$f" == *.md ]] || [[ "$f" == *.txt ]] || [[ "$f" == *.pdf ]]; then
                # Fichiers documentation : analyse basée sur les lignes
                if [ "$ADDED" -gt 100 ]; then
                    CHANGES_LIST+=("add major documentation")
                elif [ "$REMOVED" -gt 50 ]; then
                    CHANGES_LIST+=("remove old documentation")
                elif [ "$ADDED" -gt 20 ] && [ "$REMOVED" -gt 20 ]; then
                    CHANGES_LIST+=("update documentation")
                elif [ "$ADDED" -gt "$REMOVED" ]; then
                    CHANGES_LIST+=("extend documentation")
                else
                    CHANGES_LIST+=("modify documentation")
                fi
            elif [[ "$f" == *.yml ]] || [[ "$f" == *.yaml ]]; then
                # Fichiers YAML/CI-CD : analyse basée sur les lignes
                if [ "$ADDED" -gt 50 ]; then
                    CHANGES_LIST+=("add major workflow changes")
                elif [ "$REMOVED" -gt 30 ]; then
                    CHANGES_LIST+=("remove old workflow steps")
                elif [ "$ADDED" -gt 15 ] && [ "$REMOVED" -gt 15 ]; then
                    CHANGES_LIST+=("update workflow configuration")
                elif [ "$ADDED" -gt "$REMOVED" ]; then
                    CHANGES_LIST+=("extend workflow")
                else
                    CHANGES_LIST+=("modify workflow")
                fi
            elif [[ "$f" == CMakeLists.txt ]] || [[ "$f" == *.cmake ]]; then
                # Fichiers CMake : analyse basée sur les lignes
                if [ "$ADDED" -gt 30 ]; then
                    CHANGES_LIST+=("add major build changes")
                elif [ "$REMOVED" -gt 20 ]; then
                    CHANGES_LIST+=("remove old build config")
                elif [ "$ADDED" -gt 10 ] && [ "$REMOVED" -gt 10 ]; then
                    CHANGES_LIST+=("update build configuration")
                elif [ "$ADDED" -gt "$REMOVED" ]; then
                    CHANGES_LIST+=("extend build config")
                else
                    CHANGES_LIST+=("modify build config")
                fi
            elif [[ "$f" == conanfile.* ]]; then
                # Fichiers Conan : analyse basée sur les lignes
                if [ "$ADDED" -gt 10 ]; then
                    CHANGES_LIST+=("add dependencies")
                elif [ "$REMOVED" -gt 5 ]; then
                    CHANGES_LIST+=("remove dependencies")
                else
                    CHANGES_LIST+=("update dependencies")
                fi
            elif [[ "$f" == *.sh ]]; then
                # Scripts shell : détection générale des patterns bash
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*function [a-zA-Z_][a-zA-Z0-9_]*\(\)"; then
                    CHANGES_LIST+=("add functions")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(if|elif|case) .*then"; then
                    CHANGES_LIST+=("add conditionals")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(for|while) .*do"; then
                    CHANGES_LIST+=("add loops")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\$\(.*grep.*\)"; then
                    CHANGES_LIST+=("add text processing")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(echo|printf) .*[\"']"; then
                    CHANGES_LIST+=("add output")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*#.*"; then
                    CHANGES_LIST+=("add comments")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*read -p"; then
                    CHANGES_LIST+=("add user input")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(git|docker|cmake|conan) "; then
                    CHANGES_LIST+=("add tool commands")
                fi
                
                # Fallback si aucun pattern
                if [ ${#CHANGES_LIST[@]} -eq 0 ]; then
                    if [ "$ADDED" -gt 100 ]; then
                        CHANGES_LIST+=("add major script logic")
                    elif [ "$REMOVED" -gt 50 ]; then
                        CHANGES_LIST+=("remove old script code")
                    elif [ "$ADDED" -gt 30 ] && [ "$REMOVED" -gt 30 ]; then
                        CHANGES_LIST+=("refactor script structure")
                    elif [ "$ADDED" -gt 10 ] && [ "$REMOVED" -gt 10 ]; then
                        CHANGES_LIST+=("update script logic")
                    else
                        CHANGES_LIST+=("modify script")
                    fi
                fi
            elif [[ "$f" == .clang-format ]] || [[ "$f" == .clang-tidy ]] || [[ "$f" == *format* ]] || [[ "$f" == *lint* ]]; then
                # Fichiers de configuration de style
                CHANGES_LIST+=("update code style rules")
            elif [[ "$f" == .gitignore ]] || [[ "$f" == .gitattributes ]]; then
                # Fichiers Git
                if [ "$ADDED" -gt "$REMOVED" ]; then
                    CHANGES_LIST+=("add ignore rules")
                else
                    CHANGES_LIST+=("update ignore rules")
                fi
            elif [[ "$f" == *.json ]]; then
                # Fichiers JSON : analyse basée sur les lignes
                if [ "$ADDED" -gt 20 ]; then
                    CHANGES_LIST+=("add configuration")
                elif [ "$REMOVED" -gt 10 ]; then
                    CHANGES_LIST+=("remove configuration")
                else
                    CHANGES_LIST+=("update configuration")
                fi
            elif [[ "$f" == *.cpp ]] || [[ "$f" == *.hpp ]] || [[ "$f" == *.c ]] || [[ "$f" == *.h ]] || [[ "$f" == *.cc ]] || [[ "$f" == *.cxx ]]; then
                # Fichiers C/C++ : analyse détaillée
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\bclass [A-Z][a-zA-Z0-9_]* "; then
                    CHANGES_LIST+=("implement new class")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\bstruct [A-Z][a-zA-Z0-9_]* "; then
                    CHANGES_LIST+=("add new struct")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\benum (class )?[A-Z][a-zA-Z0-9_]* "; then
                    CHANGES_LIST+=("define new enum")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*[a-zA-Z_][a-zA-Z0-9_]*\([^)]*\) *\{"; then
                    CHANGES_LIST+=("implement functions")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*~[A-Z][a-zA-Z0-9_]*\("; then
                    CHANGES_LIST+=("add constructor/destructor")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* // .*(fix|Fix|bug|Bug)"; then
                    CHANGES_LIST+=("fix bug")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* // .*(error|Error|crash|Crash)"; then
                    CHANGES_LIST+=("fix error/crash")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* // .*issue #[0-9]"; then
                    CHANGES_LIST+=("fix issue")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(TEST_|ASSERT_|EXPECT_)[A-Z]+"; then
                    CHANGES_LIST+=("add tests")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\btemplate *<"; then
                    CHANGES_LIST+=("implement template")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(std::make_unique|std::make_shared|std::unique_ptr|std::shared_ptr)<"; then
                    CHANGES_LIST+=("use smart pointers")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(std::vector|std::map|std::unordered_map|std::set|std::array)<"; then
                    CHANGES_LIST+=("use STL containers")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(std::thread|std::mutex|std::lock_guard|std::async|std::future)<"; then
                    CHANGES_LIST+=("add multithreading")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(asio::[a-z_]+|boost::asio::[a-z_]+)"; then
                    CHANGES_LIST+=("add network code")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\bsf::[A-Z][a-zA-Z]+"; then
                    CHANGES_LIST+=("add SFML code")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*#include *[<\"]"; then
                    CHANGES_LIST+=("update includes")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\bnamespace [a-zA-Z_][a-zA-Z0-9_]* *\{"; then
                    CHANGES_LIST+=("define namespace")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\busing namespace [a-zA-Z_]"; then
                    CHANGES_LIST+=("import namespace")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(throw [a-zA-Z_]|std::runtime_error|std::logic_error|\btry *\{|\bcatch *)"; then
                    CHANGES_LIST+=("handle exceptions")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* /// .* @(brief|param|return)"; then
                    CHANGES_LIST+=("add documentation")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* // .*(TODO|FIXME|XXX|HACK|NOTE):"; then
                    CHANGES_LIST+=("add markers")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*(std::cout|std::cerr) *<<"; then
                    CHANGES_LIST+=("add logging")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].*\b(constexpr|const) [a-zA-Z_][a-zA-Z0-9_]* *="; then
                    CHANGES_LIST+=("add constants")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* (virtual|override|noexcept) "; then
                    CHANGES_LIST+=("update method signatures")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+](public|private|protected):"; then
                    CHANGES_LIST+=("change access modifiers")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\+[^+].* (if|for|while|switch) *\("; then
                    CHANGES_LIST+=("add control flow")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\-[^-].*\b(class|struct|enum) [A-Z]"; then
                    CHANGES_LIST+=("remove types")
                fi
                if echo "$DIFF_CONTENT" | grep -qE "^\-[^-].*[a-zA-Z_][a-zA-Z0-9_]*\([^)]*\) *\{"; then
                    CHANGES_LIST+=("remove functions")
                fi
            fi
            
            # Construire le résumé à partir des changements détectés
            if [ ${#CHANGES_LIST[@]} -eq 0 ]; then
                # Aucun pattern spécifique, analyser les lignes
                if [ "$ADDED" -gt 150 ]; then
                    CHANGE_SUMMARY="add major implementation"
                elif [ "$REMOVED" -gt 100 ]; then
                    CHANGE_SUMMARY="cleanup old code"
                elif [ "$ADDED" -gt 30 ] && [ "$REMOVED" -gt 30 ]; then
                    CHANGE_SUMMARY="refactor code structure"
                elif [ "$ADDED" -gt 10 ] && [ "$REMOVED" -gt 10 ]; then
                    CHANGE_SUMMARY="update implementation"
                elif [ "$ADDED" -gt "$REMOVED" ]; then
                    CHANGE_SUMMARY="extend functionality"
                else
                    CHANGE_SUMMARY="modify code"
                fi
            elif [ ${#CHANGES_LIST[@]} -eq 1 ]; then
                # Un seul changement
                CHANGE_SUMMARY="${CHANGES_LIST[0]}"
            else
                # Plusieurs changements : joindre avec " + "
                CHANGE_SUMMARY=$(printf "%s + " "${CHANGES_LIST[@]}" | sed 's/ + $//')
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
