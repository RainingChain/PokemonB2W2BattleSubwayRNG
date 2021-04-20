Pokemon Black 2/White 2 Battle Subway RNG Tool
==============================================
This tool calculates the trainers and pokemons that you will encounter in the Battle Subway in Pokemon Black 2 or White 2
based on your profile, the time you boot the DS and the number of current wins.
The tool uses the average of Timer0Min and Timer0Max to determine the initial seed.


Important note
--------------
After changing the DS clock, loading the game and changing map will cause daily events to reset
which will increase the PID RNG frame, causing the pokemons to be different than what the tool says.
To avoid this issue, change the DS clock, load the game, leave the subway entirely, save, come back inside
and save again next to the operator.

Basic options:
--------------
* `--profileFile`: Path to profile json file. Use other tool such as PokeFinder to find the right values. Check ./myProfile.json for an example. 
* `--dateTime`: Date when the game is started, in yyyy-m-d-h-m-s format. Default: 2009-1-1-0-0-0
* `--wins`: number of wins so far in the Battle Subway. Default: 28 wins

Input example: 
```
PokemonB2W2BattleSubwayRNG.exe --profileFile ./myProfile.json --dateTime 2021-4-18-23-54-9 --wins 28
````

Output example:
```
2021-04-18 23:54:09, Seed:0x802accf7048bb49f, InitialAdvances:57
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
For example, [408,0,1] means that encountering a Skarmory with ability Keen Eye has a +1 rating.
* `--filterMinRating`: The minimum rating score that a 21-pokemon series must have in order to be printed. Recommended value: 19.5

Credits
-------
* This tool and the Battle Subway research was made by RainingChain. (RainingChain#3489 on Discord)
* Special thanks to Admiral Fish for providing the generic pokemon RNG algorithms. Check his project [PokeFinder](https://github.com/Admiral-Fish/PokeFinder).


