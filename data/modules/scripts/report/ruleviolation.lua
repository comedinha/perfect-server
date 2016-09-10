ReportType = {
	REPORT_NAME = 0,
	REPORT_STATEMENT = 1,
	REPORT_BOT = 2
}

function onRecvbyte(player, msg, byte)
	local storage = 250820162319
	if player:getStorageValue(storage) - os.time() >= 0 then
		if player:getAccountType() < ACCOUNT_TYPE_TUTOR then
			player:sendTextMessage(MESSAGE_INFO_DESCR, "Exausted, try in ".. player:getStorageValue(storage) - os.time() .." seconds!")
			return false
		end
	end
	player:setStorageValue(storage, os.time() + 180)

	local reportType = msg:getByte()
	local reason = msg:getByte()
	local playerName = msg:getString()
	local comment = msg:getString()
	local translation = ""

	if reportType == ReportType.REPORT_NAME then
		translation = msg:getString()
	elseif reportType == ReportType.REPORT_STATEMENT then
		translation = msg:getString()
		statementId = msg:getU32()
	end
end