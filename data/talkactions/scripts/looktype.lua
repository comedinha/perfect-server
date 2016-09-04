function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	local lookType = tonumber(param)
	if lookType >= 0 and lookType ~= 1 and lookType ~= 135 and (lookType <= 160 or lookType >= 192) and lookType ~= 411 and lookType ~= 415 and lookType ~= 424 and (lookType <= 438 or lookType >= 441) and (lookType <= 467 or lookType >= 470) and (lookType < 474 or lookType > 485) and lookType ~= 501 and (lookType <= 517 or lookType >= 521) and (lookType <= 523 or lookType >= 526) and lookType ~= 536 and lookType ~= 543 and lookType ~= 549 and (lookType <= 580 or lookType >= 583) and lookType ~= 597 and lookType ~= 616 and lookType ~= 623 and lookType ~= 625 and (lookType <= 637 or lookType >= 644) and (lookType <= 644 or lookType >= 647) and (lookType <= 651 or lookType >= 664) and lookType ~= 678 and (lookType <= 699 or lookType >= 712) and lookType ~= 713 and (lookType <= 717 or lookType >= 720) and (lookType <= 721 or lookType >= 724) and lookType ~= 737 and (lookType <= 740 or lookType >= 745) and lookType ~= 748 and (lookType <= 750 or lookType >= 759) and (lookType <= 763 or lookType >= 842) and lookType ~= 861 and (lookType <= 863 or lookType >= 868) and (lookType <= 870 or lookType >= 873) and lookType ~= 880 and (lookType <= 890 or lookType >= 898) and lookType <= 903 then
		local playerOutfit = player:getOutfit()
		playerOutfit.lookType = lookType
		player:setOutfit(playerOutfit)
	else
		player:sendCancelMessage("A look type with that id does not exist.")
	end
	return false
end
