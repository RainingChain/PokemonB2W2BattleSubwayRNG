Pokemon Black 2/White 2 Battle Subway RNG Tool
==============================================
This tool calculates the trainers and pokemons that you will encounter in the Battle Subway in Pokemon Black 2 or White 2
based on your profile, the time you boot the DS and the number of current wins.
The tool uses the average of Timer0Min and Timer0Max to determine the initial seed. On emulator, the Timer0Min and Timer0Max should be the same.
The tool assumes no buttons is held when booting the game.

Important note
--------------
After changing the DS clock, loading the game and changing map will cause daily events to reset
which will increase the PID RNG frame, causing the pokemons to be different than what the tool says.
To avoid this issue, change the DS clock, load the game, leave the subway entirely, save, come back inside
and save again next to the operator.

Generate options:
--------------
This will display the list of expected trainers based on the inputs:
* `--profile`: Path to profile json file. Use other tool such as PokeFinder to find the right values. Default: ./profile.json
* `--dateTime`: Date when the game is started, in yyyy-m-d-h-m-s format. Default: 2009-1-1-0-0-0
* `--wins`: number of wins so far in the Battle Subway. Must be a multiple of 7. For Battle Institute, must be 0. Default: 28 wins
* `--subwayType`: The type of subway. Possible values: normalSingle, superSingle, normalDouble, superDouble, battleInstituteSingle, battleInstituteDouble. Default: superSingle
* `--pidRng`: Advanced. The PIDRNG value (in hex format) before talking to the subway operator. This overwrites the PIDRNG value determined by --profile and --dateTime. Ex: 0xF234567812345678. No default.
* `--printRngFramesInfo`: Advanced.  Display RNG frame info for RNG searchers. Default: false

Input example: 
```
PokemonB2W2BattleSubwayRNG.exe --profile ./profile.json --dateTime 2021-4-18-23-54-9 --wins 0
PokemonB2W2BattleSubwayRNG.exe --profile ./profile.json --dateTime 2021-12-31-1-2-3 --wins 7 --subwayType superDouble
PokemonB2W2BattleSubwayRNG.exe --pidRng 0xF234567812345678 --wins 14 --subwayType superDouble
````

Output example:
```
Date: 2009-01-01-00-00-00
Seed: 0x802accf7048bb49f, InitialAdvances:57, PIDRNG: 0x9cb929d058fb6096

(t212,p857,a0|p750,a1|p858,a0|) (t274,p589,a0|p639,a0|p619,a1|) (t252,p569,a0|p368,a1|p860,a0|) (t223,p596,a0|p585,a1|p605,a1|) (t254,p712,a0|p826,a0|p831,a1|) (t270,p380,a0|p914,a0|p667,a1|) (t282,p340,a0|p292,a1|p240,a0|)

TrainerId: 212, Name:Psychic Sambala
  Id:857, Name:Gardevoir, Item:Psychic Gem, Ability:Synchronize, Moves:Psychic,Thunderbolt,Shadow Ball,Destiny Bond,
  Id:750, Name:Medicham, Item:Life Orb, Ability:Pure Power, Moves:Hi Jump Kick,Psycho Cut,Fake Out,Detect,
  Id:858, Name:Gallade, Item:Lum Berry, Ability:Steadfast, Moves:Psycho Cut,Close Combat,Heal Pulse,Ally Switch,

TrainerId: 274, Name:Cyclist Birgit
  Id:589, Name:Dewgong, Item:Quick Claw, Ability:Thick Fat, Moves:Fake Out,Surf,Ice Beam,Aqua Jet,
  Id:639, Name:Carracosta, Item:Damp Rock, Ability:Solid Rock, Moves:Rain Dance,Waterfall,Rock Slide,Earthquake,
  Id:619, Name:Musharna, Item:Scope Lens, Ability:Synchronize, Moves:Psychic,Shadow Ball,Energy Ball,Yawn,

TrainerId: 252, Name:Parasol Lady Vanna
  Id:569, Name:Empoleon, Item:Lum Berry, Ability:Torrent, Moves:Surf,Ice Beam,Signal Beam,Icy Wind,
  Id:368, Name:Milotic, Item:Enigma Berry, Ability:Marvel Scale, Moves:Surf,Recover,Mirror Coat,Confuse Ray,
  Id:860, Name:Starmie, Item:King's Rock, Ability:Illuminate, Moves:Surf,Psychic,Thunderbolt,Ice Beam,

