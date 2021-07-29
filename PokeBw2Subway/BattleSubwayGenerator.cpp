#include "BattleSubwayGenerator.hpp"

#include <stdarg.h>

#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>

#include "BattleSubwayData.hpp"
#include "BattleSubwayState.hpp"
#include "Global.hpp"
#include "LCRNG64.hpp"
#include "SHA1.hpp"
#include "Utilities.hpp"
#include "json.hpp"

using json = nlohmann::json;

u32 frameAdvancedSum = 1;  // only works for searchOne

bool includeSaveFrameAdvance = true;

void BattleSubwayGenerator::log(const char* format, ...) const {
  if (!this->isTraceActive()) return;
  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  std::cout << buffer;
}

void BattleSubwayGenerator::advance(BWRNG& rng, u32 adv, const char* format,
                                    ...) const {
  if (!this->isTraceActive()) {
    rng.advance(adv);
    return;
  }
  if (adv == 0) std::cout << "\n---------------------------------------------";

  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  u32 advToPrint = adv;
  for (u32 i = 0; i < adv; i++) {
    rng.advance(1);
    std::cout << "\n" << frameAdvancedSum << " : ";
    frameAdvancedSum++;
    std::cout << "+" << advToPrint << " : "
              << "0x" << std::setfill('0') << std::setw(16) << std::hex
              << rng.getSeed() << std::resetiosflags(std::ios_base::basefield)
              << " : " << buffer << "";
    snprintf(buffer, sizeof(buffer), "...");
    advToPrint = 0;
  }
}

bool areCompatiblePokemons(u32 pokemon1, u32 pokemon2) {
  static const auto compatibleMatrix =
      []() -> std::array<std::array<bool, 987 + 1>, 987 + 1> {
    const auto& allTrainerPokemons = BattleSubwayData::getAllTrainersPokemons();
    std::array<std::array<bool, 987 + 1>, 987 + 1> matrix = {};

    for (const auto& tp1 : allTrainerPokemons) {
      for (const auto& tp2 : allTrainerPokemons) {
        matrix[tp1.id][tp2.id] = tp1.item != tp2.item && tp1.name != tp2.name;
      }
    }
    return matrix;
  }();
  return compatibleMatrix[pokemon1][pokemon2];
}

const std::vector<u32>& getPossiblePokemonsForTrainer(u32 trainerId) {
  static const auto pokemonsByTrainerMatrix =
      []() -> std::array<std::vector<u32>, 313 + 1> {
    std::array<std::vector<u32>, 313 + 1> matrix;
    std::ifstream read("./b2w2_trainers.json");
    if (!read.is_open()) return matrix;
    const json& trainers = json::parse(read, nullptr, false);
    for (const auto& trainer : trainers) {
      const u32 id = trainer["id"].get<u32>();
      matrix[id] = trainer["pokemons"].get<std::vector<u32>>();
    }
    return matrix;
  }();
  return pokemonsByTrainerMatrix[trainerId];
}

std::vector<u32> createList(u32 start, u32 end) {
  std::vector<u32> vector;
  vector.reserve(end - start + 1);
  for (u32 i = start; i <= end; ++i) vector.push_back(i);
  return vector;
}

