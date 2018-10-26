require("wx")
Utils = require("impl\\Utils")
GUItulator = require("libGUItulator")

local BaseWindow = {
    window = nil,
    xml = nil
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
function BaseWindow:TryOpen(windowXRCID)
    print("Window id to open: " .. windowXRCID)
    if self.window == nil then
        self.xml = Utils.GetXRCForFile(windowXRCID)
        self:LoadFrame()
    end

    self.window:Show(true)
    self:RegisterOnFrame()
end

function BaseWindow:LoadFrame()
    self.window = wx.wxFrame()
    self.xml:LoadFrame(self.window, wx.NULL, "PatterntableWindow")
    self.window:Connect(wx.wxEVT_CLOSE_WINDOW, function() self:OnClose() end )
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

return BaseWindow