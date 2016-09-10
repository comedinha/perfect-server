/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2016  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_ACCOUNT_H_34817537BA2B4CB7B71AA562AFBB118F
#define FS_ACCOUNT_H_34817537BA2B4CB7B71AA562AFBB118F

#include "enums.h"

struct Account {
	std::vector<std::string> characters;
	std::vector<uint16_t> world;
	std::string name;
	std::string key;
	time_t lastDay;
	uint32_t id;
	uint16_t premiumDays;
	AccountType_t accountType;

	Account() : lastDay(0), id(0), premiumDays(0), accountType(ACCOUNT_TYPE_NORMAL) {}
};

struct World {
	std::vector<uint16_t> id;
	std::vector<std::string> name;
	std::vector<std::string> ip;
	std::vector<uint16_t> port;
	std::vector<uint16_t> previewer;

	World() : id(0), ip(0), port(0), previewer(0) {}
};

struct Casts {
	std::vector<uint16_t> id;
	std::vector<std::string> name;
	std::vector<uint16_t> password;
	std::vector<std::string> description;
	std::vector<uint16_t> spectators;
	std::vector<uint16_t> worldid;

	Casts() : id(0), password(0), spectators(0), worldid(0) {}
};

struct Playerson {
	std::vector<uint32_t> id;
	std::vector<uint16_t> world;

	Playerson() : id(0), world(0) {}
};

#endif