const std::vector<u32>& getPossibleTrainersByWins(u32 wins, Subway type,
                                                  u32 trainerIdxInBattle) {
  // TODO: not correct list for superMulti, wifi, normalMulti

  if (type == SubwayType::normalSingle || type == SubwayType::normalDouble ||
      type == SubwayType::normalMulti || type == SubwayType::wifi) {
    static const auto& list0_49 = createList(0, 49);      // 1-50
    static const auto& list30_69 = createList(30, 69);    // 31-70
    static const auto& list50_109 = createList(50, 109);  // 51-110
    static const auto& list306_306 = createList(306, 306);
    static const auto& list308_308 = createList(308, 308);
    if (wins >= 0 && wins <= 5)  // 1-6
      return list0_49;
    if (wins >= 6 && wins <= 12)  // 7-13
      return list30_69;
    if (wins >= 13 && wins <= 19)  // 14-20
      return list50_109;
    if (wins == 20)  // 21
    {
      if (type == SubwayType::normalSingle) return list306_306;
      if (type == SubwayType::normalDouble) return list308_308;
      return list308_308;
    }
  }

  if (type == SubwayType::superSingle || type == SubwayType::superDouble) {
    static const auto& list110_159 = createList(110, 159);
    static const auto& list140_179 = createList(140, 179);
    static const auto& list160_199 = createList(160, 199);
    static const auto& list180_219 = createList(180, 219);
    static const auto& list200_239 = createList(200, 239);
    static const auto& list200_299 = createList(200, 299);
    static const auto& list307_307 = createList(307, 307);
    static const auto& list309_309 = createList(309, 309);
    static const auto& list312_312 = createList(312, 312);
    static const auto& list313_313 = createList(313, 313);

    if (wins >= 0 && wins <= 5) return list110_159;

    if (wins >= 6 && wins <= 12) return list140_179;
    if (wins >= 13 && wins <= 19) return list160_199;
    if (wins >= 20 && wins <= 26) return list180_219;
    if (wins == 27) return list200_239;
    if (wins >= 28 && wins <= 47) return list200_299;
    if (wins == 48) {
      if (type == SubwayType::superSingle) return list307_307;
      if (type == SubwayType::superDouble) return list309_309;
    }
    if (wins >= 49) return list200_299;
  }
  if (type == SubwayType::superMulti) {
    static const auto& list110_159 = createList(110, 159);
    static const auto& list140_179 = createList(140, 179);
    static const auto& list160_199 = createList(160, 199);
    static const auto& list180_219 = createList(180, 219);
    static const auto& list200_239 = createList(200, 239);
    static const auto& list220_259 = createList(220, 259);
    static const auto& list200_299 = createList(200, 299);
    static const auto& list307_307 = createList(307, 307);
    static const auto& list309_309 = createList(309, 309);
    static const auto& list312_312 = createList(312, 312);
    static const auto& list313_313 = createList(313, 313);

    if (wins >= 0 && wins <= 5) return list110_159;
    if (wins == 6 && trainerIdxInBattle == 1) return list160_199;
    if (wins >= 6 && wins <= 12) return list140_179;
    if (wins == 13 && trainerIdxInBattle == 0) return list160_199;
    if (wins == 13 && trainerIdxInBattle == 1) return list180_219;
    if (wins >= 14 && wins <= 19) return list160_199;
    if (wins >= 20 && wins <= 26) return list180_219;
    if (wins == 27) return list220_259;
    if (wins >= 28 && wins <= 47) return list200_299;
    if (wins == 48) return trainerIdxInBattle == 0 ? list312_312 : list313_313;
    if (wins >= 49) return list200_299;
  }

  if (type == SubwayType::battleInstituteSingle ||
      type == SubwayType::battleInstituteDouble) {
    static const auto& list50_69 = createList(50, 69);
    static const auto& list110_159 = createList(110, 159);
    static const auto& list160_179 = createList(160, 179);
    static const auto& list240_269 = createList(240, 269);
    static const auto& list270_299 = createList(270, 299);
    wins %= 7;
    if (wins == 0) return list50_69;
    if (wins == 1) return list110_159;
    if (wins == 2) return list160_179;
    if (wins == 3) return list240_269;
    if (wins == 4) return list270_299;
  }
  static std::vector<u32> Error;
  return Error;
}

/*
f0 = function(seed){ seed = BigInt(seed); return ((((seed >> 32n) >> 16n) +
(seed >> 32n)) >> 16n);}; f1 = function(seed, nb){ f0(seed) % BigInt(nb);}; f2 =
function(seed){ return arr[f1(seed, BigInt(arr.length))];}; arr =
*/

//\w+  f(0x$0n, 50n),

