# Copyright (c) 2015-2019 Damien Ciabrini
# This file is part of ngdevkit
#
# ngdevkit is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# ngdevkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.

all: cart nullbios

include ../Makefile.config

# ROM names and common targets
include ../Makefile.common
$(CART): $(PROM) $(CROM1) $(CROM2) $(SROM) $(VROM) $(MROM) | rom

OBJS=main
ELF=rom.elf

FIX_ASSETS=$(ASSETS)/rom/s1-shadow.bin



$(ASSETS)/rom/c1.bin $(ASSETS)/rom/s1.bin:
	$(MAKE) -C $(ASSETS)

# -------------------------------------
# Background
# -------------------------------------
sprites/back.gif: gfx/tiles_back.png | sprites
	$(CONVERT) $^ -crop 320x320+0+0 +repage $@

sprites/back.c1 sprites/back.c2: sprites/back.gif
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/back.pal: sprites/back.gif
	$(PALTOOL) $< -o $@

# -------------------------------------
# Herbe
# -------------------------------------
sprites/herbe.png: gfx/tiles_herbe.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 64x64+0+0 +repage -background black -flatten $@

sprites/herbe.c1 sprites/herbe.c2: sprites/herbe.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/herbe.pal: sprites/herbe.png
	$(PALTOOL) $< -o $@


# -------------------------------------
# Flottes
# -------------------------------------
sprites/flotte.png: gfx/tiles_flottes.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 160x16+0+0 +repage -background black -flatten $@

sprites/flotte.c1 sprites/flotte.c2: sprites/flotte.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/flotte.pal: sprites/flotte.png
	$(PALTOOL) $< -o $@

# -------------------------------------------- #
# --- Arthur1                                  #
# -------------------------------------------- #
sprites/arthur1.png: gfx/tiles_arthur1.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 320x128+0+0 +repage -background black -flatten $@

sprites/arthur1.c1 sprites/arthur1.c2: sprites/arthur1.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/arthur1.pal: sprites/arthur1.png
	$(PALTOOL) $< -o $@

# -------------------------------------------- #
# --- Arthur2                                  #
# -------------------------------------------- #
sprites/arthur2.png: gfx/tiles_arthur2.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 320x96+0+0 +repage -background black -flatten $@

sprites/arthur2.c1 sprites/arthur2.c2: sprites/arthur2.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/arthur2.pal: sprites/arthur2.png
	$(PALTOOL) $< -o $@

# -------------------------------------
# Nuage
# -------------------------------------
sprites/nuage.png: gfx/tiles_nuage.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 224x48+0+0 +repage -background black -flatten $@

sprites/nuage.c1 sprites/nuage.c2: sprites/nuage.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/nuage.pal: sprites/nuage.png
	$(PALTOOL) $< -o $@

# -------------------------------------
# Lance
# -------------------------------------
sprites/lance.png: gfx/tiles_lance.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 64x16+0+0 +repage -background black -flatten $@

sprites/lance.c1 sprites/lance.c2: sprites/lance.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/lance.pal: sprites/lance.png
	$(PALTOOL) $< -o $@

# -------------------------------------
# Map
# -------------------------------------
sprites/map.png: gfx/tiles_map.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 144x144+0+0 +repage -background black -flatten $@

sprites/map.c1 sprites/map.c2: sprites/map.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/map.pal: sprites/map.png
	$(PALTOOL) $< -o $@

# -------------------------------------
# Pont
# -------------------------------------
sprites/pont.png: gfx/tiles_pont.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 32x16+0+0 +repage -background black -flatten $@

sprites/pont.c1 sprites/pont.c2: sprites/pont.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/pont.pal: sprites/pont.png
	$(PALTOOL) $< -o $@

# -------------------------------------
# Tombes
# -------------------------------------
sprites/tombe.png: gfx/tiles_tombe.png | sprites
	$(CONVERT) $^ $^ $^ +append -crop 96x96+0+0 +repage -background black -flatten $@

sprites/tombe.c1 sprites/tombe.c2: sprites/tombe.png
	$(TILETOOL) --sprite -c $< -o $@ $(@:%.c1=%).c2

sprites/tombe.pal: sprites/tombe.png
	$(PALTOOL) $< -o $@


$(ELF):	$(OBJS:%=%.o)
	$(M68KGCC) -o $@ $^ `pkg-config --libs ngdevkit`

%.o: %.c
	$(M68KGCC) $(NGCFLAGS) -std=gnu99 -fomit-frame-pointer -g -c $< -o $@

main.c: \
	sprites/back.pal \
	sprites/herbe.pal \
	sprites/flotte.pal \
	sprites/arthur1.pal \
	sprites/arthur2.pal \
	sprites/nuage.pal \
	sprites/lance.pal \
	sprites/map.pal \
	sprites/pont.pal \
	sprites/tombe.pal \

# sound driver ROM: ngdevkit's nullsound
MROMSIZE:=131072
$(MROM): | rom
	$(Z80SDOBJCOPY) -I ihex -O binary $(NGDKSHAREDIR)/nullsound_driver.ihx $@ --pad-to $(MROMSIZE)

# sample ROM: empty
$(VROM): | rom
	dd if=/dev/zero bs=1024 count=512 of=$@

# sprite ROM C1 C2: parallax layers
CROMSIZE:=1048576
$(CROM1): $(ASSETS)/rom/c1.bin \
	sprites/back.c1 \
	sprites/herbe.c1 \
	sprites/flotte.c1 \
	sprites/arthur1.c1 \
	sprites/arthur2.c1 \
	sprites/nuage.c1 \
	sprites/lance.c1 \
	sprites/map.c1 \
	sprites/pont.c1 \
	sprites/tombe.c1 \
	| rom
	cat $(ASSETS)/rom/c1.bin $(filter %.c1,$^) > $@ && $(TRUNCATE) -s $(CROMSIZE) $@

$(CROM2): $(ASSETS)/rom/c2.bin \
	sprites/back.c2 \
	sprites/herbe.c2 \
	sprites/flotte.c2 \
	sprites/arthur1.c2 \
	sprites/arthur2.c2 \
	sprites/nuage.c2 \
	sprites/lance.c2 \
	sprites/map.c2 \
	sprites/pont.c2 \
	sprites/tombe.c2 \
	| rom
	cat $(ASSETS)/rom/c2.bin $(filter %.c2,$^) > $@ && $(TRUNCATE) -s $(CROMSIZE) $@

# fixed tile ROM: fonts from common assets
SROMSIZE:=131072
$(SROM): $(FIX_ASSETS) | rom
	cat $(FIX_ASSETS) > $@ && $(TRUNCATE) -s $(SROMSIZE) $@

# program ROM
PROMSIZE:=524288
$(PROM): $(ELF) | rom
	$(M68KOBJCOPY) -O binary -S -R .comment --gap-fill 0xff --pad-to $(PROMSIZE) $< $@ && dd if=$@ of=$@ conv=notrunc,swab

clean:
	rm -rf *.gif *.pal *.o *~ $(ELF) tmp.* rom sprites

sprites:
	mkdir $@

.PHONY: clean