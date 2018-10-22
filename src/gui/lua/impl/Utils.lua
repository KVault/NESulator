local Utils = {}

-- ---------------------------------------------------------------------------
-- return the path part of the currently executing file
function GetExePath()
    local function findLast(filePath) -- find index of last / or \ in string
        local lastOffset = nil
        local offset = nil
        repeat
            offset = string.find(filePath, "\\") or string.find(filePath, "/")

            if offset then
                lastOffset = (lastOffset or 0) + offset
                filePath = string.sub(filePath, offset + 1)
            end
        until not offset

        return lastOffset
    end

    local filePath = debug.getinfo(1, "S").source

    if string.byte(filePath) == string.byte('@') then
        local offset = findLast(filePath)
        if offset ~= nil then
            -- remove the @ at the front up to just before the path separator
            filePath = string.sub(filePath, 2, offset - 1)
        else
            filePath = "."
        end
    else
        filePath = wx.wxGetCwd()
    end

    return filePath
end

--Returns the XMLResource that holds the current XRC
function GetXRCForFile(xrcFileName)
    local xmlResource = wx.wxXmlResource()
    xmlResource:InitAllHandlers()
    local xrcFileName = utils.GetExePath() .. "/" .. xrcFileName .. ".xrc"

    if not xmlResource:Load(xrcFileName) then
        return nil
    else
        return xmlResource
    end
end

Utils.GetExePath = GetExePath
Utils.GetXRCForFile = GetXRCForFile

return Utils