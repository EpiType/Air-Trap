#!/bin/bash
# Smart Commit Tool - Groupe et commit automatiquement par logique

set -e

# Helper function to append file to group with newline separator
append_to_group() {
    local group_name="$1"
    local file="$2"
    local current_value
    eval "current_value=\"\$$group_name\""
    if [ -n "$current_value" ]; then
        eval "$group_name+=$'\\n'\"\$file\""
    else
        eval "$group_name=\"\$file\""
    fi
}

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

# echo "🔧 Application du style de code C++..."
# python .coding-style/cpp-coding-style.py

echo "🔍 Analyse des fichiers modifiés..."
echo ""

# Récupérer tous les fichiers modifiés (staged + unstaged)
# D'abord, stager tous les fichiers pour que git détecte les renames
git add -A 2>/dev/null || true

STAGED_STATUS=$(git status --porcelain)

# Détecter les fichiers déplacés (renamed) AVANT le reset
# Format: "R  old -> new" ou "R100 old -> new"
RENAMED_FILES=$(echo "$STAGED_STATUS" | grep -E "^R" | sed -E 's/^R[0-9 ]*//' | sed 's/^ *//')

# Unstage tout pour pouvoir stager sélectivement par groupe
git reset >/dev/null 2>&1

# Maintenant récupérer le statut unstaged
ALL_STATUS=$(git status --porcelain)

# Détecter les fichiers supprimés (format: " D filename")
DELETED_FILES=$(echo "$ALL_STATUS" | grep -E "^ D" | awk '{print $2}')

# Fichiers modifiés/ajoutés (format: " M filename", "?? filename")
MODIFIED_FILES=$(echo "$ALL_STATUS" | grep -E "^( M|\?\?)" | awk '{print $2}')

# Exclure les fichiers impliqués dans des renames de MODIFIED_FILES et DELETED_FILES
if [ -n "$RENAMED_FILES" ]; then
    while IFS= read -r rename_line; do
        [ -z "$rename_line" ] && continue
        old_path=$(echo "$rename_line" | awk '{print $1}')
        new_path=$(echo "$rename_line" | awk '{print $3}')
        # Retirer old et new des listes
        DELETED_FILES=$(echo "$DELETED_FILES" | grep -v "^$old_path$" || true)
        MODIFIED_FILES=$(echo "$MODIFIED_FILES" | grep -v "^$new_path$" || true)
    done <<< "$RENAMED_FILES"
fi

if [ -z "$MODIFIED_FILES" ] && [ -z "$DELETED_FILES" ] && [ -z "$RENAMED_FILES" ]; then
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
DEL_FILES=""

# Traiter les fichiers supprimés
if [ -n "$DELETED_FILES" ]; then
    while IFS= read -r file; do
        [ -z "$file" ] && continue
        # Tous les fichiers supprimés vont dans DEL_FILES
        append_to_group "DEL_FILES" "$file"
    done <<< "$DELETED_FILES"
fi