template <typename Container>
u32 getIdx(u64 seed, const Container& Vector) {
  u32 v1 = seed >> 32;
  u32 v2 = ((v1 >> 16) + v1) >> 16;
  u32 v3 = v2 % Vector.size();
  return v3;
}

template <typename Container>
const auto& getElement(u64 seed, const Container& Vector) {
  return Vector[getIdx(seed, Vector)];
}

template <typename Container>
const auto& getTrainerBattleInstitute(u64 seed, const Container& Vector) {
  u32 divider = static_cast<u32>(
      std::ceil(static_cast<double>(0xffffffff) / Vector.size()));
  u32 v1 = seed >> 32;
  return Vector[v1 / divider];
}

void BattleSubwayGenerator::addPokemonsToTrainer(
    BWRNG& rng, BattleSubwayTrainer& trainer, bool forTeammate,
    const BattleSubwayTrainer* prevTrainerForMulti) const {
  const auto& possiblePokemons =
      getPossiblePokemonsForTrainer(trainer.getTrainerId());
  auto& trainerPokemons = trainer.getPokemonsForInitialization();
  const u32 pokeCount = this->opts.subway.getPokemonCountByTrainer();
  for (u32 i = 0; i < pokeCount;) {
    this->advance(rng, 1, "selectPokemon #%d", i);
    u32 pokeIdToAdd = getElement(rng.getSeed(), possiblePokemons);
    this->log(" : Idx%d => %s", getIdx(rng.getSeed(), possiblePokemons),
              BattleSubwayData::pokeShortDesc(pokeIdToAdd));

    const bool isIncompatible = [&]() -> bool {
      const bool incompatibleWithOtherSelfPoke = std::any_of(
          trainerPokemons.data(), trainerPokemons.data() + i,
          [&](const auto& trainerPokemon) {
            return !areCompatiblePokemons(trainerPokemon.getId(), pokeIdToAdd);
          });
      if (incompatibleWithOtherSelfPoke) {
        this->log(" incompatibleWithOtherSelfPoke");
        return true;
      }
      if (forTeammate) {
        const auto& poke =
            BattleSubwayData::getAllTrainersPokemons()[pokeIdToAdd];
        const bool isIncompatibleWithPlayerPoke =
            poke.item == this->opts.playerTeam.item1 ||
            poke.item == this->opts.playerTeam.item2 ||
            poke.name == this->opts.playerTeam.pokemon1 ||
            poke.name == this->opts.playerTeam.pokemon2;
        if (isIncompatibleWithPlayerPoke) {
          this->log(" isIncompatibleWithPlayerPoke");
          return true;
        }
      }

      if (prevTrainerForMulti != nullptr) {
        bool isIncompatibleWithPrevTrainerForMulti = false;
        prevTrainerForMulti->forEachPokemon([&](const auto& prevTrainerPokemon,
                                                size_t) {
          if (!areCompatiblePokemons(prevTrainerPokemon.getId(), pokeIdToAdd))
            isIncompatibleWithPrevTrainerForMulti = true;
        });
        if (isIncompatibleWithPrevTrainerForMulti) {
          this->log(" isIncompatibleWithPrevTrainerForMulti");
          return true;
        }
      }
      return false;
    }();

    if (isIncompatible) continue;

    trainerPokemons[i].setId(pokeIdToAdd);
    i++;
  }
  this->advance(rng, 2, "unknown advances when selecting pokemon");

  for (u32 i = 0; i < pokeCount; i++) {
    this->advance(rng, 1, "selectPokemonAbilityAndGender");
    u32 ability = (rng.getSeed() >> 32) % 2;
    trainerPokemons[i].setAbility(ability);
  }
}

