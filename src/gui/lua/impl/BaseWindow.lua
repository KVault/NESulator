require("wx")
Utils = require("impl\\Utils")
GUItulator = require("libGUItulator")

local BaseWindow = {
    window = {}
}

-- The constructor. Most of this stuff is coppied from here:
-- https://www.lua.org/pil/16.2.html
function BaseWindow:new (w)
    w = w or {}
    setmetatable(w, self)
    self.__index = self
    return w
end

-- Simply sets the visibility of the window to false. That will also make it
-- go from the frame loop
function BaseWindow:OnClose()
    if self.window == nil then return end
    self.window:Show(false)
end

-- If the window doesn't exist. i.e. the first time it has been oppened,
-- then the window is created for the specified window id. In any case
-- The window visibility is set to true
function BaseWindow:TryOpen()
    self:InitWidgets()
    self.window:Show(true)
    self:RegisterOnFrame()
end

function BaseWindow:RegisterOnFrame()
    self.window:Connect(wx.wxEVT_UPDATE_UI, function() self:OnFrame() end)
end

-- Base implementation to be called every frame. It looks a bit like a game
-- engine, but in the end most of the windows will make some use of this feature
function BaseWindow:OnFrame()
    print("OnFrame implementation for ".. self .. "hasn't been specified "..
            "This is the default implementation. And you deserve a cat. ")
end

function BaseWindow:InitWidgets()
    print("Base implementation for InitWidgets.")
end

function BaseWindow:InitEvents()
    print("Base implementation for InitEvents")
end

function BaseWindow:DrawBitmap(bmp)
    local memDC = wx.wxMemoryDC()       -- create off screen dc to draw on
    memDC:SelectObject(bmp)             -- select our bitmap to draw into

    DrawPoints(memDC)

    memDC:SelectObject(wx.wxNullBitmap) -- always release bitmap
    memDC:delete() -- ALWAYS delete() any wxDCs created when done
end

return BaseWindow