# Traiter les fichiers déplacés (renamed)
if [ -n "$RENAMED_FILES" ]; then
    while IFS= read -r rename_line; do
        [ -z "$rename_line" ] && continue
        # Extraire l'ancien et le nouveau chemin
        old_path=$(echo "$rename_line" | awk '{print $1}')
        new_path=$(echo "$rename_line" | awk '{print $3}')
        
        # Grouper selon le nouveau chemin
        case "$new_path" in
            *.md|*.txt|*.pdf|docs/*|README*)
                append_to_group "DOCS_FILES" "$rename_line"
                ;;
            .github/*|CMakeLists.txt|conanfile.txt|*.cmake|*.sh|.clang*|.gitignore|scripts/*)
                append_to_group "CHORE_FILES" "$rename_line"
                ;;
            *format*|*lint*|.clang-format)
                append_to_group "STYLE_FILES" "$rename_line"
                ;;
            *)
                append_to_group "REFACTOR_FILES" "$rename_line"
                ;;
        esac
    done <<< "$RENAMED_FILES"
fi

# Traiter les fichiers modifiés/ajoutés
if [ -n "$MODIFIED_FILES" ]; then
    while IFS= read -r file; do
        [ -z "$file" ] && continue
        case "$file" in
            *.md|*.txt|*.pdf|docs/*|README*)
                append_to_group "DOCS_FILES" "$file"
                ;;
            .github/*|CMakeLists.txt|conanfile.txt|*.cmake|*.sh|.clang*|.gitignore|scripts/*|config/*)
                append_to_group "CHORE_FILES" "$file"
                ;;
            *format*|*lint*|.clang-format)
                append_to_group "STYLE_FILES" "$file"
                ;;
            *test*.cpp|*test*.hpp|tests/*)
                append_to_group "ADD_FILES" "$file"
                ;;
            *.cpp|*.hpp|*.c|*.h|client/*|server/*|common/*)
                # Détecter si c'est un fix ou une feature
                if git diff "$file" 2>/dev/null | grep -qE "fix|bug|error|crash"; then
                    append_to_group "FIX_FILES" "$file"
                else
                    append_to_group "ADD_FILES" "$file"
                fi
                ;;
            *)
                append_to_group "ADD_FILES" "$file"
                ;;
        esac
    done <<< "$MODIFIED_FILES"
fi

# Afficher les groupes détectés
echo "📦 Groupes détectés:"
if [ -n "$DOCS_FILES" ]; then echo "  [DOCS] : $(echo "$DOCS_FILES" | grep -c .) fichier(s)"; fi
if [ -n "$CHORE_FILES" ]; then echo "  [CHORE] : $(echo "$CHORE_FILES" | grep -c .) fichier(s)"; fi
if [ -n "$STYLE_FILES" ]; then echo "  [STYLE] : $(echo "$STYLE_FILES" | grep -c .) fichier(s)"; fi
if [ -n "$FIX_FILES" ]; then echo "  [FIX] : $(echo "$FIX_FILES" | grep -c .) fichier(s)"; fi
if [ -n "$ADD_FILES" ]; then echo "  [ADD] : $(echo "$ADD_FILES" | grep -c .) fichier(s)"; fi
if [ -n "$DEL_FILES" ]; then echo "  [DEL] : $(echo "$DEL_FILES" | grep -c .) fichier(s)"; fi
if [ -n "$REFACTOR_FILES" ]; then echo "  [REFACTOR] : $(echo "$REFACTOR_FILES" | grep -c .) fichier(s)"; fi
echo ""

# Demander confirmation
read -p "🚀 Créer des commits groupés ? (O/n): " confirm
if [ "$confirm" = "n" ] || [ "$confirm" = "N" ]; then
    echo "❌ Annulé"
    exit 0
fi

echo ""

# Créer les commits par groupe
COMMIT_COUNT=0

for type in DOCS CHORE STYLE FIX ADD DEL REFACTOR; do
    # Récupérer les fichiers du groupe selon le type
    case "$type" in
        DOCS) files="$DOCS_FILES" ;;
        CHORE) files="$CHORE_FILES" ;;
        STYLE) files="$STYLE_FILES" ;;
        FIX) files="$FIX_FILES" ;;
        ADD) files="$ADD_FILES" ;;
        DEL) files="$DEL_FILES" ;;
        REFACTOR) files="$REFACTOR_FILES" ;;
    esac
    
    if [ -z "$files" ]; then
        continue
    fi
    
    echo "📝 Groupe [$type]:"
    
    # Générer une description automatique basée sur les fichiers et leurs changements
    AUTO_DESC=""
    FILE_COUNT=0
    CHANGES_DESC=""
    
    # Compter les fichiers (en comptant les lignes non vides)
    while IFS= read -r line; do
        [ -z "$line" ] && continue
        FILE_COUNT=$((FILE_COUNT+1))
    done <<< "$files"
    
    # Afficher les fichiers avec leurs changements
    while IFS= read -r f; do
        [ -z "$f" ] && continue
        
        # Vérifier si c'est un fichier renommé (contient " -> ")
        if [[ "$f" == *" -> "* ]]; then
            # C'est un rename/move
            OLD_PATH=$(echo "$f" | awk '{print $1}')
            NEW_PATH=$(echo "$f" | awk '{print $3}')
            FILENAME=$(basename "$NEW_PATH")
            OLD_NAME=$(basename "$OLD_PATH")
            
            # Afficher le move
            if [ "$OLD_NAME" = "$FILENAME" ]; then
                echo "   - $f (déplacé)"
            else
                echo "   - $f (renommé)"
            fi
            
            # Générer description
            if [ "$OLD_NAME" = "$FILENAME" ]; then
                OLD_DIR=$(dirname "$OLD_PATH")
                NEW_DIR=$(dirname "$NEW_PATH")
                CHANGES_DESC="$CHANGES_DESC, $FILENAME: move from $OLD_DIR/ to $NEW_DIR/"
            else
                CHANGES_DESC="$CHANGES_DESC, $FILENAME: rename from $OLD_NAME"
            fi
            continue
        fi
        
        FILENAME=$(basename "$f")
        
        # Cas spécial pour les fichiers supprimés (DEL)
        if [ "$type" = "DEL" ]; then
            echo "   - $f (supprimé)"
            
            # Déterminer le but du fichier supprimé
            REASON=""
            case "$f" in
                build.sh|build.bat|format.sh|lint.sh|smart-commit.sh|install-hooks.sh)
                    REASON="(moved to scripts/)"
                    ;;
                *.deprecated.*|*.old|*.bak)
                    REASON="(obsolete)"
                    ;;
                *)
                    REASON=""
                    ;;
            esac
            
            if [ -n "$REASON" ]; then
                CHANGES_DESC="$CHANGES_DESC, $FILENAME: remove $REASON"
            else
                CHANGES_DESC="$CHANGES_DESC, $FILENAME: remove"
            fi
            continue
        fi
        
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
            # Fichier nouveau ou déplacé
            
            # Vérifier si c'est un renamed (contient "->")
            if [[ "$f" == *" -> "* ]]; then
                OLD_PATH=$(echo "$f" | awk '{print $1}')
                NEW_PATH=$(echo "$f" | awk '{print $3}')
                OLD_NAME=$(basename "$OLD_PATH")
                NEW_NAME=$(basename "$NEW_PATH")
                OLD_DIR=$(dirname "$OLD_PATH")
                NEW_DIR=$(dirname "$NEW_PATH")
                
                if [ "$OLD_NAME" = "$NEW_NAME" ]; then
                    # Même nom, juste déplacé
                    echo "   - $f (déplacé)"
                    CHANGES_DESC="$CHANGES_DESC, move $OLD_NAME from $OLD_DIR/ to $NEW_DIR/"
                else
                    # Renommé
                    echo "   - $f (renommé)"
                    CHANGES_DESC="$CHANGES_DESC, rename $OLD_NAME to $NEW_NAME"
                fi
            else
                # Vraiment nouveau fichier
                FILENAME=$(basename "$f")
                echo "   - $f (nouveau)"
                
                # Déterminer le but du nouveau fichier
                PURPOSE=""
                case "$f" in
                    tests/*.cpp|tests/*.hpp)
                        PURPOSE="for unit testing"
                        ;;
                    config/*.json)
                        PURPOSE="for runtime configuration"
                        ;;
                    scripts/*)
                        PURPOSE="for build automation"
                        ;;
                    assets/*/README.md)
                        PURPOSE="for assets organization"
                        ;;
                    assets/sprites/*)
                        PURPOSE="for game sprites"
                        ;;
                    assets/sounds/*)
                        PURPOSE="for sound effects"
                        ;;
                    assets/musics/*)
                        PURPOSE="for background music"
                        ;;
                    assets/fonts/*)
                        PURPOSE="for UI fonts"
                        ;;
                    docs/*.md)
                        PURPOSE="for project documentation"
                        ;;
                    common/include/*)
                        PURPOSE="for shared ECS components"
                        ;;
                    client/*)
                        PURPOSE="for client-side logic"
                        ;;
                    server/*)
                        PURPOSE="for server-side logic"
                        ;;
                    *)
                        PURPOSE=""
                        ;;
                esac
                
                if [ -n "$PURPOSE" ]; then
                    CHANGES_DESC="$CHANGES_DESC, add $FILENAME $PURPOSE"
                else
                    CHANGES_DESC="$CHANGES_DESC, add $FILENAME"
                fi
            fi
        fi
    done <<< "$files"
    
    # Nettoyer CHANGES_DESC (retirer la virgule initiale et les espaces)
    CHANGES_DESC=$(echo "$CHANGES_DESC" | sed 's/^[, ]*//')
    
    # Suggérer une description automatique intelligente
    if [ $FILE_COUNT -eq 1 ]; then
        AUTO_DESC="$CHANGES_DESC"
    else
        # Formater avec bullet points pour multi-fichiers
        AUTO_DESC=""
        while IFS= read -r f; do
            [ -z "$f" ] && continue
            
            # Pour les renames, extraire le nouveau nom
            if [[ "$f" == *" -> "* ]]; then
                FILENAME=$(echo "$f" | awk '{print $3}' | xargs basename)
            else
                FILENAME=$(basename "$f")
            fi
            
            # Extraire le résumé du changement pour ce fichier
            FILE_CHANGE=$(echo "$CHANGES_DESC" | grep -o "$FILENAME: [^,]*" | sed "s/$FILENAME: //")
            if [ -z "$AUTO_DESC" ]; then
                AUTO_DESC="- $FILENAME: $FILE_CHANGE"
            else
                AUTO_DESC="$AUTO_DESC\n- $FILENAME: $FILE_CHANGE"
            fi
        done <<< "$files"
    fi
    
    echo ""
    echo -e "   💡 Suggestion:\n$AUTO_DESC" | sed 's/^/   /'
    
    read -p "   Description (Entrée pour accepter, ou écris la tienne): " description
    
    # Utiliser la suggestion si vide
    if [ -z "$description" ]; then
        description="$AUTO_DESC"
    fi
    
    # Git add les fichiers du groupe (ou git rm pour les suppressions)
    if [ "$type" = "DEL" ]; then
        # Pour les suppressions, utiliser git rm
        while IFS= read -r f; do
            [ -z "$f" ] && continue
            git rm "$f" 2>/dev/null || git add "$f"
        done <<< "$files"
    else
        # Pour les autres, git add normal (mais fichiers déjà stagés par git add -A au début)
        # On pourrait skip, mais pour être sûr...
        while IFS= read -r f; do
            [ -z "$f" ] && continue
            # Si c'est un rename, extraire les chemins
            if [[ "$f" == *" -> "* ]]; then
                old_path=$(echo "$f" | awk '{print $1}')
                new_path=$(echo "$f" | awk '{print $3}')
                # Les renames sont déjà stagés par git add -A
                :
            else
                git add "$f" 2>/dev/null || true
            fi
        done <<< "$files"
    fi
    
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
read -p "🚀 Push maintenant ? (O/n): " push_confirm

# Entrée vide = oui par défaut
if [ -z "$push_confirm" ] || [ "$push_confirm" = "o" ] || [ "$push_confirm" = "O" ]; then
    BRANCH=$(git branch --show-current)
    echo "📤 Push vers origin/$BRANCH..."
    git push origin "$BRANCH"
    echo "✅ Push terminé!"
else
    echo "ℹ️  Push annulé. Utilise 'git push' plus tard."
fi

echo ""
echo "🎉 Terminé!"
