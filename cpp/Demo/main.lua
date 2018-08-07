function printValues()
    while true do
		sleep(2000)
		local uiCache = ui.getData()
        toast(string.format("c1:%s\nc2:%s\nc3:%s\nc4:%s\nc5:%s\nc6:%s", 
		uiCache["ed1"], uiCache["sb1"], uiCache["rg1"], uiCache["rg2"], uiCache["rg3"], uiCache["rg4"], uiCache["ed1"]), 2000)
    end
end

function main()
	printValues()
end

main()
