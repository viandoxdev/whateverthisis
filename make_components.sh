#!/usr/bin/env bash

BIN="$1"

function default_hash() {
	$BIN/hash "$1"
}

function parse_comp() {
	file="$1"
	output="$2"
	comp="0"
	name=""
	tmp="$(mktemp)"
	while IFS= read -r l; do
		if [[ "$comp" = "1" ]]; then
			comp="0"
			re="([[:blank:]]*struct[[:blank:]]+)([[:alpha:]][[:alnum:]]*)([[:blank:]]*\{)"
			if [[ "$l" =~ $re ]]; then
				name="${BASH_REMATCH[2]}"
				l="${BASH_REMATCH[1]}${name}Component${BASH_REMATCH[3]}\n"
				l+="\tComponentClass cclass;"
			fi
		fi
		if [[ "$l" =~ "#include" ]]; then
			l="$(sed 's/\.\.\///' <<< "$l")"
		fi
		re="^[[:blank:]]*//@Component[[:blank:]]*"
		if [[ "$l" =~ $re ]]; then
			comp="1"
		else
			echo -e "$l" >> "$tmp"
		fi
	done < "$file"
	lname="$(echo "$name" | tr '[:upper:]' '[:lower:]')"
	echo -e "typedef struct ${name}Component ${name}Component;" >> "$tmp"
	echo -e "inline ${name}Component* component_${lname}_get_from_list(ComponentList* list) {" >> "$tmp"
	echo -e "\treturn (${name}Component*)component_get_from_list(list, \"${lname}\");" >> "$tmp"
	echo -e "}" >> "$tmp"
	echo -e "ComponentClass const component_${lname}_class = $(default_hash "$lname");" >> "$tmp"
	echo -e "inline ${name}Component component_${lname}_new() {" >> "$tmp"
	echo -e "\t ${name}Component res = {};" >> "$tmp"
	echo -e "\tres.cclass = component_${lname}_class;" >> "$tmp"
	echo -e "\treturn res;" >> "$tmp"
	echo -e "}" >> "$tmp"
	echo 
	cat "$tmp" >> "$output"
	rm "$tmp"
}
echo "#ifndef COMPONENTS_H" > "./components.h"
echo "#define COMPONENTS_H" >> "./components.h"
echo "#include \"ecs.h\"" >> "./components.h"

for f in ./components/*.h; do
	parse_comp "$f" "./components.h"
done
echo "#endif" >> "./components.h"
