#pragma once

#include <memory>
#include <vector>
#include <map>
#include <queue>
#include <random>
#include <functional>
#include <cstdarg>
#include <utility>

#include "libtcod.hpp"

using std::shared_ptr;
using std::vector;

class Entity;

#include "spell.hpp"
#include "spellCaster.hpp"
#include "mortal.hpp"
#include "combat.hpp"
#include "ai.hpp"
#include "loot.hpp"
#include "container.hpp"
#include "action.hpp"
#include "actionQueue.hpp"
#include "interaction.hpp"
#include "entity.hpp"

#include "map.hpp"
#include "gui.hpp"
#include "uiElement.hpp"
#include "viewport.hpp"
#include "inspectScreen.hpp"
#include "log.hpp"
#include "statScreen.hpp"
#include "ui.hpp"

#include "dataFile.hpp"

#include "engine.hpp"