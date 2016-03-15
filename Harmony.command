#!/bin/bash
cd ~/Documents/Harmony
printf "\033c"
while true; do
	PS3='What do you want to do? '
	options=("Edit" "Compile" "Run" "Dump" "Self-Edit" "Quit")
	select opt in "${options[@]}"
	do
		case $opt in
			"Edit")
				PS3='What project do you want to edit? '
				cd src
				options2=( * "New" )
				select opt2 in "${options2[@]}"
				do
					case $opt2 in
						"New")
							echo "What name? "
							read pname
							mkdir "$pname"
							cd "$pname"
							break
							;;
						*)
							cd "$opt2"
							break
							;;
					esac
				done

				PS3='What file do you want to edit? '
				options3=( * "New" )
				select opt3 in "${options3[@]}"
				do
					case $opt3 in
						"New")
							echo "What filename? "
							read fname
							nano "$fname"
							cd ../..
							break 2
							;;
						*)
							nano "$opt3"
							cd ../..
							break 2
							;;
					esac
				done
				;;
			"Compile")
				cd src
				link=""
				menu=""
				libgcc="$(i686-elf-gcc $CFLAGS -print-libgcc-file-name)"
				for dir in *; do
					echo "Compiling $dir..."
					cd $dir
					mkdir "../../bin/$dir"
					for file in *; do
						echo "Compiling $file..."
						ext="${file##*.}"
						if [ $ext = "s" ]; then
							o=$(basename "$file" .s)
							o+=".o"
							nasm -felf32 "$file" -o "../../bin/$dir/$o"
							echo "Compiled as $o!"
						elif [ $ext = "c" ]; then
							o=$(basename "$file" .c)
							o+=".o"
							i686-elf-gcc -c "$file" -o "../../bin/$dir/$o" -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -Wall -Wextra -Werror -Wno-overflow -fverbose-asm -g -O2 -lgcc
							echo "Compiled as $o!"
						elif [ $ext = "ld" ]; then
							echo "LD file found, no need to compile."
							link="$dir/$file"
						elif [ $ext = "lst" ]; then
							echo "LST file found, no need to compile."
							menu="$dir/$file"
						elif [ $ext = "h" ]; then
							echo "H file found, no need to compile."
						elif [ $ext = "cfg" ]; then
							echo "CFG file found, no need to compile."
						else
							echo "File $file has unknown extension!" >&2
						fi
					done
					cd "../../bin/$dir"
					files=( * )
					echo "To compile: ${files[@]}"
					read -r type<"../../src/$link"
					if [[ $type == *"ELF"* ]]; then
						if i686-elf-ld -T "../../src/$link" "${files[@]}" "$libgcc" -o "../${dir}.elf"; then
							echo "Compiled as ELF!"
						else
							echo "Error in compilation!" >&2
						fi
					elif [[ $type == *"BIN"* ]]; then
						if i686-elf-ld -T "../../src/$link" "${files[@]}" "$libgcc" -o "../${dir}.bin"; then
             						echo "Compiled as BIN!"
           						cp "../${dir}.bin" ../../isofiles/modules
						else
                    					echo "Error in compilation!" >&2
            					fi
					else
						echo "Format not found!" >&2
					fi
					cd ../../src
				done
				cd ..
				cp bin/Harmony.elf isofiles/boot
				cp "src/$menu" isofiles/boot/grub
				mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o harmony.iso isofiles
				read -rsp $'Press any key to continue...\n' -n1 key
				printf "\033c"
				break
				;;
			"Run")
				cp src/Harmony/bochs.cfg bochsrc.txt
				bochs -f bochsrc.txt -q
				read -rsp $'Press any key to continue...\n' -n1 key
				printf "\033c"
				break
				;;
			"Dump")
				Compiler/cross/i686-elf/bin/objdump -M intel -S -l -D isofiles/boot/harmony.elf
				read -rsp $'Press any key to continue...\n' -n1 key
                                printf "\033c"
				break
				;;
			"Self-Edit")
				nano "Harmony.command"
				./Harmony.command
				exit
				;;
			"Quit")
				break 2
				;;
			*) echo Invalid Option;;
		esac
	done
	printf "\033c"
done