std::optional<BattleSubwayState> BattleSubwayGenerator::generate(
    u64 seed, u32 multiTeammateUnknownFrameAdvance) const {
  BWRNG rng(seed);
  this->advance(rng, 0, "save loaded");
  BattleSubwayState state(rng.getSeed());

  auto& trainers = state.getTrainersForInitialization();
  u32 trainerCountByBattle = this->opts.subway.getTrainerCountByBattle();
  u32 battleCount = this->opts.subway.getBattleCount();
  u32 trainerCount = trainerCountByBattle * battleCount;

  if (this->opts.subway.isSubway()) {
    if (includeSaveFrameAdvance) advance(rng, 1, "saving to start subway");

    // determine teammate
    if (this->opts.subway.isMulti()) {
      state.getMultiTeammate().setTrainerId(this->getPlayerTeam().multiTrainer);
      this->addPokemonsToTrainer(rng, state.getMultiTeammate(), true);
      this->advance(rng, multiTeammateUnknownFrameAdvance, "unknown");
      if (this->filter && !this->filter->doesMultiTeammateRespectFilter(
                              state.getMultiTeammate()))
        return std::nullopt;
    }
    this->advance(rng, 0, "selectTrainerStart");

    // determine all trainer ids
    for (u32 i = 0; i < battleCount; i++) {
      for (u32 j = 0; j < trainerCountByBattle;) {
        const u32 trainerIdx = i * trainerCountByBattle + j;
        const auto& possibleTrainers = getPossibleTrainersByWins(
            this->opts.wins + i, this->opts.subway, j);
        u32 adv = possibleTrainers.size() == 1 ? 0 : 1;
        this->advance(rng, adv, "selectTrainer #%d", trainerIdx);
        const auto& trainerId = getElement(rng.getSeed(), possibleTrainers);
        this->log(" : %s", BattleSubwayData::trainerDesc(trainerId, true));
        auto end = trainers.begin() + trainerIdx;
        auto it = std::find_if(trainers.begin(), end, [&](const auto& trainer) {
          return trainer.getTrainerId() == trainerId;
        });
        if (it != end) {
          this->log(" : isDupe");
          continue;  // We can't have the same trainer twice in the series.
        }
        this->log(" : good");
        trainers[trainerIdx].setTrainerId(trainerId);
        j++;
      }
    }
    this->advance(rng, 0, "selectTrainerEnd, selectPokemonStart");

    // determine the pokemons
    for (u32 i = 0; i < battleCount; i++) {
      for (u32 j = 0; j < trainerCountByBattle; j++) {
        u32 trainerIdx = i * trainerCountByBattle + j;
        auto& trainer = trainers[trainerIdx];
        const auto prevTrainerForMulti =
            this->opts.subway.isMulti() && trainerIdx % 2 == 1
                ? &trainers[trainerIdx - 1]
                : nullptr;
        this->addPokemonsToTrainer(rng, trainer, false, prevTrainerForMulti);
        if (this->filter != nullptr &&
            !this->filter->doesTrainerRespectFilter(trainer))
          return std::nullopt;
      }
      // after battle
      u32 pokeNpcCount =
          this->opts.subway.getPokemonCountByTrainer() * trainerCountByBattle;
      if (this->opts.subway.isMulti()) pokeNpcCount += 2;  // for teammate
      this->advance(rng, pokeNpcCount * 2,
                    "unknown advances when starting battle");

      this->advance(rng, 0, "betweenBattle");
    }
  } else {
    this->advance(rng, 3, "wandering npc");

    // determine trainer one by one
    for (u32 i = 0; i < trainerCount; i++) {
      auto& trainer = trainers[i];
      const auto& possibleTrainers =
          getPossibleTrainersByWins(this->opts.wins + i, this->opts.subway, 0);
      this->advance(rng, 1, "selectTrainer");
      const auto& trainerId =
          getTrainerBattleInstitute(rng.getSeed(), possibleTrainers);
      trainers[i].setTrainerId(trainerId);

      addPokemonsToTrainer(rng, trainer);
      if (this->filter != nullptr &&
          !this->filter->doesTrainerRespectFilter(trainer))
        return std::nullopt;
      this->advance(rng, this->opts.subway.getPokemonCountByTrainer() * 2,
                    "unknown advances when starting battle");
    }
  }
  if (this->isTraceActive()) std::cout << "\n";
  return state;
}
