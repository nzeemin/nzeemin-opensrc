; Sea Dragon for GameBoy
; Sprites
;

        PUSHO
; Define . and X to be 0 and 1
        OPT     b.X

SpritesCount	EQU	9

Sprites:
SpriteEmpty:
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%........
SpriteWater:
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%........
	DB	%..X...X.
	DB	%XX.XXX.X
SpriteSolid:
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
SpriteMine:
	DB	%X..X..X.
	DB	%.XXXXX..
	DB	%.XXXXX..
	DB	%XXXXXXX.
	DB	%.XXXXX..
	DB	%.XXXXX..
	DB	%X..X..X.
	DB	%........
SpriteChain:
	DB	%...X....
	DB	%........
	DB	%...X....
	DB	%........
	DB	%...X....
	DB	%........
	DB	%...X....
	DB	%........
SpriteAir:
	DB	%........
	DB	%........
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%........
SpriteBoat1:
	DB	%........
	DB	%........
	DB	%........
	DB	%XX..XXXX
	DB	%XXXXXXXX
	DB	%X.XXXXXX
	DB	%......XX
	DB	%........
SpriteBoat2:
	DB	%........
	DB	%....X...
	DB	%.XXXX...
	DB	%XXXXXXX.
	DB	%XXXXXXXX
	DB	%XXXXXXXX
	DB	%XXXXXXX.
	DB	%........
SpriteGun:
	DB	%..XXXX..
	DB	%.X...XX.
	DB	%XXXXXXXX
	DB	%.XXX..X.
	DB	%..XXXX..
	DB	%.....X..
	DB	%......X.
	DB	%.......X


        POPO
