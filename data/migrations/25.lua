function onUpdateDatabase()
	print("> Updating database to version 25 (Records)")
	db.query("CREATE TABLE IF NOT EXISTS `recordings` ( `id` int(11) NOT NULL AUTO_INCREMENT, `name` varchar(32) NOT NULL, `actions` varchar(10000) NOT NULL, `world_id` int(11) NOT NULL DEFAULT '1', PRIMARY KEY (`id`) ) ENGINE=InnoDB;")
	return true
end
