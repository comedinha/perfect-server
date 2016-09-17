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

#include "protocolgame.h"
#include "protocolgamebase.h"

class ProtocolGame;
class ProtocolRecord;
typedef std::shared_ptr<ProtocolGame> ProtocolGame_ptr;
typedef std::shared_ptr<ProtocolRecord> ProtocolRecord_ptr;

class ProtocolRecord final : public ProtocolGameBase
{
	public:
		static const char* protocol_name() {
			return "record protocol";
		}

		ProtocolRecord(Connection_ptr connection);

		void login(const std::string& recordName, const std::string& id); // Para funcionar o cast no flash
	private:
		ProtocolRecord_ptr getThis() {
			return std::static_pointer_cast<ProtocolRecord>(shared_from_this());
		}
		ProtocolGame_ptr client;
		OperatingSystem_t operatingSystem;

		void logout();

		void disconnectRecord(const std::string& message) const;
		void writeToOutputBuffer(const NetworkMessage& msg, bool broadcast = true) final;

		void release() final;

		void parsePacket(NetworkMessage& msg) final;
		void onRecvFirstMessage(NetworkMessage& msg) final;

		void parseRecordSay(NetworkMessage& msg);
		bool parseCoomand(const std::string& text);
};