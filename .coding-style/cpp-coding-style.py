import argparse
import json
import os
import re
import subprocess

CODING_STYLE_DIR = os.path.dirname(os.path.realpath(__file__))
TRACK_FILE = ".formatted_files.json"

def get_formatted_file_tracking():
    """Load the set of already formatted files from tracking file."""
    PATH = os.path.join(CODING_STYLE_DIR, TRACK_FILE)
    if os.path.exists(PATH):
        with open(PATH) as f:
            formatted = set(json.load(f).get("files", []))
    else:
        formatted = set()
    return formatted


def save_formatted_file_tracking(formatted):
    """Save the set of formatted files to tracking file."""
    with open(TRACK_FILE, "w") as f:
        json.dump({"files": list(formatted)}, f, indent=2)


def get_modified_files():
    """Get list of modified C++ files compared to origin/main."""
    cmd_files = ["git", "diff", "origin/main", "--name-only", "--", "*.cpp", "*.h"]
    result = subprocess.run(cmd_files, capture_output=True, text=True)
    return [f.strip() for f in result.stdout.splitlines() if f.strip()]


def get_modified_lines(file_path):
    """Get line numbers that were modified in the given file."""
    cmd = ["git", "diff", "origin/main", "--unified=3", "--", file_path]
    result = subprocess.run(cmd, capture_output=True, text=True)
    lines = set()
    for line in result.stdout.splitlines():
        if line.startswith("@@"):
            plus_part = line.split("+")[1]
            start, count = plus_part.split()[0].split(",")
            start, count = int(start), int(count)
            for i in range(start, start + count):
                lines.add(i)
    return sorted(lines)


def should_format_line(line_number, lines_to_format):
    """Check if a specific line should be formatted."""
    return not lines_to_format or line_number in lines_to_format


TRAILING_RETURN_REGEX = re.compile(
    r'''^
        (?P<signature>.*?)
        (?<!operator)
        \s*->\s*
        (?P<rtype>.+)$
    ''',
    re.VERBOSE
)

def format_trailing_return_type(stripped, indent, column_limit):
    match = TRAILING_RETURN_REGEX.match(stripped)
    if not match:
        return None

    if len(stripped) <= column_limit:
        return None

    signature = match.group("signature").rstrip()
    rtype = match.group("rtype").strip()

    return [
        signature + "\n",
        " " * indent + "-> " + rtype + "\n"
    ]



def format_constructor_initializer(stripped, indent, column_limit):
    """Format constructor initializer list if line exceeds column limit.
    
    Returns:
        list of lines if formatting applied, None otherwise.
    """
    ctor_match = re.match(r'^\s*(\w+::\w+)\((.*)\)\s*:\s*(.*)$', stripped)
    if ctor_match and len(stripped) > column_limit:
        return [
            f"{ctor_match.group(1)}({ctor_match.group(2)})\n",
            ' ' * indent + f": {ctor_match.group(3)}\n"
        ]
    return None


def is_struct_start(stripped):
    """Check if line is the start of a struct definition."""
    return re.match(r'^\s*struct\s+\w+', stripped) is not None


def is_access_modifier(stripped):
    """Check if line is an access modifier (public/private/protected)."""
    return re.match(r'^\s*(public|private|protected)\s*:\s*$', stripped) is not None


def is_struct_end(stripped):
    """Check if line is the end of a struct definition."""
    return re.match(r'^\s*};', stripped) is not None


def fix_struct_indentation(line, indent, in_struct, struct_has_access_modifier):
    """Fix indentation for struct members without access modifiers.
    
    Returns:
        fixed line if adjustment needed, None otherwise.
    """
    if not in_struct or struct_has_access_modifier:
        return None
    if re.match(r'^\s{' + str(indent * 2) + r',}\S', line):
        return " " * indent + line.lstrip()

    return None


def process_file(filename, indent, column_limit, lines_to_format=None):
    """Apply custom formatting rules to a file."""
    with open(filename, "r") as f:
        lines = f.readlines()

    new_lines = []
    in_struct = False
    struct_line_index = None
    struct_has_access_modifier = False

    for i, line in enumerate(lines, start=1):
        stripped = line.strip()

        # Skip lines that shouldn't be formatted
        if not should_format_line(i, lines_to_format):
            new_lines.append(line)
            continue

        # Format trailing return type
        formatted = format_trailing_return_type(stripped, indent, column_limit)
        if formatted:
            new_lines.extend(formatted)
            continue

        # Format constructor initializer
        formatted = format_constructor_initializer(stripped, indent, column_limit)
        if formatted:
            new_lines.extend(formatted)
            continue

        # Handle struct start
        if is_struct_start(stripped):
            in_struct = True
            struct_line_index = len(new_lines)  # index of the future struct line
            struct_has_access_modifier = False
            new_lines.append(line)
            continue

        if in_struct:
            if is_access_modifier(stripped):
                struct_has_access_modifier = True
                new_lines.append(line)
                continue

            if is_struct_end(stripped):
                if struct_has_access_modifier:
                    new_lines[struct_line_index] = new_lines[struct_line_index].replace("struct", "class")
                
                in_struct = False
                new_lines.append(line)
                continue

            if not struct_has_access_modifier:
                fixed_line = fix_struct_indentation(line, indent, in_struct, False)
                if fixed_line:
                    new_lines.append(fixed_line)
                    continue

        new_lines.append(line)

    with open(filename, "w") as f:
        f.writelines(new_lines)


def format_files(indent, column_limit):
    """Format all modified files that haven't been formatted yet."""
    formatted = get_formatted_file_tracking()
    files_to_process = get_modified_files()
    
    formatted_count = 0
    for file_path in files_to_process:
        if not os.path.exists(file_path):
            continue
            
        if file_path in formatted:
            print(f"{file_path} has already been formatted.")
            continue
            
        lines_to_format = get_modified_lines(file_path)
        process_file(file_path, indent, column_limit, lines_to_format)
        formatted.add(file_path)
        formatted_count += 1
    
    save_formatted_file_tracking(formatted)
    return formatted_count


def positive_int(value):
    """Validate that argument is a positive integer."""
    try:
        ivalue = int(value)
        if ivalue <= 0:
            raise argparse.ArgumentTypeError(f"{value} is not a positive integer")
        return ivalue
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not a valid integer")


def parse_arguments():
    """Parse command-line arguments for formatting parameters."""
    parser = argparse.ArgumentParser(description='Format C++ files according to coding style.')
    parser.add_argument('-c', '--column-limit', type=positive_int, default=80,
                        help='Maximum column limit (default: 80)')
    parser.add_argument('-i', '--indent-width', type=positive_int, default=4,
                        help='Indentation width (default: 4)')
    return parser.parse_args()


def main():
    """Main entry point for the formatting script."""
    args = parse_arguments()
    indent = args.indent_width
    column_limit = args.column_limit

    formatted_count = format_files(indent, column_limit)
    
    print(f"Files have been properly formatted, {formatted_count} have been formatted!")


if __name__ == "__main__":
    main()
