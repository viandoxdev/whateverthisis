#!/usr/bin/env bash

# argument from Makefile
BIN="$1"
CC="$2"

tests="$BIN/test"

sec=""
secspec=""
secname=""
curtest=""

gtsd=$(date +%s%3N)

echo "" > "$tests/global"
# repeat $1 $2 times
function repeat() {
	if [[ "$2" = "0" ]]; then
		echo ""
	else
		printf "=%.0s" $(seq "$2") | tr '=' "$1"
	fi
}

function erase_pre() {
	echo -en "\033[1A\033[1G\e[?25h"
}
function erase_post() {
	echo -en "\033[0K\e[?25l"
}
function erase() {
	erase_pre
	erase_post
}

function draw_task() {
	name="$1" # task name
	progress="$2" # progress (out of total)
	total="$3" # total
	w=$(tput cols) # term width
	prgw=$((w - ${#name} - 3 - 3 - ${#total} - 1 - ${#total})) # progress bar width
	prgp=$(bc <<< "scale=8; v = $progress / $total * $prgw; scale=0; v/1") # progress bar filled width
	prgb=$((prgw - prgp)) # progress bar blank width

	prgf="=" # progress bar filled
	prgt=">" # progress bar end
	prgn="-" # progress bar blank

	bar=$(repeat "$prgf" "$prgp") # make bar: filled part
	barb="$prgt$(repeat "$prgn" "$((prgb))")" # make bar: blank part
	[[ "$prgb" = "0" ]] && barb="" || barb=$(cut -c "1-$prgb" <<< "$barb")
	pad=$(repeat " " "$((${#total} - ${#progress}))") # make bar: padding at the end
	echo -en "$name  [$bar$barb]  $pad" # draw most of task
	# draw number
	if [[ "$progress" = "$total" ]]; then
		echo -en "\033[32m$progress/$total\033[0m\n"
	else
		echo -en "\033[2m$progress\033[97m/$total\033[0m\n"
	fi
}

failed="0"
failed_total="0"
failed_sec=()
failed_tests=()

function run_test() {
	prg="$i"
	sec="$(basename "$i" | cut -d "." -f 1)"
	nam="$(basename "$i" | cut -d "." -f 2)"
	w=$(tput cols)
	pre="├─ $nam"
	echo -ne "$pre$(repeat " " $((w - ${#pre} - 5)))[...]\n"
	txt=""
	txtl="0"
	
	if ( exec >/dev/null 2>&1; $prg ); then
		txt="\033[94m[✓]\033[0m"
		txtl="3"
	else
		txt="\033[31m[x]\033[0m"
		txtl="3"
		failed=$((failed + 1))
		failed_total=$((failed_total + 1))
		failed_tests+=("$(basename "$i")")
	fi
	erase_pre
	echo -ne "$pre$(repeat " " $((w - ${#pre} - txtl)))$txt\n"
	erase_post
}

echo ""
echo -e "\033[31mTESTS - MST \033[0m\033[90m(My Shitty Tests)\033[0m"
echo ""
# stands for Parsing Timer Start Date
ptsd=$(date +%s%3N)

draw_task "[parsing]  " "0" "1"
if [[ -e "$tests/.tests.c" ]]; then
	while IFS= read -r line; do
		if [[ "$line" =~ ^#\[.* ]]; then
			# this is a new section
			t=$(echo "$line" | cut -d "[" -f 2 | cut -d "]" -f 1)
			if [[ $t =~ ^[^a-z]*$ ]]; then
				sec="$t"
				if [[ "$t" = "TEST" ]]; then
					secname="$(echo "$line" | cut -d "[" -f 3 | cut -d "]" -f 1)"
					echo "" > "$tests/$secname"
				fi
			else
				secspec="$t"
			fi
		else
			if [[ "$sec" = "GLOBAL" ]]; then
				echo "$line" >> "$tests/global"
			else
				if [[ "$secspec" = "general" ]]; then
					echo "$line" >> "$tests/$secname"
				else
					re="^[[:blank:]]*void[[:blank:]]+_*([a-zA-Z][a-zA-Z0-9_]*)[[:blank:]]*\([[:blank:]]*\)([[:blank:]]*\{[[:blank:]]*)$"
					if [[ "$line" =~ $re ]]; then
						curtest=${BASH_REMATCH[1]}
						line="int main()${BASH_REMATCH[2]}"
						echo "" > "$tests/$secname.$curtest"
					fi
					if [[ -n "$curtest" ]]; then
						echo "$line" >> "$tests/$secname.$curtest"
					fi
				fi
			fi
		fi
	done < "$tests/.tests.c"
fi
erase_pre
draw_task "[parsing]  " "1" "1"
erase_post
ptse=$(date +%s%3N)
rm -rf "${tests:?}/bin"
mkdir -p "$tests/bin"
num="0" # total compilation
_num="0" # current
for i in "$tests"/*.*; do num=$((num + 1)); done
draw_task "[compiling]" "0" "$num"
for i in "$tests"/*.*; do
	mv "$i" "$i.tmp"
	tname=$(basename "$i" | cut -d "." -f1)
	if [[ -f "$tests/global" ]]; then
		cat "$tests/global" >> "$i"
	fi
	if [[ -f "$tests/$tname" ]]; then
		cat "$tests/$tname" >> "$i"
	fi
	tr '\n' '\f' < "$i.tmp" | sed 's/\(.*\)\}/\1\treturn 0;\n}/m' | tr '\f' '\n' >> "$i"
	rm "$i.tmp"
	mv "$i" "__tmp_test.c"
	bash -c "$CC -c __tmp_test.c -o $BIN/__tmp_test.o"
	[[ -f "$BIN/main.o" ]] && mv "$BIN/main.o" "$BIN/main.o.hidden"
	bash -c "$CC $BIN/*.o -o $tests/bin/$(basename "$i")"
	[[ -f "$BIN/main.o.hidden" ]] && mv "$BIN/main.o.hidden" "$BIN/main.O"
	rm "$BIN/__tmp_test.o"
	mv "__tmp_test.c" "$i"
	_num=$((_num + 1))
	erase_pre
	draw_task "[compiling]" "$_num" "$num"
	erase_post
done
_num="0"
echo ""
echo ""
draw_task "[running]  " "0" "$num"
sec=""
tstcount="0"
function print_end() {
	res=""
	resl="0"
	w=$(tput cols)
	if [[ "$failed" = "0" ]]; then
		res="\033[1m\033[94m[PASSED]"
		resl="8"
	else
		res="\033[1m\033[31m[FAILED]"
		resl="8"
		failed_sec+=("$sec")
	fi
	amm="($((tstcount - failed))/$((tstcount)))"
	echo -e "└─ tested \033[1m$sec\033[0m $amm$(repeat " " $((w - 11 - ${#sec} - resl - ${#amm})))$res\033[0m"
}

tss=$(date +%s%3N)
ctse=$(date +%s%3N)
for i in "$tests/bin"/*; do
	erase_pre
	erase_pre
	_sec=$(basename "$i" | cut -d "." -f 1)
	if ! [[ "$_sec" = "$sec" ]]; then
		if [[ -n "$sec" ]]; then
			print_end
			erase_post
		fi
		tstcount=$(find "$tests/bin" -name "$_sec.*" -type f -printf '.' | wc -c)
		echo -ne "[testing] \033[1m$_sec \033[0m($tstcount)\n"
		failed="0"
		erase_post
	fi
	sec="$_sec"
	run_test "$i"
	erase_post
	if [[ "$_num" = "$((num - 1))" ]]; then
		print_end
	fi
	echo ""
	_num=$((_num + 1))
	draw_task "[running]  " "$_num" "$num"
	erase_post
done

tse=$(date +%0s%3N)
rdlt="$((tse - tss))"
gdlt="$((tse - gtsd))"
pdlt="$((ptse - ptsd))"
cdlt="$((ctse - ptse))"

function dlttosec() {
	r="$1"
	if [[ ${#r} -lt 4 ]]; then
		r="$(repeat "0" $((4 - ${#r})))$r"
	fi
	echo "$r" | sed 's/\([0-9]\{3\}\)$/.\1/'
}

sg="$(dlttosec "$gdlt")"
sp="$(dlttosec "$pdlt")"
sc="$(dlttosec "$cdlt")"
sr="$(dlttosec "$rdlt")"

echo ""
echo -e "\033[31mMST: Tests ran, results: \033[0m"
echo ""
echo -e "ran \033[1m$_num\033[0m tests in \033[1m$sg\033[0ms"
echo "(parsing: ${sp}s, compiling: ${sc}s, running: ${sr}s)"
echo -e "passed: \033[1m$((_num - failed_total))\033[0m/\033[1m$_num\033[0m (failed: \033[1m$failed_total\033[0m)"

if ! [[ "$failed_total" = "0"  ]]; then
	echo "failed tests: "
	for f in "${failed_tests[@]}"; do
		echo -e "\033[31m($(echo "$f" | cut -d "." -f 1)) $(echo "$f" | cut -d "." -f 2)\033[0m"
	done
	echo ""
	echo -en "\033[1m\033[31mTESTS FAILED\033[0m"
	echo ""
	exit 1
else
	echo ""
	echo -en "\033[1mTESTS PASSED\033[0m"
	echo ""
	exit 0
fi
stty sane
