BugReportType = {
	BUG_CATEGORY_MAP = 0,
	BUG_CATEGORY_TYPO = 1,
	BUG_CATEGORY_TECHNICAL = 2,
	BUG_CATEGORY_OTHER = 3
}

function onRecvbyte(player, msg, byte)
	if player:getLevel() < 50 and player:getAccountType() < ACCOUNT_TYPE_TUTOR then
		player:sendTextMessage(MESSAGE_EVENT_DEFAULT, "Your level is too low. Try again with level 50.")
		return false
	end

	local storage = 2508162321
	if player:getStorageValue(storage) - os.time() >= 0 then
		if player:getAccountType() < ACCOUNT_TYPE_TUTOR then
			player:sendTextMessage(MESSAGE_INFO_DESCR, "Exausted, try in ".. player:getStorageValue(storage) - os.time() .." seconds!")
			return false
		end
	end
	player:setStorageValue(storage, os.time() + 180)

	local category = msg:getByte()
	local message = msg:getString()

	if category == BugReportType.BUG_CATEGORY_MAP then
		position = msg:getPosition()
	else
		position = player:getPosition()
	end

	message = message .. "\n\nPosition: " .. position.x .. ", " .. position.y .. ", " .. position.z .. ""
	
	db:query("INSERT INTO `z_helpdesk` (`ticket`, `date`, `account_id`, `player_id`, `subject`, `text`, `category`) VALUES ('BUGRPT', " .. os.time() .. ", " .. player:getAccountId() .. ", " .. player:getGuid() .. ", 'Bug report in game', " .. db.escapeString(message) .. ", 9)")
	player:sendTextMessage(MESSAGE_INFO_DESCR, "Your report has been sent to a new ticket in website and will soon answered.")
end