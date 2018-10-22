require("wx")
Utils = require("impl\\Utils")

local PatterntableWindow = {}
window = nil
xml = nil


function OnClose()
    window:Show(false)
end

function LoadFrame()
    window = wx.wxFrame()
    xml:LoadFrame(window, wx.NULL, "PatterntableWindow")
    window:Connect(wx.wxEVT_CLOSE_WINDOW, OnClose)
end

function TryOpen()
    if window == nil then
        xml = Utils.GetXRCForFile("PatterntableWindow")
        LoadFrame()
    end

    window:Show(true)
end

PatterntableWindow.TryOpen = TryOpen
return PatterntableWindow