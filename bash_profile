alias gng1="cd ~/Documents/ngdevkit/neogeo && php tiled.php > tile_layers.c && make clean && make && cp rom/puzzledp.zip ../../mame/roms/ && cd ../../mame && ./mame -window -resolution 640x480 roms/puzzledp.zip && cd ~/Documents/ngdevkit/neogeo";
alias gng2="cd ~/Documents/ngdevkit/neogeo && php tiled.php > tile_layers.c && make && cp rom/puzzledp.zip ../../mame/roms/ && cd ../../mame && ./mame -window -resolution 640x480 roms/puzzledp.zip && cd ~/Documents/ngdevkit/neogeo";
alias mister="neosdconv -i rom -o mister.neo -n 'Ghouls version Thons' -g 'Platformer' -y 2024";
alias pull="git pull origin main"
alias push="git push origin main"

