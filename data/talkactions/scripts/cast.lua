function onSay(player, words, param)
	if player:getExhaustion(1000) <= 0 then
		player:setExhaustion(1000, 2)

		if player:isLiveCaster() then
			player:stopLiveCast()
			player:sendTextMessage(MESSAGE_INFO_DESCR, "You have stopped casting.")
			return true
		end

		player:startLiveCast(param)
		
		if param == '' then
			player:sendTextMessage(MESSAGE_INFO_DESCR, "You have started casting your gameplay.")
		else
			player:sendTextMessage(MESSAGE_INFO_DESCR, "You have started casting your gameplay. PASSWORD: " .. param)	
		end
	else
		player:sendTextMessage(MESSAGE_STATUS_SMALL, 'You\'re exhausted for: '..player:getExhaustion(1000)..' seconds.')
	end
	return false
end
