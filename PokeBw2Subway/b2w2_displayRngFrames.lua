# Based on Kaphotics' work

require "bit"

local readInt32 = memory.readdwordunsigned

local isBlack2 = readInt32(0x02FFFE0C) == 0x4F455249;
local pidValAddress = isBlack2 and 0x021FFC18 or 0x021FFC58 -- PRNG Seed Location
local mtRngTableTopAddress = isBlack2 and 0x021FED28 or 0x021FED68 -- Mersenne Twister Table Top

-- Advance the PidVal by 1
local function getNextPidValLow(pidLow)
    -- SEED1 = (0x6C078965 * SEED1) + 0x00269EC3; from Kazo
    local a = 0x6C07 * (pidLow % 65536) + bit.rshift(pidLow, 16) * 0x8965
    local b = 0x8965 * (pidLow % 65536) + (a % 65536) * 65536 + 0x00269EC3
    local c = b % 4294967296
    return c
end

-- Returns the number of times <lastValue> must be advanced to have the value <currentValue>
local function getPidFrameAdvancedCount(lastValue, currentValue)
  for i = 0, 200 do
      -- we only check the lower 32bits.
      if lastValue == currentValue then
          return i
      end
      lastValue = getNextPidValLow(lastValue)
  end
  -- too many advances. must stop otherwise will do infinite loop
  return nil
end

local initialSeedLow = 0
local initialSeedHigh = 0
local lastSeedLow = 0
local lastMtv = 0
local lastMtp = 0
local mtTableUsedCount = 0
local totalPidFrame = 0
local isFirstLoop = true
local lastEmuFrameCount = -1

local function loop()
    local currentEmuFrameCount = emu.framecount()
    if currentEmuFrameCount < lastEmuFrameCount then
        isFirstLoop = true  --emu was reset
    end
    lastEmuFrameCount = currentEmuFrameCount

    local ingamePidValLow = readInt32(pidValAddress)
    local ingamePidValHigh = readInt32(pidValAddress + 4)
    if ingamePidValLow == 0 and ingamePidValHigh == 0 then
        return -- game isn't loaded yet
    end

    local ingameMtv = readInt32(mtRngTableTopAddress)
    local ingameMtp = readInt32(mtRngTableTopAddress + 0x9C0)
    
    if isFirstLoop then
        isFirstLoop = false
        totalPidFrame = 0
        mtTableUsedCount = 0
        initialSeedLow = ingamePidValLow
        initialSeedHigh = ingamePidValHigh
        print("Game started.")
    else
        local pidFrameAdvancedThisLoop = getPidFrameAdvancedCount(lastSeedLow, ingamePidValLow)
        if pidFrameAdvancedThisLoop == nil then
            print("Error. Too many PIDRNG frames advanced in a single game frame. Most likely because the game state was altered externally.")
            pidFrameAdvancedThisLoop = 0
            isFirstLoop = true
        end

        -- Check to see if the MTRNG changed
        if (lastMtp > ingameMtp) or (lastMtp == ingameMtp and lastMtv ~= ingameMtv) then
            mtTableUsedCount = mtTableUsedCount + 1 -- mersenne twister created a new table
        end
    
        totalPidFrame = totalPidFrame + pidFrameAdvancedThisLoop
        local totalMtFrame = ingameMtp + (mtTableUsedCount - 1) * 624 - 1 -- Mersenne Twister Frame = Pointer Value + (TableRefresh-1)*624 ; this accounts for the initial value of 624 which is actually zero
        gui.text(1, 0, string.format("Initial Seed: %08X%08X", initialSeedHigh, initialSeedLow))
        gui.text(1, 10, string.format("PIDRNG Value: %08X%08X", ingamePidValHigh, ingamePidValLow))
        gui.text(1, 20, string.format("PIDRNG Frame: %d", totalPidFrame))
        gui.text(1, 30, string.format("IVRNG  Frame: %d", totalMtFrame))
    end

    lastSeedLow = ingamePidValLow
    lastMtp = ingameMtp
    lastMtv = ingameMtv
end

gui.register(loop)