TrainerId: 223, Name:Pokemon Ranger Terran
  Id:596, Name:Unfezant, Item:Power Herb, Ability:Big Pecks, Moves:Sky Attack,Return,Detect,U-turn,
  Id:585, Name:Whiscash, Item:Expert Belt, Ability:Anticipation, Moves:Earth Power,Surf,Blizzard,Protect,
  Id:605, Name:Lilligant, Item:Leftovers, Ability:Own Tempo, Moves:Quiver Dance,Energy Ball,Leech Seed,Protect,

TrainerId: 254, Name:Roughneck Ganymed
  Id:712, Name:Vanilluxe, Item:Leftovers, Ability:Ice Body, Moves:Hail,Blizzard,Flash Cannon,Sheer Cold,
  Id:826, Name:Pinsir, Item:Choice Scarf, Ability:Hyper Cutter, Moves:X-Scissor,Earthquake,Close Combat,Guillotine,
  Id:831, Name:Donphan, Item:Quick Claw, Ability:Sturdy, Moves:Earthquake,Stone Edge,Seed Bomb,Fissure,

TrainerId: 270, Name:Worker Hayes
  Id:380, Name:Garchomp, Item:Lax Incense, Ability:Sand Veil, Moves:Earthquake,Protect,Sandstorm,Sand Tomb,
  Id:914, Name:Regirock, Item:Leftovers, Ability:Clear Body, Moves:Sandstorm,Curse,Rock Slide,Drain Punch,
  Id:667, Name:Gigalith, Item:Custap Berry, Ability:Sturdy, Moves:Stone Edge,Earthquake,Sandstorm,Explosion,

TrainerId: 282, Name:Depot Agent Isadore
  Id:340, Name:Jolteon, Item:Magnet, Ability:Volt Absorb, Moves:Quick Attack,Thunderbolt,Thunder Wave,Swagger,
  Id:292, Name:Zebstrika, Item:Electric Gem, Ability:Motor Drive, Moves:Swagger,Volt Switch,Wild Charge,Pursuit,
  Id:240, Name:Manectric, Item:Life Orb, Ability:Static, Moves:Crunch,Odor Sleuth,Quick Attack,Wild Charge,
```

Search usage:
=============

The tool can also be used to find the boot time that will give you the most chance to win.
For example, it can determine that if using Volcarona, the best time to boot the game is at 2066-9-10-5-27-57, because
if booted at that time, 19 out of the 21 pokemons in the series is outsped by Volcarona and Volcarona is guaranteed to OHKO them.

Search options: 
---------------
* `--playerPokemonsFile`: Path to the file containing pokemon ratings. Check ./b2w2_playerPokemons.json for an example. 
\"ratings\" is an array of [PokemonBattleSubwayId,AbilityIdx,Rating]
To determine the PokemonBattleSubwayId of a pokemon, check ./b2w2_pokemons.json
For example, [408,0,1] means that encountering a Skarmory (id=408) with ability Keen Eye (AbilityIdx=0) has a +1 rating. Default: ./b2w2_playerPokemons.json
* `--playerPokemonsIdxFilter`:  List of indexes of player pokemons to consider from the file --playerPokemonsFile, separated by commas. For example,
the file of --playerPokemonsFile contains 20 pokemons, but you only want the tool to consider the pokemons at index 7 and 11.
In that case, you'd use --playerPokemonsIdxFilter 7,11.
If omitted, all pokemons of the file are considered. No default.
* `--filterMinRating`: The minimum rating score that the trainer series must have in order to be printed. Recommended value for superSingle: 19. Recommended value for superDouble: 25.

Options for normalMulti and superMulti: 
---------------
In normalMulti and superMulti, your own pokemon influences your teammate and thus, the trainers you will face.
This means you must provide information about your own pokemons.
*`--multiTrainer`: The battle orientation of your teammate. Either attack, defense or balanced. Default: attack.
*`--playerPokemonsFile`: See above. This option is considered even in \"generate\" mode.
*`--playerPokemonsIdxFilter`: See above. --playerPokemonsIdxFilter must contain 2 elements that represent your 2 pokemons.
*`--multiTeammateUnknownFrameAdvance`: Number of frame advances after selecting your teammate in multi battle.
The logic that determines the number of advance is unknown. There is about 25% chance to be 12 advances.
Thus, the tool calculates the correct list of trainers only 25% of the time.
Further reearch is needed to automatically determine that value. Default: 12 

Credits
-------
* This tool and the Battle Subway research was made by RainingChain. (RainingChain#3489 on Discord)
* Special thanks to Admiral Fish for providing the generic pokemon RNG algorithms. Check his project [PokeFinder](https://github.com/Admiral-Fish/PokeFinder).


