function onSay(player, words, param)
	if player:getExhaustion(1000) <= 0 then
		player:setExhaustion(1000, 2)

		if player:isRecording() then
			player:stopRecord()
			player:sendTextMessage(MESSAGE_INFO_DESCR, "You have stopped record.")
			return true
		end

		player:startRecord()
		player:sendTextMessage(MESSAGE_INFO_DESCR, "You have started record your gameplay.")
	else
		player:sendTextMessage(MESSAGE_STATUS_SMALL, 'You\'re exhausted for: '..player:getExhaustion(1000)..' seconds.')
	end
	return false
end
