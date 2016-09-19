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

#include "otpch.h"

#include "protocolgame.h"
#include "protocolrecord.h"

#include "outputmessage.h"

#include "tile.h"
#include "player.h"
#include "chat.h"

#include "configmanager.h"
#include "iologindata.h"

#include "game.h"

#include "connection.h"
#include "scheduler.h"
#include "ban.h"

extern Game g_game;
extern ConfigManager g_config;
extern Chat* g_chat;

ProtocolRecord::ProtocolRecord(Connection_ptr connection) :
	ProtocolGameBase(connection),
	client(nullptr)
{

}

void ProtocolRecord::disconnectRecord(const std::string& message) const
{
	auto output = OutputMessagePool::getOutputMessage();
	output->addByte(0x14);
	output->addString(message);
	send(std::move(output));
	disconnect();
}

void ProtocolRecord::onRecvFirstMessage(NetworkMessage& msg)
{
	if (g_game.getGameState() == GAME_STATE_SHUTDOWN) {
		disconnect();
		return;
	}

	operatingSystem = (OperatingSystem_t)msg.get<uint16_t>();
	version = msg.get<uint16_t>();

	msg.skipBytes(7); // U32 clientVersion, U8 clientType

	if (!RSA_decrypt(msg)) {
		disconnect();
		return;
	}

	uint32_t key[4];
	key[0] = msg.get<uint32_t>();
	key[1] = msg.get<uint32_t>();
	key[2] = msg.get<uint32_t>();
	key[3] = msg.get<uint32_t>();
	enableXTEAEncryption();
	setXTEAKey(key);

	if (operatingSystem >= CLIENTOS_OTCLIENT_LINUX) {
		NetworkMessage opcodeMessage;
		opcodeMessage.addByte(0x32);
		opcodeMessage.addByte(0x00);
		opcodeMessage.add<uint16_t>(0x00);
		writeToOutputBuffer(opcodeMessage);
	}

	msg.skipBytes(1); // gamemaster flag
	msg.getString();
	std::string recordInformation = msg.getString();
	size_t pos = recordInformation.find(':');

	std::string id = recordInformation.substr(0, pos);
	std::string characterName = recordInformation.substr(pos + 1);
	trimString(characterName);

	uint32_t timeStamp = msg.get<uint32_t>();
	uint8_t randNumber = msg.getByte();
	if (challengeTimestamp != timeStamp || challengeRandom != randNumber) {
		disconnect();
		return;
	}

	if (version < CLIENT_VERSION_MIN || version > CLIENT_VERSION_MAX) {
		disconnectRecord("Only clients with protocol " CLIENT_VERSION_STR " allowed!");
		return;
	}

	if (g_game.getGameState() == GAME_STATE_STARTUP) {
		disconnectRecord("Gameworld is starting up. Please wait.");
		return;
	}

	if (g_game.getGameState() == GAME_STATE_MAINTAIN) {
		disconnectRecord("Gameworld is under maintenance. Please re-connect in a while.");
		return;
	}

	BanInfo banInfo;
	if (IOBan::isIpBanned(getIP(), banInfo)) {
		if (banInfo.reason.empty()) {
			banInfo.reason = "(none)";
		}

		std::ostringstream ss;
		ss << "Your IP has been banned until " << formatDateShort(banInfo.expiresAt) << " by " << banInfo.bannedBy << ".\n\nReason specified:\n" << banInfo.reason;
		disconnectRecord(ss.str());
		return;
	}
	g_dispatcher.addTask(createTask(std::bind(&ProtocolRecord::login, std::static_pointer_cast<ProtocolRecord>(shared_from_this()), characterName, id)));
}

void ProtocolRecord::login(const std::string& recordName, const std::string& id)
{
	auto _player = g_game.getPlayerByName(recordName);
	if (!_player || _player->isRemoved()) {
		disconnectRecord("Beta system.");
		return;
	}

	const auto liveCasterProtocol = ProtocolGame::getLiveCast(_player);
	if (!liveCasterProtocol) {
		disconnectRecord("Live cast no longer exists. Please relogin to refresh the list.");
		return;
	}

	player = _player;
	player->incrementReferenceCounter();
	eventConnect = 0;
	client = liveCasterProtocol;
	acceptPackets = true;
	OutputMessagePool::getInstance().addProtocolToAutosend(shared_from_this());
}

void ProtocolRecord::logout()
{
	acceptPackets = false;
	disconnect();
}

void ProtocolRecord::parsePacket(NetworkMessage& msg)
{
	if (!acceptPackets || g_game.getGameState() == GAME_STATE_SHUTDOWN || msg.getLength() <= 0) {
		return;
	}

	uint8_t recvbyte = msg.getByte();

	if (!player) {
		if (recvbyte == 0x0F) {
			disconnect();
		}

		return;
	}

	//a dead player can not perform actions
	if (player->isRemoved() || player->getHealth() <= 0) {
		disconnect();
		return;
	}

	switch (recvbyte) {
		case 0x14: g_dispatcher.addTask(createTask(std::bind(&ProtocolRecord::logout, getThis()))); break;
		case 0x1D: g_dispatcher.addTask(createTask(std::bind(&ProtocolRecord::sendPingBack, getThis()))); break;
		case 0x1E: g_dispatcher.addTask(createTask(std::bind(&ProtocolRecord::sendPing, getThis()))); break;
		//Reset viewed position/direction if the spectator tries to move in any way
		case 0x64: case 0x65: case 0x66: case 0x67: case 0x68: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: case 0x70: case 0x71:
		case 0x72: g_dispatcher.addTask(createTask(std::bind(&ProtocolRecord::sendCancelWalk, getThis()))); break;
		case 0x96: parseRecordSay(msg); break;
		default:
			break;
	}

	if (msg.isOverrun()) {
		disconnect();
	}
}

void ProtocolRecord::parseRecordSay(NetworkMessage& msg)
{
	const std::string text = msg.getString();
	if (text.length() > 255) {
		return;
	}

	if (parseCoomand(text)) {
		return;
	}
}

void ProtocolRecord::release()
{
	//dispatcher
	Protocol::release();
	OutputMessagePool::getInstance().removeProtocolFromAutosend(shared_from_this());
}

void ProtocolRecord::writeToOutputBuffer(const NetworkMessage& msg, bool broadcast)
{
	OutputMessage_ptr out = getOutputBuffer(msg.getLength());
	out->append(msg);
}

bool ProtocolRecord::parseCoomand(const std::string& text)
{
	if (text[0] == '!') {

		StringVec t = explodeString(text.substr(1, text.length()), " ", 1);
		if (t.size() > 0) {
			toLowerCaseString(t[0]);

			std::string command = t[0];

			if (command == "help") {
				sendTextMessage(TextMessage(MESSAGE_CHANNEL, "Avalible commands: !help."));
			}
		}

		return true;
	}

	return false;
}