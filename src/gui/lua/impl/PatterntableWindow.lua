--
-- Created by IntelliJ IDEA.
-- User: jossanta
-- Date: 21/10/2018
-- Time: 21:55
-- To change this template use File | Settings | File Templates.
--

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


function TryOpen(xml_resource)
    if window == nil then
        xml = xml_resource
        LoadFrame()
    end

    window:Show(true)
end